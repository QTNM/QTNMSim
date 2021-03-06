#ifndef QTRunAction_h
#define QTRunAction_h 1

#include "QTEventAction.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class QTRunAction : public G4UserRunAction
{
public:
  QTRunAction(QTEventAction* eventAction, G4String name, G4int na);
  virtual ~QTRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  QTEventAction* fEventAction;  // have event information for run
  G4String       fout;          // output file name
  G4int          nAntenna;      // sets ntuple output
};


#endif
