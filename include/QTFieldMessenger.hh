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
/// \file field/field02/include/F02FieldMessenger.hh
/// \brief Definition of the F02FieldMessenger class
//
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef QTFieldMessenger_h
#define QTFieldMessenger_h 1

#include "G4UImessenger.hh"

class QTMagneticFieldSetup;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class QTFieldMessenger: public G4UImessenger
{
  public:
    QTFieldMessenger(QTMagneticFieldSetup* );
    ~QTFieldMessenger() override;

    virtual void SetNewValue(G4UIcommand*, G4String) override;
 
  private:

    QTMagneticFieldSetup*      fEMFieldSetup;

    G4UIdirectory*             fFieldDir;
    G4UIcmdWithAString*        fFileNameCmd;
    G4UIcmdWithADoubleAndUnit* fBFieldZCmd;
    G4UIcmdWithADoubleAndUnit* fTrapZCmd;
    G4UIcmdWithADoubleAndUnit* fTrapRadiusCmd;
    G4UIcmdWithADoubleAndUnit* fTrapCurrentCmd;
    G4UIcmdWithADoubleAndUnit* fMinStepCmd;
    G4UIcmdWith3VectorAndUnit* fBFieldCmd;
    G4UIcmdWithoutParameter*   fUpdateCmd;
    G4UIcmdWithoutParameter*   fComsolBCmd;
    G4UIcmdWithoutParameter*   fBathtubBCmd;
    G4UIcmdWithoutParameter*   fTestBCmd;
};

#endif
