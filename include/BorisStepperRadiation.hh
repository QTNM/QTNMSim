#ifndef BorisStepperRadiation_HH
#define BorisStepperRadiation_HH

#include "BorisStepper.hh"
#include "QTEquationOfMotion.hh"

class BorisStepperRadiation : public BorisStepper
{
public:

  BorisStepperRadiation(QTEquationOfMotion* EqnOfMot);
  ~BorisStepperRadiation();

  void RadiationReaction(G4ThreeVector initVelocity,
			 G4ThreeVector endVelocity);

private:

  QTEquationOfMotion* fPtrQTEqOfMot;

};

#endif
