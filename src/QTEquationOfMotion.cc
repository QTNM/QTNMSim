#include "QTEquationOfMotion.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

QTEquationOfMotion::QTEquationOfMotion(G4MagneticField *magField)
  : G4Mag_UsualEqRhs(magField)
{
}

QTEquationOfMotion::~QTEquationOfMotion() = default;


void QTEquationOfMotion::SetChargeMomentumMass(G4ChargeState particleCharge,
					       G4double MomentumXc,
					       G4double mass)
{
  // internal G4 units
  fCof_val = particleCharge.GetCharge()*eplus*c_light ; //  B must be in Tesla
  fMass = mass;
  fCharge = particleCharge.GetCharge();
}

G4ThreeVector QTEquationOfMotion::GetCachedFieldValue()
{
  // not used currently
  return G4ThreeVector(0.,0.,0.);
}

G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield, G4ThreeVector beta)
{
  // service function for trajectory
  G4double betamag = beta.mag();
  G4double gamma_rel = 1.0/std::sqrt(1 - betamag*betamag);
  G4cout << "Omega inputs: "
  	 << ", received beta " << beta.x() << ", " << beta.y() << ", " << beta.z() 
  	 << ", gamma " << gamma_rel << G4endl;
    return (fCharge*e_SI) * Bfield / (fMass/kg) / gamma_rel;
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector BField, G4ThreeVector beta)
{
  G4ThreeVector omega = CalcOmegaGivenB(BField, beta);
  return beta.cross(omega) * c_light; // missing radiation acceleration term
}
