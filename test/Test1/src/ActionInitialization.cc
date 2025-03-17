#include "ActionInitialization.hh"
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
  SetUserAction(new RunAction(fDetector, 0, foutname));
}

void ActionInitialization::Build() const {
  PrimaryGeneratorAction *prim = new PrimaryGeneratorAction(fDetector);
  SetUserAction(prim);

  RunAction *run = new RunAction(fDetector, prim, foutname);
  SetUserAction(run);

  SetUserAction(new SteppingAction());
}
