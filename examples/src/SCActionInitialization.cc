#include "SCActionInitialization.hh"
#include "QTOutputManager.hh"
#include "SCDetectorConstruction.hh"
#include "SCPrimaryGeneratorAction.hh"
#include "SCRunAction.hh"
#include "SCSteppingAction.hh"

SCActionInitialization::SCActionInitialization(G4String name,
                                               SCDetectorConstruction *det)
    : G4VUserActionInitialization(), foutname(std::move(name)), fDetector(det) {
}

SCActionInitialization::~SCActionInitialization() = default;

void SCActionInitialization::BuildForMaster() const {
  auto output = new QTOutputManager(foutname);
  SetUserAction(new SCRunAction(fDetector, 0));
}

void SCActionInitialization::Build() const {
  SCPrimaryGeneratorAction *prim = new SCPrimaryGeneratorAction(fDetector);
  SetUserAction(prim);

  SCRunAction *run = new SCRunAction(fDetector, prim);
  SetUserAction(run);

  SetUserAction(new SCSteppingAction());

  auto output = new QTOutputManager(foutname);
}
