#ifndef QTEquationOfMotion_HH
#define QTEquationOfMotion_HH

#include "G4Mag_UsualEqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4ChargeState.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


class QTEquationOfMotion : public G4Mag_UsualEqRhs
{
public:

  QTEquationOfMotion(G4MagneticField *magField);
  ~QTEquationOfMotion() override;

  void SetChargeMomentumMass(G4ChargeState particleCharge,
			     G4double MomentumXc,
			     G4double mass);

  G4ThreeVector GetCachedFieldValue();
  G4ThreeVector CalcRadiationAcceleration(G4ThreeVector, G4ThreeVector);
  G4ThreeVector CalcOmegaGivenB(G4ThreeVector, G4ThreeVector);
  G4ThreeVector CalcAccGivenB(G4ThreeVector, G4ThreeVector);
  G4double      CalcPowerGivenB(G4ThreeVector, G4ThreeVector);

private:
  G4double fCof_val;
  G4double fMass;
  G4double fCharge;

  static constexpr G4double c_SI    = c_light/(m/s); // explicit SI units
  static constexpr G4double eps0_SI = epsilon0/(farad/m); // explicit SI units
};

#endif
