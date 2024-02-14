#ifndef QTTrackingAction_h
#define QTTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"
#include <vector>

class QTTrackingAction : public G4UserTrackingAction
{
public:
  QTTrackingAction();
  ~QTTrackingAction() override;

  virtual void PreUserTrackingAction(const G4Track*) override;
  virtual void PostUserTrackingAction(const G4Track*) override;

private:

};

#endif
