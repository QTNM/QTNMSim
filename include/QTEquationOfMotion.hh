#ifndef QTEquationOfMotion_HH
#define QTEquationOfMotion_HH

#include "G4Mag_UsualEqRhs.hh"
#include "G4ThreeVector.hh"
#include "G4ChargeState.hh"

class QTEquationOfMotion : public G4Mag_UsualEqRhs
{
public:

  QTEquationOfMotion(G4MagneticField *magField);
  ~QTEquationOfMotion() override;

  void SetChargeMomentumMass(G4ChargeState particleCharge,
			     G4double MomentumXc,
			     G4double mass);

  G4ThreeVector GetCachedFieldValue();
  G4ThreeVector CalcOmegaGivenB(G4ThreeVector, G4ThreeVector);
  G4ThreeVector CalcAccGivenB(G4ThreeVector, G4ThreeVector);

private:
  G4double fCof_val;
  G4double fMass;
  G4double fCharge;
};

#endif
