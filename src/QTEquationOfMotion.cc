#include "QTEquationOfMotion.hh"

QTEquationOfMotion::QTEquationOfMotion(QTLarmorEMField *magField)
  : G4EqMagElectricField(magField)
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


G4ThreeVector QTEquationOfMotion::CalcRadiationAcceleration(G4ThreeVector Bfield, G4ThreeVector beta)
{
  // calculate acceleration from the radiation reaction force
  // form is from Ford & O'Connell equation in 3D
  G4double m_e     = (fMass/c_squared)/kg; // [kg]
  G4double tau_SI  = e_SI*e_SI/(6.0*pi*eps0_SI*c_SI*c_SI*c_SI*m_e); // 6.26e-24 s
  G4double gamma = 1.0/std::sqrt(1 - beta.mag2());
  G4double factor = tau_SI*gamma*gamma*gamma;
  G4ThreeVector omega = CalcOmegaGivenB(Bfield, beta); // relativistic
  G4ThreeVector vel = beta*c_SI;
  G4ThreeVector mu  = omega-vel.dot(omega)/(c_SI*c_SI)*vel; // order beta^2 non-negligible

  // target vector
  G4ThreeVector acc; // default constructor sets components to 0.0
  acc[0] -= factor*mu[1]*(omega[1]*vel[0]-omega[0]*vel[1]);
  acc[0] += factor*mu[2]*(omega[0]*vel[2]-omega[2]*vel[0]);

  acc[1] -= factor*mu[2]*(omega[2]*vel[1]-omega[1]*vel[2]);
  acc[1] += factor*mu[0]*(omega[1]*vel[0]-omega[0]*vel[1]);

  acc[2] -= factor*mu[0]*(omega[0]*vel[2]-omega[2]*vel[0]);
  acc[2] += factor*mu[1]*(omega[2]*vel[1]-omega[1]*vel[2]);

  return acc;
}


G4ThreeVector QTEquationOfMotion::CalcOmegaGivenB(G4ThreeVector Bfield, G4ThreeVector beta)
{
  // B must arrive in [Tesla]

  // service function for trajectory
  G4double gamma_rel = 1.0/std::sqrt(1 - beta.mag2());
  G4double m_e     = (fMass/c_squared)/kg; // [kg]

  // G4cout << ">>> QTEqn Omega inputs: "
  // 	 << " gamma " << gamma_rel 
  // 	 << " B = " << Bfield.x() << ", " << Bfield.y() << ", " << Bfield.z() 
  // 	 << G4endl;
  return (fCharge*e_SI) * Bfield / m_e / gamma_rel;
}

G4ThreeVector QTEquationOfMotion::CalcAccGivenB(G4ThreeVector Bfield, G4ThreeVector beta)
{
  G4ThreeVector omega = CalcOmegaGivenB(Bfield, beta);
  G4ThreeVector rad_acceleration = CalcRadiationAcceleration(Bfield, beta);

  // G4cout << "QTEqn Acc inputs: "
  // 	 << " omega mag " << omega.mag() 
  // 	 << " radacc = " << rad_acceleration.x() 
  // 	 << ", " << rad_acceleration.y() << ", " << rad_acceleration.z() 
  // 	 << G4endl;
  return beta.cross(omega) * c_SI + rad_acceleration; // added radiation acceleration term
}

G4double QTEquationOfMotion::CalcPowerGivenB(G4ThreeVector Bfield, G4ThreeVector beta)
{
  G4double tau_SI  = e_SI*e_SI/(6.0*pi*eps0_SI*c_SI*c_SI*c_SI); // 6.26e-24 s
  G4ThreeVector acceleration = CalcAccGivenB(Bfield, beta);
  return tau_SI*acceleration.mag2(); // m_e cancelled
}
