// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// from G4BorisScheme implementation
//
// Author: Divyansh Tiwari, Google Summer of Code 2022
// Supervision: John Apostolakis,Renee Fatemi, Soon Yung Jun
// --------------------------------------------------------------------
#include <cmath>
#include "G4FieldUtils.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4EquationOfMotion.hh"
#include "G4ThreeVector.hh"

#include "QTEquationOfMotion.hh"
#include "QTBorisScheme.hh"


using namespace field_utils;

QTBorisScheme::QTBorisScheme( G4EquationOfMotion* equation,
                                        G4int nvar )
  : fEquation(equation), fnvar(nvar)
{
  if (nvar <= 0)
  {
    G4Exception("QTBorisScheme::QTBorisScheme()",
                "GeomField0002", FatalException,
                "Invalid number of variables; must be greater than zero!");
  }

  pEqn = dynamic_cast<QTEquationOfMotion*>(fEquation); // requires our object
}

void QTBorisScheme::DoStep(const G4double restMass,const G4double charge, const G4double yIn[], 
                                 G4double yOut[], G4double hstep) const
{
  G4double yOut1Temp[G4FieldTrack::ncompSVEC];
  G4double yOut2Temp[G4FieldTrack::ncompSVEC];
  
  // Used the scheme described in the following paper:
  // https://www.research-collection.ethz.ch/bitstream/handle/20.500.11850/153167/eth-5175-01.pdf?sequence=1
  UpdatePosition(restMass, charge,  yIn, yOut1Temp, hstep/2);
  UpdateVelocity(restMass, charge, yOut1Temp, yOut2Temp, hstep);
  UpdatePosition(restMass, charge, yOut2Temp, yOut, hstep/2);
}

void QTBorisScheme::UpdatePosition(const G4double restMass, const G4double /*charge*/, const G4double yIn[],
                                   G4double yOut[], G4double hstep) const
{
  // Calculations use SI units
  // Particle information
  copy(yOut, yIn);
  
  // Obtaining velocity
  G4ThreeVector momentum_vec =G4ThreeVector(yIn[3],yIn[4],yIn[5]);
  G4double momentum_mag = momentum_vec.mag();
  G4ThreeVector momentum_dir =(1.0/momentum_mag)*momentum_vec;
  
  G4double velocity_mag = momentum_mag*(c_l)/(std::sqrt(sqr(momentum_mag) +sqr(restMass)));
  G4ThreeVector velocity = momentum_dir*velocity_mag;
  
  //Obtaining the time step from the length step
  
  hstep /= velocity_mag*CLHEP::m;
  
  // Updating the Position
  for(G4int i = 0; i <3; i++ )
    {
      G4double pos = yIn[i]/CLHEP::m;
      pos += hstep*velocity[i];
      yOut[i] = pos*CLHEP::m;
    }
}

void QTBorisScheme::UpdateVelocity(const G4double restMass, const G4double charge, const G4double yIn[], 
                                   G4double yOut[], G4double hstep) const
{
  // Calculations use SI units
  //Particle information
  G4ThreeVector momentum_vec =G4ThreeVector(yIn[3],yIn[4],yIn[5]);
  G4double momentum_mag = momentum_vec.mag();
  G4ThreeVector momentum_dir =(1.0/momentum_mag)*momentum_vec;
  
  G4double gamma0 = std::sqrt(sqr(momentum_mag) + sqr(restMass))/restMass;  
  
  G4double mass_SI = (restMass/(c_l*c_l))/CLHEP::kg;

  //Obtaining velocity; SI unit, not beta
  G4double velocity_mag = momentum_mag*(c_l)/(std::sqrt(sqr(momentum_mag) +sqr(restMass)));
  G4ThreeVector velocity = momentum_dir*velocity_mag;
  
  ////Obtaining the time step from the length step
  hstep /= velocity_mag*CLHEP::m; // in [s] with v in SI
  
  // Obtaining the field values
  G4double PositionAndTime[4]; // copy from G4EquationOfMotion.icc
  G4double fieldValue[6] ={0,0,0,0,0,0};
  
  // Position
  PositionAndTime[0] = yIn[0]; // internal units for call?
  PositionAndTime[1] = yIn[1]; // not according to previous call
  PositionAndTime[2] = yIn[2]; // to EvaluateRhsReturnB(yIn,...)
  // Global Time
  PositionAndTime[3] = yIn[7];  // See G4FieldTrack::LoadFromArray

  // use inherited get field value function, get B-field array
  fEquation->GetFieldValue(PositionAndTime, fieldValue) ; // by-passes RHS evaluation, not needed.
  
  //Initializing Vector
  G4ThreeVector B;
  copy(yOut, yIn);
  for( G4int i = 0; i < 3; i++)
    {
      B[i] = fieldValue[i]/CLHEP::tesla; // into SI units
    }

  
  //Boris Algorithm, assume SI units throughout
  // removed this part from original version
  //  G4double qd = hstep*(charge/(2*mass*gamma));
  //  G4ThreeVector h = qd*B;
  //  no E-field: G4ThreeVector u = velocity + qd*E;

  // Try Tom's implementation
  G4double u_n = velocity * gamma0;
  G4double gamma_minus = sqrt(1.0+u_n.mag2()/(c_l*c_l));
  G4double Bnorm = B.mag();
  G4double thetahalf = hstep*Bnorm*(charge/(2*mass_SI*gamma_minus));
  G4ThreeVector h = tan(thetahalf) * B/Bnorm;
  G4ThreeVector radAcc = pEqn->CalcRadiationAcceleration(B, u_n/c_l); // with beta in call
  G4ThreeVector u = u_n + (hstep/2.0)*radAcc; // half-time step acceleration
  //  G4ThreeVector u = velocity; // no loss
  G4double h_l = h.mag2();
  G4ThreeVector s_1 = (2*h)/(1 + h_l);
  G4ThreeVector ud = u + (u + u.cross(h)).cross(s_1);
  radAcc = pEqn->CalcRadiationAcceleration(B, ud/c_l); // for next half-step, beta in call
  G4ThreeVector u_plus = ud + (hstep/2.0)*radAcc; // half-time step acceleration
  G4double gamma_plus = sqrt(1.0+u_plus.mag2()/(c_l*c_l));
  //  G4ThreeVector v_fi = ud; // no loss
  G4ThreeVector v_fi = u_plus/gamma_plus; // no loss
  G4double v_mag = v_fi.mag();
  G4ThreeVector v_dir = v_fi/v_mag;
  // back to G4 units
  G4double momen_mag = (restMass*v_mag)/(std::sqrt(c_l*c_l - v_mag*v_mag));
  G4ThreeVector momen = momen_mag*v_dir;
  
  // Storing the updated momentum
  for(int i = 3; i < 6; i++)
    {
      yOut[i] = momen[i-3];   
    }
}

// ----------------------------------------------------------------------------------

void QTBorisScheme::copy(G4double dst[], const G4double src[]) const
{
  std::memcpy(dst, src, sizeof(G4double) * fnvar);
}

// ----------------------------------------------------------------------------------
// - Methods using the Boris Scheme Stepping to estimate integration error
// ----------------------------------------------------------------------------------
void QTBorisScheme::
StepWithErrorEstimate(const G4double yIn[], G4double restMass, G4double charge, G4double hstep,
                      G4double yOut[], G4double yErr[]) const
{
  // Use two half-steps (comparing to a full step) to obtain output and error estimate
  G4double yMid[G4FieldTrack::ncompSVEC];
  StepWithMidAndErrorEstimate(yIn, restMass, charge, hstep, yMid, yOut, yErr);
}

// ----------------------------------------------------------------------------------

void QTBorisScheme::
StepWithMidAndErrorEstimate(const G4double yIn[],  G4double restMass, G4double charge, G4double hstep,
                                  G4double yMid[], G4double yOut[],   G4double yErr[]
   ) const
{
   G4double halfStep= 0.5*hstep;
   G4double yOutAlt[G4FieldTrack::ncompSVEC];   

   // In a single step
   DoStep(restMass, charge, yIn,  yOutAlt, hstep );

   // Same, and also return mid-point evaluation
   DoStep(restMass, charge, yIn,  yMid, halfStep );
   DoStep(restMass, charge, yMid, yOut, halfStep );

   for( G4int i= 0; i<fnvar; i++ )
   {
      yErr[i] = yOutAlt[i] - yOut[i];
   }
}
