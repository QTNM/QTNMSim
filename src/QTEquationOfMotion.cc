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

G4ThreeVector QTEquationOfMotion::GetCachedFieldValue()
{
  return G4ThreeVector(0.,0.,0.);
}

G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield)
{
  return G4ThreeVector(0.,0.,0.);
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector BField)
{
  return G4ThreeVector(0.,0.,0.);
}
