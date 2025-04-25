#include "QTActionInitialization.hh"
#include "QTEventAction.hh"
#include "NAEventAction.hh"
#include "QTPrimaryGeneratorAction.hh"
#include "QTRunAction.hh"
#include "NARunAction.hh"
#include "QTTrackingAction.hh"
#include "NATrackingAction.hh"
#include "QTOutputManager.hh"
#include "NAOutputManager.hh"


QTActionInitialization::QTActionInitialization(G4String name, std::vector<G4double> ang)
: G4VUserActionInitialization()
, foutname(std::move(name))
, angles(std::move(ang))
{}

QTActionInitialization::~QTActionInitialization() = default;

void QTActionInitialization::BuildForMaster() const
{
  if (angles.empty()) { // noAntenna simulation case
    auto output = new NAOutputManager(foutname);
    SetUserAction(new NARunAction(output));
  }
  else {
    auto output = new QTOutputManager(foutname);
    SetUserAction(new QTRunAction(output));
  }
}

void QTActionInitialization::Build() const
{
  // forward detector
  SetUserAction(new QTPrimaryGeneratorAction());

  if (angles.empty()) { // noAntenna simulation case
    SetUserAction(new NATrackingAction());
    auto output = new NAOutputManager(foutname);
    SetUserAction(new NAEventAction(output));
    SetUserAction(new NARunAction(output));
  }
  else {
    SetUserAction(new QTTrackingAction(angles));
    auto output = new QTOutputManager(foutname);
    SetUserAction(new QTEventAction(output));
    SetUserAction(new QTRunAction(output));
  }
}
