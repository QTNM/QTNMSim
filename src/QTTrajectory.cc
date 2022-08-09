#include "QTTrajectory.hh"

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
  // start with zero radiation
  vel.set(0.0, 0.0, 0.0);
  acc.set(0.0, 0.0, 0.0);

  // for all antenna, first entry
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i]->push_back(convertToVT(i));
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
  gltime = aStep->getTrack()->GetGlobalTime();
  pos.set(aStep->GetPostStepPoint()->GetPosition());
  vel.set();  // should come from our G4Step
  acc.set();  // should come from our G4Step

  // for all antenna
  for (unsigned int i=0;i<fAngles.size();++i) {
    fVT[i]->push_back(convertToVT(i));
  }
}

std::pair<double,double> QTTrajectory::convertToVT(unsigned int which)
{
  // parameter which as index to antenna array with angles
  // insert field at antenna calculation
  // make use of data members, position, velocity, acceleration
  // return pair of time (already known in gltime), voltage
  // return std::make_pair(gltime, voltage);
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
