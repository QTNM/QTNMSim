#include "QTEquationOfMotion.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

QTEquationOfMotion::QTEquationOfMotion(G4MagneticField *magField)
  : G4Mag_UsualEqRhs(magField)
{
}

QTEquationOfMotion::~QTEquationOfMotion()
{
}

void QTEquationOfMotion::SetChargeMomentumMass(G4ChargeState particleCharge,
					       G4double MomentumXc,
					       G4double mass)
{
  fCof_val = particleCharge.GetCharge()*eplus*c_light ; //  B must be in Tesla
  fMass = mass;
  fCharge = particleCharge.GetCharge();
}

G4ThreeVector QTEquationOfMotion::GetCachedFieldValue()
{
  return G4ThreeVector(0.,0.,0.);
}

G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield, G4ThreeVector mom)
{
  G4double u_sq = mom.mag() / fMass;
  G4double gamma_rel = std::sqrt(1 + u_sq * u_sq / c_light / c_light);
  G4cout << "Omega inputs: usq " << u_sq 
	 << ", received mom " << mom.x() << ", " << mom.y() << ", " << mom.z() 
	 << ", gamma " << gamma_rel 
	 << ", charge " << fCharge 
	 << ", mass " << fMass << G4endl;
  return fCharge * Bfield / fMass / gamma_rel;
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector BField, G4ThreeVector mom)
{
  G4ThreeVector omega = CalcOmegaGivenB(BField, mom);
  return mom.cross(omega);
}
