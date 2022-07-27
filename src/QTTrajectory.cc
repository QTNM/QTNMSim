#include "QTTrajectory.hh"

G4Allocator<QTTrajectory>*& myTrajectoryAllocator()
{
  G4ThreadLocalStatic G4Allocator<QTTrajctory>* _instance = nullptr;
  return _instance;
}


QTTrajectory::QTTrajectory(const G4Track* aTrack, std::vector<G4double>& ang)
: G4VTrajectory()
, fAngles(ang)
{
  // for all antenna, first entry
  for (VTcontainer vtcon : fVT) {
    vtcon.clear();
    vtcon.push_back(convertToVT(aTrack->GetPosition()));
  }
}

QTTrajectory::~QTTrajectory()
{
  // for all antenna
  for (VTcontainer vtcon : fVT) {
    vtcon.clear();
  }
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // for all antenna
  for (VTcontainer vtcon : fVT) {
    vtcon.push_back(convertToVT(aStep->GetPostStepPoint()->GetPosition()));
  }
}

std::pair<double,double> QTTrajectory::convertToVT(G4ThreeVector sourcepos)
{
  // insert field at antenna calculation
  // return pair of time, voltage
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
