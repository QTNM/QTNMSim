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
/// \file field/field02/src/F02ElectricFieldSetup.cc
/// \brief Implementation of the F02ElectricFieldSetup class
//
//
//
//   User Field class implementation.
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "QTMagneticFieldSetup.hh"
#include "QTFieldMessenger.hh"

#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "QTEquationOfMotion.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ChordFinder.hh"

// #include "BorisStepper.hh"
#include "G4BorisScheme.hh"
#include "G4BorisDriver.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//  Constructors:

QTMagneticFieldSetup::QTMagneticFieldSetup()
 : fMinStep(0.010*mm),  // minimal step of 10 microns
   fFieldManager(0),
   fChordFinder(0),
   fEquation(0),
   fEMfield(0),
   fStepper(0),
   fIntgrDriver(0),
   fBStepper(0),
   fBDriver(0),
   fFieldMessenger(nullptr)   
{
  G4ThreeVector fieldVector( 0.0, 0.0, 1.0 * CLHEP::tesla);

  fEMfield = new G4UniformMagField(fieldVector);
  fEquation = new QTEquationOfMotion(fEMfield);

  fFieldManager = GetGlobalFieldManager();

  //  UpdateIntegrator();
  SetUpBorisDriver();
  fFieldMessenger = new QTFieldMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTMagneticFieldSetup::QTMagneticFieldSetup(G4ThreeVector fieldVector)
  : fMinStep(0.010*mm),  // minimal step of 10 microns
    fFieldManager(0),
    fChordFinder(0),
    fEquation(0),
    fEMfield(0),
    fStepper(0),
    fIntgrDriver(0),
    fBStepper(0),
    fBDriver(0),
    fFieldMessenger(nullptr)
{
  fEMfield = new G4UniformMagField(fieldVector);

  fEquation = new QTEquationOfMotion(fEMfield);

  fFieldManager = GetGlobalFieldManager();

  //  UpdateIntegrator();
  SetUpBorisDriver();

  fFieldMessenger = new QTFieldMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTMagneticFieldSetup::~QTMagneticFieldSetup()
{

  delete fFieldMessenger; fFieldMessenger= nullptr;
   // Delete the messenger first, to avoid messages to deleted classes!
  
  delete fChordFinder;  fChordFinder= nullptr;
  delete fStepper;      fStepper = nullptr;
  delete fBStepper;     fBStepper = nullptr;
  delete fEquation;     fEquation = nullptr;
  delete fEMfield;      fEMfield = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void QTMagneticFieldSetup::SetUpBorisDriver()
{
  // set up Boris driver from Geant4.11, follows example field01
  G4cout << " F01FieldSetup::CreateAndSetupBorisDriver() called. " << G4endl;   
  G4cout << "   1. Creating Scheme (Stepper)."  << G4endl;
  fBStepper = new G4BorisScheme(fEquation);
  G4cout << "   2. Creating Driver."  << G4endl;
  fBDriver  = new G4BorisDriver(fMinStep, fBStepper);

  G4cout  << "  3. Creating ChordFinder."  << G4endl;
  fChordFinder = new G4ChordFinder( fBDriver );

  G4cout  << "  4. Updating Field Manager (with ChordFinder, field)."  << G4endl;
  fFieldManager->SetChordFinder( fChordFinder );
  fFieldManager->SetDetectorField(fEMfield );

}


void QTMagneticFieldSetup::UpdateAll()
{
  // It must be possible to call 'again' after an alternative stepper
  //   has been chosen, or other changes have been made
  // Focus here are G4 Boris objects only.
  assert(fEquation!=nullptr);

  G4cout<< " QTMagneticFieldSetup: The minimal step is equal to "
        << fMinStep/mm << " mm" << G4endl;

  if (fChordFinder) {
     delete fChordFinder;
     fChordFinder= nullptr;
     // The chord-finder's destructor deletes the driver
     fBDriver= nullptr;
  }
  
  // Currently driver does not 'own' stepper      ( 17.05.2017 J.A. )
  //   -- so this stepper is still a valid object after this

  if( fBStepper ) {
     delete fBStepper;
     fBStepper = nullptr;
  }

  SetUpBorisDriver();
}


void QTMagneticFieldSetup::SetFieldZValue(G4double fieldValue)
{
  // Set the value of the Global Field to fieldValue along Z
  G4ThreeVector fieldVector( 0.0, 0.0, fieldValue );

  SetFieldValue( fieldVector );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void QTMagneticFieldSetup::SetFieldValue(G4ThreeVector fieldVector)
{
  if (fEMfield) delete fEMfield;
  // Set the value of the Global Field value to fieldVector

  // Find the Field Manager for the global field
  G4FieldManager* fieldMgr= GetGlobalFieldManager();

  if (fieldVector != G4ThreeVector(0.,0.,0.))
  {
    fEMfield = new G4UniformMagField(fieldVector);
  }
  else
  {
    // If the new field's value is Zero, then it is best to
    //  insure that it is not used for propagation.
    fEMfield = 0;
  }
  fieldMgr->SetDetectorField(fEMfield);
  fEquation->SetFieldObj(fEMfield);  // must now point to the new field
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4FieldManager*  QTMagneticFieldSetup::GetGlobalFieldManager()
{
//  Utility method

  return G4TransportationManager::GetTransportationManager()
           ->GetFieldManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
