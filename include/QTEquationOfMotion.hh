#ifndef QTEquationOfMotion_HH
#define QTEquationOfMotion_HH

#include "G4Mag_UsualEqRhs.hh"
#include "G4ThreeVector.hh"

class QTEquationOfMotion : public G4Mag_UsualEqRhs
{
public:

  QTEquationOfMotion(G4MagneticField *magField);
  ~QTEquationOfMotion();

  void SetChargeMomentumMass(G4double particleCharge,
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
