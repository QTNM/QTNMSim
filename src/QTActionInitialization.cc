#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"
#include "QATrackingAction.hh"


QTActionInitialization::QTActionInitialization(G4String name, G4int na, std::vector<G4ThreeVector> pos)
: G4VUserActionInitialization()
, foutname(std::move(name))
, nAntenna(na)
, positions(pos)
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  auto event = new QTEventAction;
  SetUserAction(new QTRunAction(event, foutname, nAntenna));
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());
  auto event = new QTEventAction(nAntenna);
  SetUserAction(event);
  SetUserAction(new QTRunAction(event, foutname, nAntenna));
  SetUserAction(new QTTrackingAction(positions);
}
