#include "NATrajectory.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4TwoVector.hh"
#include "G4TrajectoryPoint.hh"
#include "G4ChordFinder.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4ChargeState.hh"
#include "G4UserLimits.hh"


G4Allocator<NATrajectory>*& myTrajectoryAllocator2()
{
  G4ThreadLocalStatic G4Allocator<NATrajectory>* _instance = nullptr;
  return _instance;
}


NATrajectory::NATrajectory(const G4Track* aTrack)
: G4VTrajectory()
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

}

NATrajectory::~NATrajectory()
{
  fOm.clear();
  fKE.clear();
  ft.clear();
  xp.clear();
  yp.clear();
  zp.clear();
  betax.clear();
  betay.clear();
  betaz.clear();
  accx.clear();
  accy.clear();
  accz.clear();
}

void NATrajectory::AppendStep(const G4Step* aStep)
{
  // stop trajectory outside volumes of interest
  if (!(aStep->GetTrack()->GetMaterial()->GetName()=="G4_Galactic" ||
	aStep->GetTrack()->GetMaterial()->GetName()=="matT")) return;

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
  acc = getAcceleration();
  

  fOm.push_back(getOmega().mag()); // angular frequency magnitude
  fKE.push_back(aStep->GetPostStepPoint()->GetKineticEnergy());
  ft.push_back(gltime);
  xp.push_back(pos.x());
  yp.push_back(pos.y());
  zp.push_back(pos.z());
  betax.push_back(beta.x());
  betay.push_back(beta.y());
  betaz.push_back(beta.z());
  accx.push_back(acc.x());
  accy.push_back(acc.y());
  accz.push_back(acc.z());
}

G4ThreeVector NATrajectory::getOmega()
{
  // collect required values
  // note that pEqn returns values in SI units 
  G4double pos_[3]; // interface needs array pointer
  pos_[0] = pos[0]; pos_[1] = pos[1]; pos_[2] = pos[2]; // [mm] default
  G4double B[6]; // interface needs array pointer
  pfieldManager->GetDetectorField()->GetFieldValue(pos_, B);
  G4ThreeVector Bfield = G4ThreeVector( B[0], B[1], B[2] ) / tesla; // [Tesla] explicitly
  // G4cout << "b-field from eqn: " << Bfield.x() << ", " << Bfield.y() << ", " 
  // 	 << Bfield.z() << G4endl;

  return pEqn->CalcOmegaGivenB(Bfield, beta);
}

G4ThreeVector NATrajectory::getAcceleration()
{
  // collect required values
  // note that pEqn returns values in SI units 
  G4double pos_[3]; // interface needs array pointer
  pos_[0] = pos[0]; pos_[1] = pos[1]; pos_[2] = pos[2]; // [mm] default
  G4double B[6]; // interface needs array pointer
  pfieldManager->GetDetectorField()->GetFieldValue(pos_, B);
  G4ThreeVector Bfield = G4ThreeVector( B[0], B[1], B[2] ) / tesla; // [Tesla] explicitly
  // G4cout << "b-field from eqn: " << Bfield.x() << ", " << Bfield.y() << ", " 
  // 	 << Bfield.z() << G4endl;

  return pEqn->CalcAccGivenB(Bfield, beta);
}

void NATrajectory::ShowTrajectory(std::ostream& os) const
{
}

void NATrajectory::DrawTrajectory() const
{
}

G4VTrajectoryPoint* NATrajectory::GetPoint(G4int) const
{
  return nullptr;
}

G4int NATrajectory::GetPointEntries() const
{
  return 0;
}

void NATrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  //  G4cout << "PRINT>>> in merge traj function." << G4endl;
}
