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
  fCharge = particleCharge;
}

G4ThreeVector QTEquationOfMotion::GetCachedFieldValue()
{
  return G4ThreeVector(0.,0.,0.);
}

G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield, G4ThreeVector mom)
{
  G4double u_sq = mom.mag() / fMass;
  G4double gamma_rel = std::sqrt(1 + u_sq * u_sq / c_light / c_light);
  return fCharge * Bfield / fMass / gamma_rel;
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector BField, G4ThreeVector mom)
{
  G4ThreeVector omega = CalcOmegaGivenB(BField, mom);
  return mom.cross(omega);
}
