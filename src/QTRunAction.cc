#include "EGRunAction.hh"
#include "g4root.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

EGRunAction::EGRunAction(QTEventAction* eventAction, G4String name)
: G4UserRunAction()
, fEventAction(eventAction)
, fout(std::move(name))
{
  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetNtupleDirectoryName("ntuple");

  // Creating ntuple 0 with number entries
  //
  analysisManager->CreateNtuple("Score", "Hits");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("TrackID");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("TimeStamp");
  analysisManager->CreateNtupleDColumn("Kine");
  analysisManager->CreateNtupleDColumn("Px");
  analysisManager->CreateNtupleDColumn("Py");
  analysisManager->CreateNtupleDColumn("Pz");
  analysisManager->CreateNtupleDColumn("Posx");
  analysisManager->CreateNtupleDColumn("Posy");
  analysisManager->CreateNtupleDColumn("Posz");
  analysisManager->FinishNtuple();

  // Creating ntuple 1 with vector entries
  //
  analysisManager->CreateNtuple("Signal", "Time-series");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleTColumn("TimeVec", fEventAction->GetTimeVec());
  analysisManager->CreateNtupleTColumn("VoltageVec", fEventAction->GetVoltageVec());
  analysisManager->FinishNtuple();
}

EGRunAction::~EGRunAction() { delete G4AnalysisManager::Instance(); }

void EGRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  analysisManager->OpenFile(fout);
}

void EGRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}
