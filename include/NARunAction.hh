#ifndef NARunAction_h
#define NARunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"

class G4Run;
class G4Timer;
class NAOutputManager;

/// Run action class
///

class NARunAction : public G4UserRunAction
{

public:
  NARunAction(NAOutputManager*);
  ~NARunAction() override;

  virtual void BeginOfRunAction(const G4Run*) override;
  virtual void EndOfRunAction(const G4Run*) override;

private:
  void DefineCommand();

  NAOutputManager*    fOutput = nullptr; // outsource output file storage
  G4Timer*            fTimer  = nullptr;
  G4GenericMessenger* fMessenger = nullptr;
  G4double            fMaxStep; // for transport

};


#endif
