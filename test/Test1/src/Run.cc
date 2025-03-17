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
/// \file electromagnetic/TestEm13/src/Run.cc
/// \brief Implementation of the Run class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Run.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run(DetectorConstruction *det, G4String name)
  : fDetector(det), fout(std::move(name)) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::SetPrimary(G4ParticleDefinition *particle, G4double energy) {
  fParticle = particle;
  fEkin = energy;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::RecordSecondaryData(const G4Step* aStep) {
  G4StepPoint *endPoint = aStep->GetPostStepPoint();
  G4String procName = endPoint->GetProcessDefinedStep()->GetProcessName();

  G4cout << "Recording secondary information for: " << procName << G4endl;

  // Secondary energy - total loss = secondary energy + ionisation
  G4double esec = (-aStep->GetTotalEnergyDeposit() - aStep->GetDeltaEnergy()) / CLHEP::eV;

  if (esec > 0.0) {
    secondaryEnergy.push_back(esec);
    G4cout << "Secondary energy = " << esec << G4endl;
  } else {
    G4double ke = endPoint->GetKineticEnergy() / CLHEP::eV;
    G4cout << "Elastic collision detected? Final kinetic energy = " << ke << G4endl;
  }
  G4cout << "=======================================" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun() {
  // Store energy values in file.
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");

  // Create directories
  analysisManager->SetVerboseLevel(1);

  // Creating ntuple with vector entries
  //
  analysisManager->CreateNtuple("SecondarySamples", "SecondarySamples");
  analysisManager->CreateNtupleDColumn("Energy");
  analysisManager->FinishNtuple();

  analysisManager->OpenFile(fout);

  for (size_t j=0; j<secondaryEnergy.size(); ++j) {
    analysisManager->FillNtupleDColumn(0, secondaryEnergy[j] / CLHEP::eV);
    analysisManager->AddNtupleRow();
  }

  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
