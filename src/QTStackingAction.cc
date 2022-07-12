#include "QTStackingAction.hh"

G4ClassificationOfNewTrack QTStackingAction ::ClassifyNewTrack(const G4Track* aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;

  return classification;
}

void QTStackingAction::NewStage() { ; }

void QTStackingAction::PrepareNewEvent() { ; }
