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
/// \file electromagnetic/TestEm0/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//

//
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  fBoxSize = 1*mm;
  DefineMaterials();
  SetMaterial("HydrogenGas");
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ delete fDetectorMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  //
  // define Materials
  //
  G4Element *H = new G4Element("Hydrogen", "H", 1., 1.008 * g / mole);
  G4Material *HMat = new G4Material("HydrogenGas", 1e-8 * g / cm3, 1);
  HMat->AddElement(H, 1);

  G4Element *D = new G4Element("Deuterium", "D", 1., 2.0141 * g / mole);
  G4Material *DMat = new G4Material("DeuteriumGas", 1e-8 * g / cm3, 1);
  DMat->AddElement(D, 1);

  G4Element *T = new G4Element("Tritium", "T", 1., 3.016 * g / mole);
  G4Material *TMat = new G4Material("TritiumGas", 1e-8 * g / cm3, 1);
  TMat->AddElement(T, 1);

  G4Element *He = new G4Element("Helium", "He", 2., 4.003 * g / mole);
  G4Material *HeMat = new G4Material("HeliumGas", 1e-8 * g / cm3, 1);
  HeMat->AddElement(He, 1);

  G4Element *Ar = new G4Element("Argon", "Ar", 18., 39.948 * g / mole);
  G4Material *ArMat = new G4Material("ArgonGas", 1e-8 * g / cm3, 1);
  ArMat->AddElement(Ar, 1);

  // Add others as appropriate
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // the geometry is created once
  if(fBox) { return fBox; }

  G4Box*
    sBox = new G4Box("Container",                        //its name
                     fBoxSize/2,fBoxSize/2,fBoxSize/2);  //its dimensions

  G4LogicalVolume*
    lBox = new G4LogicalVolume(sBox,                     //its shape
                               fMaterial,                //its material
                               fMaterial->GetName());    //its name

  fBox = new G4PVPlacement(0,                            //no rotation
                           G4ThreeVector(0.,0.,0.),      //at (0,0,0)
                           lBox,                         //its logical volume
                           fMaterial->GetName(),         //its name
                           0,                            //its mother  volume
                           false,                        //no boolean operation
                           0);                           //copy number

  //always return the root volume
  //
  return fBox;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintParameters() const
{
  G4cout << "\n The Box is " << G4BestUnit(fBoxSize,"Length")
         << " of " << fMaterial->GetName() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"

void DetectorConstruction::SetMaterial(const G4String& materialChoice)
{
  // search the material by its name, or build it from nist data base
  G4Material* pttoMaterial =
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fMaterial = pttoMaterial;
    if(fBox) { fBox->GetLogicalVolume()->SetMaterial(fMaterial); }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4cout << "\n " << fMaterial << G4endl;
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
           << materialChoice << " not found" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
