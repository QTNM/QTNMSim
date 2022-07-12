#ifndef QTTrackingAction_h
#define QTTrackingAction_h 1

#include "G4UserTrackingAction.hh"

class QTTrackingAction : public G4UserTrackingAction
{
public:
  QTTrackingAction();
  virtual ~QTTrackingAction(){};

  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);
};

#endif
