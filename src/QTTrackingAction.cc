#include "QTTrackingAction.hh"
#include "QTTrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

QTTrackingAction::QTTrackingAction()
  : G4UserTrackingAction()
{}


QTTrackingAction::~QTTrackingAction() = default;


void QTTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track if requested
  // fpTrackingManager->SetStoreTrajectory(true);
  if(fpTrackingManager->GetStoreTrajectory() > 0)
  {
    fpTrackingManager->SetTrajectory(new QTTrajectory(aTrack));
  }
}

void QTTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
