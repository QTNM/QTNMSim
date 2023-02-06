#include "QTEquationOfMotion.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

QTEquationOfMotion::QTEquationOfMotion(G4MagneticField *magField)
  : G4Mag_EqRhs(magField)
{
}

QTEquationOfMotion::~QTEquationOfMotion()
{
}

void QTEquationOfMotion::SetChargeMomentumMass(G4double particleCharge,
					       G4double MomentumXc,
					       G4double mass)
{
  fCof_val = particleCharge*eplus*c_light ; //  B must be in Tesla
  fMass = mass;
}
