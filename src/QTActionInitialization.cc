#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"
#include "QTTrackingAction.hh"


QTActionInitialization::QTActionInitialization(G4String name, std::vector<G4double> ang)
: G4VUserActionInitialization()
, foutname(std::move(name))
, nAntenna((int)ang.size())
, angles(std::move(ang))
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  auto event = new QTEventAction(nAntenna);
  SetUserAction(new QTRunAction(event, foutname, nAntenna));
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());
  auto event = new QTEventAction(nAntenna);
  SetUserAction(event);
  SetUserAction(new QTRunAction(event, foutname, nAntenna));
  SetUserAction(new QTTrackingAction(angles);
}
