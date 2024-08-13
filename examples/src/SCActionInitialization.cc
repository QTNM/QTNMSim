#include "SCActionInitialization.hh"
#include "SCDetectorConstruction.hh"
#include "SCPrimaryGeneratorAction.hh"
#include "SCRunAction.hh"
#include "QTOutputManager.hh"


SCActionInitialization::SCActionInitialization(G4String name, SCDetectorConstruction* det)
: G4VUserActionInitialization()
, foutname(std::move(name))\
, fDetector(det)
{}

SCActionInitialization::~SCActionInitialization() = default;

void SCActionInitialization::BuildForMaster() const
{
  auto output = new QTOutputManager(foutname);
  SetUserAction(new SCRunAction(fDetector,0));
}

void SCActionInitialization::Build() const
{
  SCPrimaryGeneratorAction* prim = new SCPrimaryGeneratorAction(fDetector);
  SetUserAction(prim);

  SCRunAction* run = new SCRunAction(fDetector,prim);
  SetUserAction(run);

  auto output = new QTOutputManager(foutname);
}
