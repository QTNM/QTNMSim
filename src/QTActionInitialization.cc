#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"
#include "QTTrackingAction.hh"
#include "QTOutputManager.hh"


QTActionInitialization::QTActionInitialization(G4String name, std::vector<G4double> ang)
: G4VUserActionInitialization()
, foutname(std::move(name))
, angles(std::move(ang))
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  auto output = new QTOutputManager(foutname);
  SetUserAction(new QTRunAction(output));
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());
  SetUserAction(new QTTrackingAction(angles));

  auto output = new QTOutputManager(foutname);
  auto event = new QTEventAction((G4int)angles.size(), output);
  SetUserAction(event);

  SetUserAction(new QTRunAction(output));

}
