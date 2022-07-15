#include "G4UnitsTable.hh"

#include "QTTrajectory.hh"

G4ThreadLocal G4Allocator<QTTrajectory>* myTrajectoryAllocator = nullptr;

QTTrajectory::QTTrajectory(const G4Track* aTrack, std::vector<G4ThreeVector>& pos)
: G4VTrajectory()
, fPositions(pos)
{
  if (!fVT.empty())
    fVT.clear();
  fVT.push_back(convertToVT(aTrack->GetPosition()));
}

QTTrajectory::~QTTrajectory()
{
  fVT.clear();
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  // just for one antenna, not n
  fVT.push_back(convertToVT(aStep->GetPostStepPoint()->GetPosition()));
}

std::pair<double,double> QTTrajectory::convertToVT(G4ThreeVector)
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
