#include "BorisStepperRadiation.hh"

BorisStepperRadiation::BorisStepperRadiation(QTEquationOfMotion* EqnOfMot)
  : BorisStepper(EqnOfMot),
    fPtrQTEqOfMot(EqnOfMot)
{
};

BorisStepperRadiation::~BorisStepperRadiation()
{
};


void
BorisStepperRadiation::RadiationReaction(G4ThreeVector initVelocity,
					 G4ThreeVector endVelocity)
{
  // Dummy code - do nothing
  endVelocity = initVelocity;
};
