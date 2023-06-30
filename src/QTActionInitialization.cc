#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"
#include "QTTrackingAction.hh"


QTActionInitialization::QTActionInitialization(G4String name, std::vector<G4double> ang)
: G4VUserActionInitialization()
, foutname(std::move(name))
, angles(std::move(ang))
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  auto event = new QTEventAction((G4int)angles.size());
  SetUserAction(new QTRunAction(event, foutname));
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());
  auto event = new QTEventAction((G4int)angles.size());
  SetUserAction(event);
  SetUserAction(new QTRunAction(event, foutname));
  SetUserAction(new QTTrackingAction(angles));
}
