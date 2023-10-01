#include "QTRunAction.hh"
#include "QTOutputManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Timer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"

#include <string>

QTRunAction::QTRunAction(QTOutputManager* out)
: G4UserRunAction()
, fOutput(out)
, fTimer(0)
, fMaxStep(1.0*CLHEP::mm)       // default max step 1 mm
{
  DefineCommand();
}

QTRunAction::~QTRunAction()
{
  delete fMessenger;
}

void QTRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  fOutput->Book(); // set up output

  // set step limit
  G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->SetLargestAcceptableStep(fMaxStep); // limit step length.

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


void QTRunAction::DefineCommand()
{
  // Define /QT/run command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/QT/run/", "run transport control");

  auto& stepCmd = fMessenger->DeclarePropertyWithUnit("maxstep", "mm", fMaxStep,
					      "Set maximum electron transport step in [mm].");
  stepCmd.SetParameterName("step", true);
  stepCmd.SetDefaultValue("1 mm");


}
