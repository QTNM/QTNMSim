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
/// \file field/field02/src/F02FieldMessenger.cc
/// \brief Implementation of the F02FieldMessenger class
//
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "QTFieldMessenger.hh"

#include "QTMagneticFieldSetup.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTFieldMessenger::QTFieldMessenger(QTMagneticFieldSetup* fieldSetup)
 : G4UImessenger(),
   fEMFieldSetup(fieldSetup),
   fFieldDir(0),
   fFileNameCmd(0),
   fBFieldZCmd(0),
   fBFieldCmd(0),
   fTrapZCmd(0),
   fTrapRadiusCmd(0),
   fTrapCurrentCmd(0),
   fMinStepCmd(0),
   fTestBCmd(0),
   fBathtubBCmd(0),
   fComsolBCmd(0),
   fUpdateCmd(0)
{
  fFieldDir = new G4UIdirectory("/field/");
  fFieldDir->SetGuidance("QT field tracking control.");

  fUpdateCmd = new G4UIcmdWithoutParameter("/field/update",this);
  fUpdateCmd->SetGuidance("Update calorimeter geometry.");
  fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
  fUpdateCmd->AvailableForStates(G4State_Idle);

  fTestBCmd = new G4UIcmdWithoutParameter("/field/uniformB",this);
  fTestBCmd->SetGuidance("Switch to test case Uniform B-field.");
  fTestBCmd->AvailableForStates(G4State_Idle);

  fBathtubBCmd = new G4UIcmdWithoutParameter("/field/bathTubB",this);
  fBathtubBCmd->SetGuidance("Switch to Bathtub  B-field.");
  fBathtubBCmd->AvailableForStates(G4State_Idle);

  fComsolBCmd = new G4UIcmdWithoutParameter("/field/comsolB",this);
  fComsolBCmd->SetGuidance("Switch to Comsol field map B-field.");
  fComsolBCmd->AvailableForStates(G4State_Idle);

  fBFieldZCmd = new G4UIcmdWithADoubleAndUnit("/field/setFieldZ",this);
  fBFieldZCmd->SetGuidance("Define uniform magnetic field.");
  fBFieldZCmd->SetGuidance("Magnetic field will be in Z direction.");
  fBFieldZCmd->SetGuidance("Value of magnetic field has to be given in Tesla");
  fBFieldZCmd->SetParameterName("Bz",false,false);
  fBFieldZCmd->SetDefaultUnit("tesla");
  fBFieldZCmd->SetDefaultValue(1.0);
  fBFieldZCmd->AvailableForStates(G4State_Idle);
 
  fBFieldCmd = new G4UIcmdWith3VectorAndUnit("/field/setField",this);
  fBFieldCmd->SetGuidance("Define uniform magnetic field.");
  fBFieldCmd->SetGuidance("Value of magnetic field has to be given in Tesla");
  fBFieldCmd->SetParameterName("Bx","By","Bz",false,false);
  fBFieldCmd->SetDefaultUnit("tesla");
  fBFieldCmd->AvailableForStates(G4State_Idle);
 
  fMinStepCmd = new G4UIcmdWithADoubleAndUnit("/field/setMinStep",this);
  fMinStepCmd->SetGuidance("Define minimal step");
  fMinStepCmd->SetParameterName("min step",false,false);
  fMinStepCmd->SetDefaultUnit("mm");
  fMinStepCmd->AvailableForStates(G4State_Idle);

  fTrapCurrentCmd = new G4UIcmdWithADoubleAndUnit("/field/setCurrent",this);
  fTrapCurrentCmd->SetGuidance("Define trapping current");
  fTrapCurrentCmd->SetParameterName("Trap Current",false,false);
  fTrapCurrentCmd->SetDefaultUnit("ampere");
  fTrapCurrentCmd->SetDefaultValue(1.0);
  fTrapCurrentCmd->AvailableForStates(G4State_Idle);

  fTrapRadiusCmd = new G4UIcmdWithADoubleAndUnit("/field/setRadius",this);
  fTrapRadiusCmd->SetGuidance("Define trapping radius");
  fTrapRadiusCmd->SetParameterName("Trap Radius",false,false);
  fTrapRadiusCmd->SetDefaultUnit("mm");
  fTrapRadiusCmd->SetDefaultValue(20.0);
  fTrapRadiusCmd->AvailableForStates(G4State_Idle);

  fTrapZCmd = new G4UIcmdWithADoubleAndUnit("/field/setZPos",this);
  fTrapZCmd->SetGuidance("Define coil +- z-position");
  fTrapZCmd->SetParameterName("Trap Z position",false,false);
  fTrapZCmd->SetDefaultUnit("mm");
  fTrapZCmd->SetDefaultValue(20.0);
  fTrapZCmd->AvailableForStates(G4State_Idle);

  fFileNameCmd = new G4UIcmdWithAString("/field/comsolFileName",this);
  fFileNameCmd->SetGuidance("Set bespoke COMSOL file name for reading (ending .csv.gz)");
  fFileNameCmd->SetParameterName("File Name",false,false);
  fFileNameCmd->SetDefaultValue(""); // empty default
  fFileNameCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTFieldMessenger::~QTFieldMessenger()
{
  delete fComsolBCmd;
  delete fBathtubBCmd;
  delete fTestBCmd;
  delete fFileNameCmd;
  delete fBFieldZCmd;
  delete fBFieldCmd;
  delete fMinStepCmd;
  delete fTrapCurrentCmd;
  delete fTrapRadiusCmd;
  delete fTrapZCmd;
  delete fFieldDir;
  delete fUpdateCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void QTFieldMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if( command == fTestBCmd )
    fEMFieldSetup->SetUniformB();
  if( command == fBathtubBCmd )
    fEMFieldSetup->SetBathTubB();
  if( command == fComsolBCmd )
    fEMFieldSetup->SetComsolB();
  if( command == fUpdateCmd )
    fEMFieldSetup->UpdateAll();
  if( command == fFileNameCmd )
    fEMFieldSetup->SetComsolFileName(fFileNameCmd->GetCurrentValue(newValue));
  if( command == fBFieldZCmd )
    fEMFieldSetup->SetFieldZValue(fBFieldZCmd->GetNewDoubleValue(newValue));
  if( command == fBFieldCmd )
    fEMFieldSetup->SetFieldValue(fBFieldCmd->GetNew3VectorValue(newValue));
  if( command == fMinStepCmd )
    fEMFieldSetup->SetMinStep(fMinStepCmd->GetNewDoubleValue(newValue));
  if( command == fTrapCurrentCmd )
    fEMFieldSetup->SetTrapCurrent(fTrapCurrentCmd->GetNewDoubleValue(newValue));
  if( command == fTrapRadiusCmd )
    fEMFieldSetup->SetTrapRadius(fTrapRadiusCmd->GetNewDoubleValue(newValue));
  if( command == fTrapZCmd )
    fEMFieldSetup->SetCoilsAtZ(fTrapZCmd->GetNewDoubleValue(newValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
