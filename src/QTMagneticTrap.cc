#include "QTMagneticTrap.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

QTMagneticTrap::QTMagneticTrap(const G4ThreeVector& FieldVector )
{
  fFieldComponents[0] = FieldVector.x();
  fFieldComponents[1] = FieldVector.y();
  fFieldComponents[2] = FieldVector.z();
  fFieldComponents[3] = 0.0;
  fFieldComponents[4] = 0.0;
  fFieldComponents[5] = 0.0;

  radius_  = 20.0*mm; // 2cm
  current_ = 1 * ampere; // 1 Amp
  zpos_    = 20.0*mm; // +- 2cm
  SetCentralField();
}


QTMagneticTrap::~QTMagneticTrap() = default;


void
QTMagneticTrap::SetFieldValue(const G4ThreeVector& newFieldVector )
{
  fFieldComponents[0] = newFieldVector.x();
  fFieldComponents[1] = newFieldVector.y();
  fFieldComponents[2] = newFieldVector.z();
}

void
QTMagneticTrap::SetCurrent(const G4double TrapCurrent)
{
  current_ = TrapCurrent;
  SetCentralField();
}

void
QTMagneticTrap::SetRadius(const G4double TrapRadius)
{
  radius_ = TrapRadius;
  SetCentralField();
}

void
QTMagneticTrap::SetCentralField()
{
  b_central_ = current_ * CLHEP::mu0 / radius_ / 2.0;
  G4cout << "set trap coil central field [T] at " << b_central_/tesla << G4endl;
}

void QTMagneticTrap::GetFieldValue (const G4double yIn[7],
				       G4double *B  ) const 
{

  G4double field[3];
  // coil at zpos_, can be 0.0 for single coil harmonic trap
  EvaluateCoils(yIn, 1.0, field);
  
  B[0] = fFieldComponents[0] + field[0] ;
  B[1] = fFieldComponents[1] + field[1] ;
  B[2] = fFieldComponents[2] + field[2] ;
  B[3] = 0.0 ;
  B[4] = 0.0 ;
  B[5] = 0.0 ;

  // coil at -zpos_
  if (zpos_!=0.0) {
    EvaluateCoils(yIn, -1.0, field);
    
    B[0]+= field[0] ;
    B[1]+= field[1] ;
    B[2]+= field[2] ;
  }
}

void QTMagneticTrap::EvaluateCoils(const G4double yIn[7], G4double zfactor, G4double field[3]) const
{
  G4double x = yIn[0];
  G4double y = yIn[1];
  G4double z = yIn[2];

  double rad = sqrt(pow(x,2) + pow(y,2));

  if (rad / radius_ < 1e-10) {
    double radius2 = pow(radius_,2);
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = CLHEP::mu0 * current_ * radius2 / (2.0 * pow(radius2 + pow(zfactor*zpos_ - z,2), 1.5));
    return;
  }

  double z_rel = z - zfactor*zpos_;
  double rad_norm = rad / radius_;
  double rad_norm2 = pow(rad_norm, 2);
  double z_norm2 = pow(z_rel / radius_, 2);

  double alpha = pow(1.0 + rad_norm, 2) + z_norm2;
  double root_alpha_pi = sqrt(alpha) * CLHEP::pi;
  double root_beta = sqrt(4.0 * rad_norm / alpha);

#ifdef HAVE_BOOST
  double int_e = boost::math::ellint_2(root_beta);
  double int_k = boost::math::ellint_1(root_beta);
#else
  double int_e = std::comp_ellint_2(root_beta);
  double int_k = std::comp_ellint_1(root_beta);
#endif

  double gamma = alpha - 4.0 * rad_norm;
  double b_r = b_central_ * (int_e * ((1.0 + rad_norm2 + z_norm2) / gamma) - int_k) / root_alpha_pi * (z_rel / rad);
  double b_z = b_central_ * (int_e * ((1.0 - rad_norm2 - z_norm2) / gamma) + int_k) / root_alpha_pi;

  field[0] = b_r * x / rad;
  field[1] = b_r * y / rad;
  field[2] = b_z;
}
