#include "QTEquationOfMotion.hh"

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

G4ThreeVector QTEquationOfMotion::CalcRadiationAcceleration(G4ThreeVector Bfield, G4ThreeVector beta)
{
  // calculate acceleration from the radiation reaction force
  // form is from Ford & O'Connell equation in 3D
  G4ThreeVector omega = CalcOmegaGivenB(BField, beta);
  G4double denom = 1.0+tau_SI*tau_SI*(omega.dot(omega));

  // target vector
  G4ThreeVector acc;
  acc[0] -= tau_SI*(omega[2]*omega[2]+omega[1]*omega[1])*beta[0]*c_SI;
  acc[0] += tau_SI*omega[0]*(omega[2]*beta[2]+omega[1]*beta[1])*c_SI;

  acc[1] -= tau_SI*(omega[2]*omega[2]+omega[0]*omega[0])*beta[1]*c_SI;
  acc[1] += tau_SI*omega[1]*(omega[2]*beta[2]+omega[0]*beta[0])*c_SI;

  acc[2] -= tau_SI*(omega[0]*omega[0]+omega[1]*omega[1])*beta[2]*c_SI;
  acc[2] += tau_SI*omega[2]*(omega[0]*beta[0]+omega[1]*beta[1])*c_SI;

  acc /= denom;
  return acc;
}


G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield, G4ThreeVector beta)
{
  // B must arrive in [Tesla]

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
  G4ThreeVector omega = CalcOmegaGivenB(BField, beta);
  return beta.cross(omega) * c_SI; // missing radiation acceleration term
}
