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
  fCof_val = particleCharge.GetCharge()*eplus*c_light ;
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
  // B must arrive in [Tesla]
  // conversion in [kg] not working
  static constexpr G4double m_e = 9.1093837e-31; // [electron mass in [kg]; needed only here

  // service function for trajectory
  G4double betamag2 = beta.mag2();
  G4double gamma_rel = 1.0/std::sqrt(1 - betamag2);
  // G4cout << "Omega inputs: "
  // 	 << " gamma " << gamma_rel 
  // 	 << " B = " << Bfield.x() << ", " << Bfield.y() << ", " << Bfield.z() 
  // 	 << G4endl;
    return (fCharge*e_SI) * Bfield / m_e / gamma_rel;
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector BField, G4ThreeVector beta)
{
  G4double c_SI = c_light/(m/s); // explicit SI units
  G4ThreeVector omega = CalcOmegaGivenB(BField, beta);
  return beta.cross(omega) * c_SI; // missing radiation acceleration term
}
