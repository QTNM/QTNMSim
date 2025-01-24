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
/// \file electromagnetic/TestEm0/TestEm0.cc
/// \brief Main program of the electromagnetic/TestEm0 example
//
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

//#include "G4UIExecutive.hh"

#include "CLI11.hpp"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv) {

  // command line interface
  CLI::App    app{ "QTNM simulation app" };
  int         nthreads = 4;
  int         seed     = 1234;
  std::string outputFileName("qtnm.root");
  std::string macroName;
  std::string gdmlFileName("test.gdml");
  std::string physListName("QTNMPhysicsList");

  app.add_option("-m,--macro", macroName, "<Geant4 macro filename> Default: None");
  app.add_option("-g,--gdml", gdmlFileName, "<Geant4 GDML filename> Default: test.gdml");
  app.add_option("-p,--physlist", physListName, "<Geant4 physics list macro> Default: QTNMPhysicsList");
  app.add_option("-s,--seed", seed, "<Geant4 random number seed + offset 1234> Default: 1234");
  app.add_option("-o,--outputFile", outputFileName,
                 "<FULL PATH ROOT FILENAME> Default: qtnm.root");
  app.add_option("-t, --nthreads", nthreads, "<number of threads to use> Default: 4");

  CLI11_PARSE(app, argc, argv);

  // Get the pointer to the User Interface manager
  //
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  // Don't accept interactive mode (no macroName).
  if(macroName.empty())
  {
    G4cout << "No interactive mode running of example: provide a macro!" << G4endl;
    return 1;
  }

  // set the random seed + offset 1234; avoiding zero seed -> runtime error
  CLHEP::HepRandom::setTheSeed(1234+seed);

  //Construct a serial run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);

  // set mandatory initialization classes
  DetectorConstruction* det;
  PrimaryGeneratorAction* prim;
  runManager->SetUserInitialization(det = new DetectorConstruction);
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserAction(prim = new PrimaryGeneratorAction(det));

  // set user action classes
  runManager->SetUserAction(new RunAction(det,prim));

    // Batch mode only - no visualisation
  G4String command = "/control/execute ";
  UImanager->ApplyCommand(command + macroName);

  //job termination
  //
  delete runManager;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
