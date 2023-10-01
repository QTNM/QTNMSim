#ifndef QTMagneticTrap_h
#define QTMagneticTrap_h 1

#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "QTLarmorEMField.hh"

#ifdef HAVE_BOOST
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#else
#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#endif

class QTMagneticTrap : public QTLarmorEMField
{
public:  // with description
  
  QTMagneticTrap(const G4ThreeVector& FieldVector );

  ~QTMagneticTrap() override;

  void GetFieldValue(const G4double yTrack[4],
		     G4double *Field) const override;

  void SetFieldValue(const G4ThreeVector& newFieldValue);

  void SetCurrent(const G4double TrapCurrent);

  void SetRadius(const G4double TrapRadius);

  inline void SetCoilZ(const G4double z) { zpos_ = z;}

private:

  G4double fFieldComponents[6] ;
  G4double radius_;
  G4double current_;
  G4double zpos_;
  G4double b_central_;

  void EvaluateCoils(const G4double yIn[7], G4double fac, G4double field[3]) const;
  void SetCentralField();
};
#endif
