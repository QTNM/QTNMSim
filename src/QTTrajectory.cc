#include "QTTrajectory.hh"
#include "QT_LD_EqnRHS.hh"  // proposed eqn of motion right hand side object

#include "G4LogicalVolumeStore.hh"
#include "G4Tubs.hh"
#include "G4PhysicalConstants.hh"


G4Allocator<QTTrajectory>*& myTrajectoryAllocator()
{
  G4ThreadLocalStatic G4Allocator<QTTrajctory>* _instance = nullptr;
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
{
  fVT = new VTcontainer [fAngles.size()]; // instantiate array
  
  // set up information retrieval from singleton
  pfieldmanager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  fAntennaRad   = G4LogicalVolumeStore::GetInstance()->GetVolume("AntennaLV")->
    dynamic_cast<G4Tubs*>GetSolid()->GetInnerRadius();
  
  // for all antenna, first entry, set zero at start
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i]->push_back(std::make_pair(gltime,0.0));
  }
}

QTTrajectory::~QTTrajectory()
{
  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i]->clear();
  }
  delete [] fVT;
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // take care of units [time] [distance]
  gltime = aStep->getTrack()->GetGlobalTime();
  pos  = aStep->GetPostStepPoint()->GetPosition(); // returns const reference
  // only source beta=v/c enters radiation formulae
  beta = aStep->GetPostStepPoint()->GetMomentumDirection() * aStep->GetPostStepPoint()->GetBeta();

  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i]->push_back(convertToVT(i));
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
  fAntennaNormal = (-fAntennaPos).unit(); // inward direction; to origin

  // ask RHS object, acceleration as const reference
  acc = pfieldmanager->GetChordFinder()->GetIntegrationDriver()->GetEquationOfMotion()->acc();
  G4double omega = pfieldmanager->GetChordFinder()->GetIntegrationDriver()->GetEquationOfMotion()->GetOmega();


}

void QTTrajectory::ShowTrajectory(std::ostream& os) const
{
}

void QTTrajectory::DrawTrajectory() const
{
}

void QTTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
}
