#include "QTRunAction.hh"
#include "QTOutputManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Timer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <string>

QTRunAction::QTRunAction(QTOutputManager* out)
: G4UserRunAction()
, fOutput(out)
, fTimer(0)
{
}

QTRunAction::~QTRunAction() = default;

void QTRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  fOutput->Book(); // set up output

  if (isMaster) {
    fTimer = new G4Timer();
    fTimer->Start();
  }
}

void QTRunAction::EndOfRunAction(const G4Run* aRun)
{
  if (isMaster) {
    fTimer->Stop();
    G4cout << "Master thread time: " << *fTimer << G4endl;
    delete fTimer;
  }

  G4int nofEvents = aRun->GetNumberOfEvent();
  G4cout << "End of Run: number of events to file is " << nofEvents << G4endl;
  fOutput->Save(); // write and close

}
