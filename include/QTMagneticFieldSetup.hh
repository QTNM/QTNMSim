//
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
/// \file field/field02/include/F02MagneticFieldSetup.hh
/// \brief Definition of the F02MagneticFieldSetup class
//
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef QTMagneticFieldSetup_h
#define QTMagneticFieldSetup_h 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

class G4FieldManager;
class G4ChordFinder;
class QTBorisDriver;
class QTBorisScheme;
class QTEquationOfMotion;
class QTFieldMessenger;

/// A class for control of the Magnetic Field of the detector.
///     The field for this case is uniform.
///
/// The field for this case is uniform.
/// It is simply a 'setup' class that creates the field and necessary
/// other parts

class QTMagneticFieldSetup
{
public:
  QTMagneticFieldSetup(G4ThreeVector);  //  The value of the field
  QTMagneticFieldSetup();               //  A zero field - true value set later

  ~QTMagneticFieldSetup();

  void SetMinStep(G4double s) { fMinStep = s ; }

   // Set/Get Field strength in Geant4 units
  void SetUniformB(); // switch; default true
  inline void SetFieldValue(G4ThreeVector fv) { fFieldVector = fv;}
  void SetFieldZValue(G4double);

   // Set/Get Trap Field in Geant4 units
  void SetBathTubB(); // switch; default false
  inline void SetTrapCurrent(G4double c) { fTrapCurrent = c;}
  inline void SetTrapRadius(G4double r) { fTrapRadius = r;}
  inline void SetCoilsAtZ(G4double z) { fTrapZPos = z;} // 2 coils +- zcoil

   // Set/Get Comsol field map in Geant4 units
  void SetComsolB(); // switch; default false
  inline void SetComsolFileName(G4String s) { fFileName = s;}

  void UpdateAll();
  // allow messenger commands to update values
  //   NOTE:  field and equation must have been created before calling this.

protected:

  // Find the global Field Manager

  G4FieldManager*         GetGlobalFieldManager();

  void SetUpBorisDriver();
  void UpdateBField();
   // Prepare all the classes required for tracking

private:
  G4double                fMinStep;
  G4double                fTrapCurrent;
  G4double                fTrapRadius;
  G4double                fTrapZPos;
  G4bool                  fTest;
  G4bool                  fBathTub;
  G4bool                  fComsol;
  G4String                fFileName;
  G4ThreeVector           fFieldVector;

  G4FieldManager*         fFieldManager;

  G4ChordFinder*          fChordFinder;

  QTEquationOfMotion*     fEquation;

  G4MagneticField*        fEMfield;
 
  QTBorisScheme*          fBStepper;
  QTBorisDriver*          fBDriver;

  QTFieldMessenger*       fFieldMessenger;

};

#endif
