#include "G4UnitsTable.hh"

#include "QTTrajectory.hh"

G4ThreadLocal G4Allocator<QTTrajectory>* myTrajectoryAllocator = nullptr;

QTTrajectory::QTTrajectory(const G4Track* aTrack)
: G4VTrajectory()
, fPositionRecord{ new QTTrajectoryPointContainer() }
, fTrackID{ aTrack->GetTrackID() }
, fParentID{ aTrack->GetParentID() }
, fParticleDefinition{ aTrack->GetDefinition() }
, fParticleName{ fParticleDefinition->GetParticleName() }
, fPDGEncoding{ fParticleDefinition->GetPDGEncoding() }
, fVertexName{ aTrack->GetLogicalVolumeAtVertex()->GetName() }
, fVertexPosition{ aTrack->GetVertexPosition() }
{
  fPositionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));
}

QTTrajectory::~QTTrajectory()
{
  size_t i;
  for(i = 0; i < fPositionRecord->size(); i++)
  {
    delete(*fPositionRecord)[i];
  }
  fPositionRecord->clear();

  delete fPositionRecord;
}

void QTTrajectory::ShowTrajectory(std::ostream& os) const
{
}

void QTTrajectory::DrawTrajectory() const
{
}

void QTTrajectory::AppendStep(const G4Step* aStep)
{
  fPositionRecord->push_back(
    new G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition()));
}

void QTTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory)
    return;

  QTTrajectory*   seco = (QTTrajectory*) secondTrajectory;
  G4int           ent  = seco->GetPointEntries();
  //
  // initial point of the second trajectory should not be merged
  for(int i = 1; i < ent; i++)
  {
    fPositionRecord->push_back((*(seco->fPositionRecord))[i]);
  }
  delete(*seco->fPositionRecord)[0];
  seco->fPositionRecord->clear();
}
