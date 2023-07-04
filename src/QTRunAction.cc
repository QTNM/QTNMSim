#include "QTRunAction.hh"
#include "QTEventAction.hh"
#include "QTOutputManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <string>

QTRunAction::QTRunAction(QTOutputManager* out, QTEventAction* fevent)
: G4UserRunAction()
, fOutput(out)
, fEvent(fevent)
{
  fOutput->SetIDVec(fEvent->GetAntennaID()); // signal output where data is stored. 
  fOutput->SetTimeVec(fEvent->GetTimeVec());
  fOutput->SetVoltageVec(fEvent->GetVoltageVec());  
}

QTRunAction::~QTRunAction() = default;

void QTRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  fOutput->Book(); // set up output
}

void QTRunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int nofEvents = aRun->GetNumberOfEvent();
  G4cout << "End of Run: number of events to file is " << nofEvents << G4endl;
  fOutput->Save(); // write and close
}
