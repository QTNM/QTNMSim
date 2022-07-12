#ifndef QTStackingAction_H
#define QTStackingAction_H 1

#include "G4Track.hh"
#include "G4UserStackingAction.hh"
#include "globals.hh"

class QTStackingAction : public G4UserStackingAction
{
public:
  QTStackingAction()          = default;
  virtual ~QTStackingAction() = default;

public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void                       NewStage();
  virtual void                       PrepareNewEvent();

private:
};

#endif
