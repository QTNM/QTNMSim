#include "QTTrackingAction.hh"
#include "QTTrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

QTTrackingAction::QTTrackingAction(std::vector<G4ThreeVector>& pos)
  : G4UserTrackingAction()
  , positions(pos)
{}


void QTTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track
  fpTrackingManager->SetTrajectory(new QTTrajectory(aTrack, positions));
}

void QTTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
