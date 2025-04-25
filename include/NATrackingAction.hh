#ifndef NATrackingAction_h
#define NATrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "G4Types.hh"
#include <vector>

class NATrackingAction : public G4UserTrackingAction
{
public:
  NATrackingAction();
  ~NATrackingAction() override;

  virtual void PreUserTrackingAction(const G4Track*) override;
  virtual void PostUserTrackingAction(const G4Track*) override;

private:

};

#endif
