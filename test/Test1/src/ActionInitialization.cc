#include "ActionInitialization.hh"
#include "QTOutputManager.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(G4String name,
                                               DetectorConstruction *det)
    : G4VUserActionInitialization(), foutname(std::move(name)), fDetector(det) {
}

ActionInitialization::~ActionInitialization() = default;

void ActionInitialization::BuildForMaster() const {
  auto output = new QTOutputManager(foutname);
  SetUserAction(new RunAction(fDetector, 0));
}

void ActionInitialization::Build() const {
  PrimaryGeneratorAction *prim = new PrimaryGeneratorAction(fDetector);
  SetUserAction(prim);

  RunAction *run = new RunAction(fDetector, prim);
  SetUserAction(run);

  SetUserAction(new SteppingAction());

  auto output = new QTOutputManager(foutname);
}
