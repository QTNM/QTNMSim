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
#include "G4ChordFinder.hh"

#include "QTEquationOfMotion.hh"
#include "QTBorisScheme.hh"
#include "QTBorisDriver.hh"
#include "QTMagneticTrap.hh"
#include "QTComsolField.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//  Constructors:

QTMagneticFieldSetup::QTMagneticFieldSetup()
 : fMinStep(0.010*mm),  // minimal step of 10 microns
   fTest(true),         // default case
   fBathTub(false),
   fComsol(false),
   fFieldManager(0),
   fChordFinder(0),
   fEquation(0),
   fEMfield(0),
   fTrapfield(0),
   fCMfield(0),
   fBStepper(0),
   fBDriver(0),
   fFieldMessenger(nullptr)   
{
  fTrapCurrent = 1.0*ampere; // defaults
  fTrapRadius  = 20.0*mm;
  fTrapZPos    = 20.0*mm; // +- 2cm
  fFileName    = "";
  G4ThreeVector fieldVector( 0.0, 0.0, 1.0 * CLHEP::tesla);
  fFieldVector = fieldVector; // initialize

  fEMfield = new G4UniformMagField(fieldVector); // set default
  fEquation = new QTEquationOfMotion(fEMfield);

  fFieldManager = GetGlobalFieldManager();

  SetUpBorisDriver();
  fFieldMessenger = new QTFieldMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTMagneticFieldSetup::QTMagneticFieldSetup(G4ThreeVector fieldVector)
  : fMinStep(0.010*mm),  // minimal step of 10 microns
    fTest(true),         // default case
    fBathTub(false),
    fComsol(false),
    fFieldManager(0),
    fChordFinder(0),
    fEquation(0),
    fEMfield(0),
    fTrapfield(0),
    fCMfield(0),
    fBStepper(0),
    fBDriver(0),
    fFieldMessenger(nullptr)
{
  fTrapCurrent = 1.0*ampere; // defaults
  fTrapRadius  = 20.0*mm;
  fTrapZPos    = 20.0*mm; // +- 2cm
  fFileName    = "";
  fFieldVector = fieldVector;
  fEMfield = new G4UniformMagField(fieldVector); // default

  fEquation = new QTEquationOfMotion(fEMfield);

  fFieldManager = GetGlobalFieldManager();

  SetUpBorisDriver();

  fFieldMessenger = new QTFieldMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTMagneticFieldSetup::~QTMagneticFieldSetup()
{

  delete fFieldMessenger; fFieldMessenger= nullptr;
   // Delete the messenger first, to avoid messages to deleted classes!
  
  delete fChordFinder;  fChordFinder= nullptr;
  delete fBStepper;     fBStepper = nullptr;
  delete fEquation;     fEquation = nullptr;
  if (fEMfield) {  
    delete fEMfield;
    fEMfield = nullptr;
  }
  if (fTrapfield) {
    delete fTrapfield;
    fTrapfield = nullptr;
  }
  if (fCMfield) {
    delete fCMfield;
    fCMfield = nullptr;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void QTMagneticFieldSetup::SetUpBorisDriver()
{
  // set up Boris driver from Geant4.11, follows example field01
  //  G4cout << " QTFieldSetup::CreateAndSetupBorisDriver() called. " << G4endl;   
  //  G4cout << "   1. Creating Scheme (Stepper)."  << G4endl;
  fBStepper = new QTBorisScheme(fEquation);
  //  G4cout << "   2. Creating Driver."  << G4endl;
  fBDriver  = new QTBorisDriver(fMinStep, fBStepper);

  //  G4cout  << "  3. Creating ChordFinder."  << G4endl;
  fChordFinder = new G4ChordFinder( fBDriver );

  //  G4cout  << "  4. Updating Field Manager (with ChordFinder, field)."  << G4endl;
  fFieldManager->SetChordFinder( fChordFinder );
  // field switches are unique
  if (fTest)
    fFieldManager->SetDetectorField(fEMfield );
  else if (fBathTub)
    fFieldManager->SetDetectorField(fTrapfield );
  else
    fFieldManager->SetDetectorField(fCMfield );

}


void QTMagneticFieldSetup::UpdateAll()
{
  // It must be possible to call 'again' after an alternative stepper
  //   has been chosen, or other changes have been made
  // Focus here are G4 Boris objects only.
  assert(fEquation!=nullptr);

  G4cout<< " QTMagneticFieldSetup/UpdateAll: The minimal step is equal to "
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


void QTMagneticFieldSetup::SetUniformB()
{
  // switch on uniform B-field.
  G4cout << "set uniform field" << G4endl;
  fTest = true;
  fBathTub = false; // allow only one option
  fComsol  = false;
  UpdateBField();
}

void QTMagneticFieldSetup::SetBathTubB()
{
  // switch on Bathtub B-field.
  G4cout << "set bathtub field" << G4endl;
  fTest    = false;
  fBathTub = true; // allow only one option
  fComsol  = false;
  UpdateBField();
}

void QTMagneticFieldSetup::SetComsolB()
{
  // switch on Comsol B-field map.
  G4cout << "set comsol field" << G4endl;
  fTest    = false;
  fBathTub = false; // allow only one option
  fComsol  = true;
  
  UpdateBField();
}


void QTMagneticFieldSetup::UpdateBField()
{
  // any change to parameter or types needs this update.

  if (fEMfield) {
    delete fEMfield;
    fEMfield = nullptr;
  }
  if (fTrapfield) {
    delete fTrapfield;
    fTrapfield = nullptr;
  }
  if (fCMfield) {
    delete fCMfield;
    fCMfield = nullptr;
  }
  // Set the value of the Global Field value to fieldVector

  // Find the Field Manager for the global field
  G4FieldManager* fieldMgr= GetGlobalFieldManager();

  if (fTest) {
    if (fFieldVector != G4ThreeVector(0.,0.,0.)) // set zero on purpose
      {
	fEMfield = new G4UniformMagField(fFieldVector);
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
  else if (fBathTub) {
    fTrapfield = new QTMagneticTrap(fFieldVector);
    fTrapfield->SetCurrent(fTrapCurrent);
    fTrapfield->SetRadius(fTrapRadius);
    fTrapfield->SetCoilZ(fTrapZPos);

    fieldMgr->SetDetectorField(fTrapfield);
    fEquation->SetFieldObj(fTrapfield);  // must now point to the new field
  }
  else {
    if (!fFileName.empty())
      fCMfield = new QTComsolField(fFileName);
    else {
      G4ExceptionDescription ed;
      ed << "File name not set, empty! " << std::endl;
      G4Exception("QTMagneticFieldSetup::UpdateBField",
		  "qtnmsim001",FatalException,ed);
    }
    fieldMgr->SetDetectorField(fCMfield);
    fEquation->SetFieldObj(fCMfield);  // must now point to the new field
  }
}


void QTMagneticFieldSetup::SetFieldZValue(G4double fieldValue)
{
  // Set the value of the Global Field to fieldValue along Z
  G4ThreeVector fieldVector( 0.0, 0.0, fieldValue );

  SetFieldValue( fieldVector );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4FieldManager*  QTMagneticFieldSetup::GetGlobalFieldManager()
{
//  Utility method

  return G4TransportationManager::GetTransportationManager()
           ->GetFieldManager();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
