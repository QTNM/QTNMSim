// ********************************************************************
// QTNM simulation app

// standard
#include <algorithm>
#include <string>
#include <vector>

// Geant4
#include "G4Types.hh"
#include "Randomize.hh"

#ifdef G4MULTITHREADED
#  include "G4MTRunManager.hh"
#else
#  include "G4RunManager.hh"
#endif

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4Threading.hh"
#include "G4GenericPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4GDMLParser.hh"
#include "G4LogicalVolume.hh"
#include "G4StepLimiterPhysics.hh"

// us
#include "CLI11.hpp"  // c++17 safe; https://github.com/CLIUtils/CLI11
#include "QTDetectorConstruction.hh"
#include "QTActionInitialization.hh"

int main(int argc, char** argv)
{
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

  // GEANT4 code
  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Don't accept interactive mode (no macroName).
  if(macroName.empty())
  {
    G4cout << "No interactive mode running of example: provide a macro!" << G4endl;
    return 1;
  }

  // set the random seed + offset 1234; avoiding zero seed -> runtime error
  CLHEP::HepRandom::setTheSeed(1234+seed);

  // -- Construct the run manager : MT or sequential one
  auto* runManager = G4RunManagerFactory::CreateRunManager();
#ifdef G4MULTITHREADED
  nthreads =
    std::min(nthreads, G4Threading::G4GetNumberOfCores());  // limit thread number to
                                                            // max on machine
  G4cout << "      ********* Run Manager constructed in MT mode: " << nthreads
         << " threads ***** " << G4endl;
  runManager->SetNumberOfThreads(nthreads);
#endif


  G4GDMLParser parser;
  parser.SetOverlapCheck(true);
  parser.Read(gdmlFileName); // const G4String&

  // IGNORE for noAntennaInfo branch
  //
  // retrieve antenna information
  // std::vector<double> angles;
  // const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
  // for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
  //     iter!=auxmap->end(); iter++)
  //   {
  //     G4LogicalVolume* lv = (*iter).first;
  //     G4String nam = lv->GetName();
  //     // in name of logical volume, even for CAD input, assume 'Antenna'
  //     // since that should receive a list of auxiliaries.
  //     if (G4StrUtil::contains(nam, "Antenna")) {
  // 	for (auto entry : (*iter).second) { // G4GDMLAuxStructType in std::vector
  // 	  if (entry.type=="angle") { // assume radians
  // 	    std::string theta = entry.value;
  // 	    angles.push_back(std::stod(theta)); // convert string to double
  // 	  }
  // 	}
  //     }
  //   }

  // -- Set mandatory initialization classes
  runManager->SetUserInitialization(new QTDetectorConstruction(parser));

  // -- set user physics list
  // Physics list factory
  G4VModularPhysicsList* physList = nullptr;

  // from vector of physics constructor names
  std::vector<G4String>* myConstructors = new std::vector<G4String>;
  myConstructors->push_back(physListName.data());
  physList = new G4GenericPhysicsList(myConstructors);

  // Register Step limiter
  physList->RegisterPhysics(new G4StepLimiterPhysics());

  // finish physics list
  runManager->SetUserInitialization(physList);


  // -- Set user action initialization class.
  //  auto* actions = new QTActionInitialization(outputFileName, angles);
  auto* actions = new QTActionInitialization(outputFileName);
  runManager->SetUserInitialization(actions);


  // Batch mode only - no visualisation
  G4String command = "/control/execute ";
  UImanager->ApplyCommand(command + macroName);

  delete runManager;
  return 0;
}
