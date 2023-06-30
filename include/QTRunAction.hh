#ifndef QTRunAction_h
#define QTRunAction_h 1

#include "QTEventAction.hh"

#include "G4RootAnalysisManager.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class QTRunAction : public G4UserRunAction
{
  using G4AnalysisManager = G4RootAnalysisManager;

public:
  QTRunAction(QTEventAction* eventAction, G4String name);
  virtual ~QTRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  QTEventAction* fEventAction;  // have event information for run
  G4String       fout;          // output file name
};


#endif
