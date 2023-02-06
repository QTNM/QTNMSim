#ifndef QTEquationOfMotion_HH
#define QTEquationOfMotion_HH

#include "G4Mag_EqRhs.hh"

class QTEquationOfMotion : public G4Mag_EqRhs
{
public:

  QTEquationOfMotion(G4MagneticField *magField);
  ~QTEquationOfMotion();

  void SetChargeMomentumMass(G4double particleCharge,
			     G4double MomentumXc,
			     G4double mass);

private:
  G4double fCof_val;
  G4double fMass;
};

#endif
