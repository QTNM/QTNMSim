#include "SCActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTOutputManager.hh"


SCActionInitialization::SCActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

SCActionInitialization::~SCActionInitialization() = default;

void SCActionInitialization::BuildForMaster() const
{
  auto output = new QTOutputManager(foutname);
}

void SCActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());

  auto output = new QTOutputManager(foutname);

}
