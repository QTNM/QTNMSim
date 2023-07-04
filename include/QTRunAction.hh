#ifndef QTRunAction_h
#define QTRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class QTEventAction;
class QTOutputManager;

/// Run action class
///

class QTRunAction : public G4UserRunAction
{

public:
  QTRunAction(QTOutputManager*, QTEventAction*);
  virtual ~QTRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  QTOutputManager* fOutput = nullptr; // outsource output file storage
  QTEventAction*   fEvent  = nullptr; // event action info
};


#endif
