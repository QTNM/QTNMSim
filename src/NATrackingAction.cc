#include "NATrackingAction.hh"
#include "NATrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

NATrackingAction::NATrackingAction()
  : G4UserTrackingAction()
{}


NATrackingAction::~NATrackingAction() = default;


void NATrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track if requested
  // fpTrackingManager->SetStoreTrajectory(true);
  if(fpTrackingManager->GetStoreTrajectory() > 0)
  {
    fpTrackingManager->SetTrajectory(new NATrajectory(aTrack));
  }
}

void NATrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
