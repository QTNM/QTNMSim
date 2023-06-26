#include "QTTrajectory.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4Tubs.hh"
#include "G4PhysicalConstants.hh"
#include "G4TrajectoryPoint.hh"
#include "G4ChordFinder.hh"
#include "G4ChargeState.hh"

G4Allocator<QTTrajectory>*& myTrajectoryAllocator()
{
  G4ThreadLocalStatic G4Allocator<QTTrajectory>* _instance = nullptr;
  return _instance;
}


QTTrajectory::QTTrajectory(const G4Track* aTrack, std::vector<G4double>& ang)
: G4VTrajectory()
, fAngles(ang)
, vpos(aTrack->GetVertexPosition())
, vmom(aTrack->GetVertexMomentumDirection())
, venergy(aTrack->GetVertexKineticEnergy())
, pos(aTrack->GetPosition())
, gltime(aTrack->GetGlobalTime())
, ParticleName(aTrack->GetDefinition()->GetParticleName())
, PDGCharge(aTrack->GetDefinition()->GetPDGCharge())
, PDGEncoding(aTrack->GetDefinition()->GetPDGEncoding())
, fTrackID(aTrack->GetTrackID())
, fParentID(aTrack->GetParentID())
, initialMomentum(aTrack->GetMomentum())
{
  positionRecord = new G4TrajectoryPointContainer();

  // Following is for the first trajectory point
  positionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));

  fVT = new VTcontainer [fAngles.size()]; // instantiate array
  
  // set up information retrieval from singleton
  pfieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  pEqn = dynamic_cast<QTEquationOfMotion*>(pfieldManager->GetChordFinder()->GetIntegrationDriver()->GetEquationOfMotion());

  // set up charge info on particle at start of trajectory
  G4ChargeState chargeState(aTrack->GetDynamicParticle()->GetCharge(),0.,0.,0.,0.);
  pEqn->SetChargeMomentumMass(chargeState, aTrack->GetDynamicParticle()->GetTotalMomentum(),
			      aTrack->GetDynamicParticle()->GetMass());

  // geometry
  G4Tubs*  Tubs = dynamic_cast<G4Tubs*>(G4LogicalVolumeStore::GetInstance()->GetVolume("AntennaLV")->GetSolid());
  fAntennaRad   = Tubs->GetInnerRadius();
  
  // for all antenna, first entry, set zero at start
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i].push_back(std::make_pair(gltime,0.0));
  }
}

QTTrajectory::~QTTrajectory()
{
  if (positionRecord != nullptr) {
    for(std::size_t i=0; i<positionRecord->size(); ++i)
      {
	delete  (*positionRecord)[i];
      }
    positionRecord->clear();
    delete positionRecord;
  }

  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i].clear();
  }
  delete [] fVT;
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // take care of units [time] [distance]
  gltime = aStep->GetTrack()->GetGlobalTime();
  pos  = aStep->GetPostStepPoint()->GetPosition(); // returns const reference
  mom  = aStep->GetPostStepPoint()->GetMomentum();
  // only source beta=v/c enters radiation formulae
  beta = aStep->GetPostStepPoint()->GetMomentumDirection() * aStep->GetPostStepPoint()->GetBeta();
  // G4cout << "PRINT>>> in append step, data: t= " << gltime/CLHEP::ns << ", pos " 
  // 	 << pos.x()/CLHEP::mm << ", " << pos.y()/CLHEP::mm << ", " << pos.z()/CLHEP::mm << G4endl;
  // G4cout << "PRINT>>> beta= " << beta.x() << ", " << beta.y() << ", " << beta.z() << G4endl;
  // G4cout << "PRINT>>> momentum= " << mom.x() << ", " << mom.y() << ", " << mom.z() << G4endl;

  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i].push_back(convertToVT(i));
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
  // means no repeat calculation.
  // likely needs: dynamic_cast<QT_LD_EqnRHS*>GetEquationOfMotion()->acc()
  // since method for acceleration access does not exist by default. Make ourselves.
  fAntennaPos.setRThetaPhi(fAntennaRad, CLHEP::halfpi, fAngles[which]/360.0 * CLHEP::twopi);
  // G4cout << "antenna pos: " << fAntennaPos.x()/CLHEP::mm 
  // 	 << ", " << fAntennaPos.y()/CLHEP::mm << ", " 
  // 	 << fAntennaPos.z()/CLHEP::mm << G4endl;
  fAntennaNormal = (-fAntennaPos).unit(); // inward direction; to origin
  // G4cout << "antenna normal: " << fAntennaNormal.x() 
  // 	 << ", " << fAntennaNormal.y() << ", " 
  // 	 << fAntennaNormal.z() << G4endl;

  // collect required values
  // TODO - this would be better done using the actual field object
  G4double pos_[3];
  pos_[0] = pos[0];pos_[1] = pos[1];pos_[2] = pos[2];
  G4double B[3];
  pEqn->GetFieldValue(pos_, B);
  G4ThreeVector Bfield = G4ThreeVector( B[0], B[1], B[2] );
  // G4cout << "b-field from eqn: " << Bfield.x() << ", " << Bfield.y() << ", " 
  // 	 << Bfield.z()/CLHEP::tesla << G4endl;
  // END TODO
  G4double omega = pEqn->CalcOmegaGivenB(Bfield, mom).mag();
  G4cout << "Omega magn. = " << omega << G4endl;
  acc = pEqn->CalcAccGivenB(Bfield, mom);
  G4cout << "acceleration= (" << acc.x() 
	 << ", " << acc.y() << ", " 
	 << acc.z() << ")" << G4endl;

  G4double wvlg  = CLHEP::c_light / (omega / CLHEP::twopi);
  G4double fac   = CLHEP::electron_charge / (4.0*CLHEP::pi*CLHEP::epsilon0*CLHEP::c_light);
  G4double dist  = (fAntennaPos - pos).mag();
  //  G4cout << "antenna - charge distance = " << dist << G4endl;
  G4ThreeVector Runit = (fAntennaPos - pos).unit();
  G4double dummy = 1.0 - Runit.dot(beta);
  fac /= dummy*dummy*dummy;

  G4ThreeVector relFarEField = fac*(Runit.cross((Runit-beta).cross(acc/CLHEP::c_light))) / dist;
  // G4cout << "E-field1= (" << relFarEField.x() 
  // 	 << ", " << relFarEField.y() << ", " 
  // 	 << relFarEField.z() << ")" << G4endl;
  G4ThreeVector relNearEField = fac*((1.0-beta.mag2())*(Runit-beta)) / (dist*dist);
  // G4cout << "E-field2= (" << relNearEField.x() 
  // 	 << ", " << relNearEField.y() << ", " 
  // 	 << relNearEField.z() << ")" << G4endl;
  // assume half wave dipole eff length as wvlg/pi
  G4double voltage = wvlg/CLHEP::pi * (relFarEField+relNearEField).dot(fAntennaNormal);
  
  G4cout << "append step called, t [ns], v: " << gltime/CLHEP::ns << ", " << voltage << G4endl;
  return std::make_pair(gltime, voltage);
}

void QTTrajectory::ShowTrajectory(std::ostream& os) const
{
}

void QTTrajectory::DrawTrajectory() const
{
}

void QTTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  G4cout << "PRINT>>> in merge traj function." << G4endl;
  if(secondTrajectory == nullptr) return;

  QTTrajectory* seco = (QTTrajectory*)secondTrajectory;
  G4int ent = seco->GetPointEntries();
  for(G4int i=1; i<ent; ++i) // initial pt of 2nd trajectory shouldn't be merged
  {
    positionRecord->push_back((*(seco->positionRecord))[i]);
  }
  delete (*seco->positionRecord)[0];
  seco->positionRecord->clear();
}
