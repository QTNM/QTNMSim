#include "QTTrackingAction.hh"
#include "QTTrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

QTTrackingAction::QTTrackingAction(std::vector<G4double> ang)
  : G4UserTrackingAction()
  , angles(ang)
{}


QTTrackingAction::~QTTrackingAction() = default;


void QTTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track if requested
  // fpTrackingManager->SetStoreTrajectory(true);
  if(fpTrackingManager->GetStoreTrajectory() > 0)
  {
    fpTrackingManager->SetTrajectory(new QTTrajectory(aTrack, angles));
  }
}

void QTTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
