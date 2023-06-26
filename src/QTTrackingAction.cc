#include "QTTrackingAction.hh"
#include "QTTrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

QTTrackingAction::QTTrackingAction(std::vector<G4double> ang)
  : G4UserTrackingAction()
  , angles(ang)
{}


void QTTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new QTTrajectory(aTrack, angles));
}

void QTTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
