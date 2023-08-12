#include "QTTrajectory.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4TrajectoryPoint.hh"
#include "G4ChordFinder.hh"
#include "G4ChargeState.hh"
#include "G4PropagatorInField.hh"
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

  // avoid large first step
  G4double maxStep = G4LogicalVolumeStore::GetInstance()->GetVolume("worldLV")->GetUserLimits()->GetMaxAllowedStep(*aTrack);
  G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->SetLargestAcceptableStep(maxStep-1.0*micrometer); // just below the user limit by 1 mum.

  // set up charge info on particle at start of trajectory
  G4ChargeState chargeState(aTrack->GetDynamicParticle()->GetCharge(),0.,0.,0.,0.);
  pEqn->SetChargeMomentumMass(chargeState, aTrack->GetDynamicParticle()->GetTotalMomentum(),
			      aTrack->GetDynamicParticle()->GetMass());

  // geometry
  G4Tubs*  Tubs = dynamic_cast<G4Tubs*>(G4LogicalVolumeStore::GetInstance()->GetVolume("AntennaLV")->GetSolid());
  fAntennaRad   = Tubs->GetInnerRadius(); // [mm] default
  
  // for all antenna, first entry, set zero at start
  for (unsigned int i=0;i<fAngles.size();++i) {
    fAntennaID.push_back((G4int)i);            // which antenna
    fVT.push_back(std::make_pair(gltime,0.0)); // data for antenna signal
  }
}

QTTrajectory::~QTTrajectory()
{
  fVT.clear();
  fAntennaID.clear();
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // stop trajectory outside volumes of interest
  if (!(aStep->GetTrack()->GetMaterial()->GetName()=="G4_Galactic" ||
	aStep->GetTrack()->GetMaterial()->GetName()=="matT")) return;

  // take care of units [time] [distance]
  gltime = aStep->GetTrack()->GetGlobalTime(); // [ns] default
  pos  = aStep->GetPostStepPoint()->GetPosition(); // [mm] default

  // only source beta=v/c enters radiation formulae
  beta = aStep->GetPostStepPoint()->GetMomentumDirection() * aStep->GetPostStepPoint()->GetBeta();

  // G4cout << "PRINT>>> in append step, data: t= " << gltime/CLHEP::ns << ", pos " 
  // 	 << pos.x()/CLHEP::mm << ", " << pos.y()/CLHEP::mm << ", " << pos.z()/CLHEP::mm << G4endl;
  // G4cout << "PRINT>>> beta= " << beta.x() << ", " << beta.y() << ", " << beta.z() << G4endl;

  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fAntennaID.push_back((G4int)i);            // which antenna
    fVT.push_back(convertToVT(i));
  }
}

std::pair<double,double> QTTrajectory::convertToVT(unsigned int which)
{
  // parameter which as index to antenna array with angles
  // make use of data members, position, velocity, acceleration
  // return pair of time (already known in gltime), voltage
  // return std::make_pair(gltime, voltage);
  
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
  pos_[0] = pos[0];pos_[1] = pos[1];pos_[2] = pos[2]; // [mm] default
  G4double B[3]; // interface needs array pointer
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
  
  // G4cout << "append step called, (t [ns], v [V]): " << gltime << ", " << voltage << G4endl;
  return std::make_pair(gltime, voltage);
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
