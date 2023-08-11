#ifndef QTRunAction_h
#define QTRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class G4Timer;
class QTOutputManager;

/// Run action class
///

class QTRunAction : public G4UserRunAction
{

public:
  QTRunAction(QTOutputManager*);
  ~QTRunAction() override;

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  QTOutputManager* fOutput = nullptr; // outsource output file storage
  G4Timer*         fTimer  = nullptr;

};


#endif
