#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"


QTActionInitialization::QTActionInitialization(G4String name)
: G4VUserActionInitialization()
, foutname(std::move(name))
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  auto event = new QTEventAction;
  SetUserAction(new QTRunAction(event, foutname));
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());
  auto event = new QTEventAction;
  SetUserAction(event);
  SetUserAction(new QTRunAction(event, foutname));
}
