#include "QTTrajectory.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4TrajectoryPoint.hh"
#include "G4ChordFinder.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4ChargeState.hh"
#include "G4UserLimits.hh"


G4Allocator<QTTrajectory>*& myTrajectoryAllocator()
{
  G4ThreadLocalStatic G4Allocator<QTTrajectory>* _instance = nullptr;
  return _instance;
}


QTTrajectory::QTTrajectory(const G4Track* aTrack, std::vector<G4double>& ang)
: G4VTrajectory()
, fAngles(ang)
, initialPos(aTrack->GetPosition()) // set all constant starter values
, initialEnergy(aTrack->GetKineticEnergy())
, gltime(aTrack->GetGlobalTime())
, ParticleName(aTrack->GetDefinition()->GetParticleName())
, PDGCharge(aTrack->GetDefinition()->GetPDGCharge())
, PDGEncoding(aTrack->GetDefinition()->GetPDGEncoding())
, fTrackID(aTrack->GetTrackID())
, fParentID(aTrack->GetParentID())
, initialMomentum(aTrack->GetMomentum())
{
  // set up information retrieval from singletons
  pfieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  pEqn = dynamic_cast<QTEquationOfMotion*>(pfieldManager->GetChordFinder()->GetIntegrationDriver()->GetEquationOfMotion());

  // set up charge info on particle at start of trajectory
  G4ChargeState chargeState(aTrack->GetDynamicParticle()->GetCharge(),0.,0.,0.,0.);
  pEqn->SetChargeMomentumMass(chargeState, aTrack->GetDynamicParticle()->GetTotalMomentum(),
			      aTrack->GetDynamicParticle()->GetMass());

  // geometry
  G4Tubs*  Tubs = dynamic_cast<G4Tubs*>(G4LogicalVolumeStore::GetInstance()->GetVolume("AntennaLV")->GetSolid());
  fAntennaRad   = Tubs->GetInnerRadius(); // [mm] default
  
}

QTTrajectory::~QTTrajectory()
{
  fST.clear();
  fKE.clear();
  fOm.clear();
  fVT.clear();
  fAntennaID.clear();
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // stop trajectory outside volumes of interest
  if (!(aStep->GetTrack()->GetMaterial()->GetName()=="G4_Galactic" ||
	aStep->GetTrack()->GetMaterial()->GetName()=="matT")) return;

  // stop trajectory below threshold vertex kinetic energy
  if (GetInitialEnergy()/keV <= 1.5) return;

  // take care of units [time] [distance]
  // observed steps with equal time values -> prevent; time must be larger than previous
  // appear to be boundary steps
  if (gltime>=aStep->GetTrack()->GetGlobalTime()) return; // avoid equal time storage
  else
    gltime = aStep->GetTrack()->GetGlobalTime(); // [ns] default
  // G4cout << ">> Traj app step: global time [G4]: " << gltime << G4endl;;

  // info
  // G4cout << ">> Traj: pre  step KE: " << aStep->GetPreStepPoint()->GetKineticEnergy()/eV << G4endl;;
  // G4cout << ">> Traj: post step KE: " << aStep->GetPostStepPoint()->GetKineticEnergy()/eV << G4endl;;

  pos  = aStep->GetPostStepPoint()->GetPosition(); // [mm] default

  // only source beta=v/c enters radiation formulae
  beta = aStep->GetPostStepPoint()->GetMomentumDirection() * aStep->GetPostStepPoint()->GetBeta();

  // low-cost request, repeated here for omega
  G4double pos_[3];
  pos_[0] = pos[0]; pos_[1] = pos[1]; pos_[2] = pos[2]; // [mm] default
  G4double B[6]; // interface needs array pointers
  pfieldManager->GetDetectorField()->GetFieldValue(pos_, B);
  G4ThreeVector Bfield = G4ThreeVector( B[0], B[1], B[2] ) / tesla; // [Tesla] explicitly
  fOm.push_back(pEqn->CalcOmegaGivenB(Bfield, beta).mag());
  fKE.push_back(aStep->GetPostStepPoint()->GetKineticEnergy());
  fST.push_back(gltime); // [ns] by default

  for (unsigned int i=0;i<fAngles.size();++i) {
    fAntennaID.push_back((G4int)i);            // which antenna
    fVT.push_back(convertToVT(i));
  }
}

std::pair<double,double> QTTrajectory::convertToVT(unsigned int which)
{
  // parameter which as index to antenna array with angles
  // make use of data members, position, velocity, acceleration
  // return pair of time, voltage
  
  // retrieve required info from outside, propose EqnOfMotion object.
  // need frequency and acceleration
  // both known at every point by EqnOfMotion - store and offer for access.

  G4ThreeVector antennaPos;
  antennaPos.setRThetaPhi(fAntennaRad, halfpi, fAngles[which]/360.0 * twopi); // [mm]
  // G4cout << "antenna pos: " << antennaPos.x()/CLHEP::mm 
  // 	 << ", " << antennaPos.y()/CLHEP::mm << ", " 
  // 	 << antennaPos.z()/CLHEP::mm << G4endl;

  G4ThreeVector antennaNormal = (-antennaPos).unit(); // inward direction; to origin, not used yet
  G4TwoVector   antennaArm(antennaPos); // discard z-component
  
  G4ThreeVector antennaPolarisation = G4ThreeVector(antennaArm.orthogonal()).unit(); // along the dipole
  // G4cout << "antenna polarisation: " << antennaPolarisation.x() 
  // 	 << ", " << antennaPolarisation.y() << ", " 
  // 	 << antennaPolarisation.z() << G4endl;

  // collect required values
  // DONE - this would be better done using the actual field object
  G4double pos_[3];
  pos_[0] = pos[0]; pos_[1] = pos[1]; pos_[2] = pos[2]; // [mm] default
  G4double B[6]; // interface needs array pointer
  pfieldManager->GetDetectorField()->GetFieldValue(pos_, B);
  G4ThreeVector Bfield = G4ThreeVector( B[0], B[1], B[2] ) / tesla; // [Tesla] explicitly
  // G4cout << "b-field from eqn: " << Bfield.x() << ", " << Bfield.y() << ", " 
  // 	 << Bfield.z() << G4endl;

  G4double omega = pEqn->CalcOmegaGivenB(Bfield, beta).mag();
  //  G4cout << "Omega magn. = " << omega << G4endl; // needed in [Hz]

  acc = pEqn->CalcAccGivenB(Bfield, beta);
  // G4cout << "acceleration= (" << acc.x()
  // 	 << ", " << acc.y() << ", " 
  // 	 << acc.z() << ")" << G4endl;

  G4double wvlg  = c_SI / (omega / twopi);
  //  G4cout << "wavelength [m] = " << wvlg << G4endl;

  G4double fac   = electron_charge*e_SI / (4.0*pi*eps0_SI*c_SI);
  G4double dist  = (antennaPos - pos).mag() / m; // [m] SI explicit
  //  G4cout << "antenna - charge distance [m] = " << dist << G4endl;

  G4ThreeVector Runit = (antennaPos - pos).unit();
  G4double dummy = 1.0 - Runit.dot(beta);
  // modify prefactor
  fac /= dummy*dummy*dummy;

  G4ThreeVector relFarEField = fac*(Runit.cross((Runit-beta).cross(acc/c_SI))) / dist;
  // G4cout << "E-field far= (" << relFarEField.x() 
  // 	 << ", " << relFarEField.y() << ", " 
  // 	 << relFarEField.z() << ")" << G4endl;

  G4ThreeVector relNearEField = fac*c_SI*((1.0-beta.mag2())*(Runit-beta)) / (dist*dist);
  // G4cout << "E-field near= (" << relNearEField.x() 
  // 	 << ", " << relNearEField.y() << ", " 
  // 	 << relNearEField.z() << ")" << G4endl;

  // assume half wave dipole eff length as wvlg/pi
  G4double voltage = wvlg/pi * (relFarEField+relNearEField).dot(antennaPolarisation);

  // time transform, source local to antenna local
  G4double antenna_time = gltime + dist/c_m_per_ns;
  return std::make_pair(antenna_time, voltage);
}

void QTTrajectory::ShowTrajectory(std::ostream& os) const
{
}

void QTTrajectory::DrawTrajectory() const
{
}

G4VTrajectoryPoint* QTTrajectory::GetPoint(G4int) const
{
  return nullptr;
}

G4int QTTrajectory::GetPointEntries() const
{
  return 0;
}

void QTTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  //  G4cout << "PRINT>>> in merge traj function." << G4endl;
}
