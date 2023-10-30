//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "QTOutputManager.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

QTOutputManager::QTOutputManager(G4String fname)
{
  fout = fname;

  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
}

// leave deleting to run manager, follows AnaEx01

QTOutputManager::~QTOutputManager() = default;


void QTOutputManager::Book()
{
  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();

  if ( ! fFactoryOn ) {
    analysisManager->SetVerboseLevel(0);
    analysisManager->SetNtupleMerging(true);

    // Create directory
    analysisManager->SetNtupleDirectoryName("ntuple");
  }

  // Open an output file
  //
  G4bool fileOpen = analysisManager->OpenFile(fout);
  if (! fileOpen) {
    G4cerr << "\n---> OutputManager::Book(): cannot open "
           << GetFileName() << G4endl;
    return;
  }

  if ( ! fFactoryOn ) {
    // Create ntuples, after file is open.
    // Ntuples ids are generated automatically starting from 0.
    
    // Creating ntuple 0 with number entries
    //
    analysisManager->CreateNtuple("Score", "Hits");
    analysisManager->CreateNtupleIColumn("EventID");
    analysisManager->CreateNtupleIColumn("TrackID");
    analysisManager->CreateNtupleDColumn("Edep");
    analysisManager->CreateNtupleDColumn("TimeStamp");
    analysisManager->CreateNtupleDColumn("PreKine");
    analysisManager->CreateNtupleDColumn("PostKine");
    analysisManager->CreateNtupleDColumn("PreTheta");
    analysisManager->CreateNtupleDColumn("PostTheta");
    analysisManager->CreateNtupleDColumn("Posx");
    analysisManager->CreateNtupleDColumn("Posy");
    analysisManager->CreateNtupleDColumn("Posz");
    analysisManager->FinishNtuple();
    
    // Creating ntuple 1 with vector entries
    //
    G4String kvecname = "KEVec";
    G4String tvecname = "TimeVec";
    G4String xpname = "PosxVec";
    G4String ypname = "PosyVec";
    G4String zpname = "PoszVec";
    G4String bxname = "BetaxVec";
    G4String byname = "BetayVec";
    G4String bzname = "BetazVec";
    G4String axname = "AccxVec";
    G4String ayname = "AccyVec";
    G4String azname = "AcczVec";
    analysisManager->CreateNtuple("Signal", "Time-series");
    analysisManager->CreateNtupleIColumn("EventID");
    analysisManager->CreateNtupleIColumn("TrackID");
    analysisManager->CreateNtupleDColumn("Posx"); // single starter values
    analysisManager->CreateNtupleDColumn("Posy"); // for each trajectory
    analysisManager->CreateNtupleDColumn("Posz"); // vertex position and
    analysisManager->CreateNtupleDColumn("PitchAngle"); // pitch angle wrt z-axis
    analysisManager->CreateNtupleDColumn("KinEnergy"); // kinetic energy
    // These need passing a reference to the vector
    // filled by AddNtupleRow() assumed
    analysisManager->CreateNtupleDColumn(kvecname, GetKEVec());
    analysisManager->CreateNtupleDColumn(tvecname, GetTimeVec());
    analysisManager->CreateNtupleDColumn(xpname, GetXVec());
    analysisManager->CreateNtupleDColumn(ypname, GetYVec());
    analysisManager->CreateNtupleDColumn(zpname, GetZVec());
    analysisManager->CreateNtupleDColumn(bxname, GetBetaXVec());
    analysisManager->CreateNtupleDColumn(byname, GetBetaYVec());
    analysisManager->CreateNtupleDColumn(bzname, GetBetaZVec());
    analysisManager->CreateNtupleDColumn(axname, GetAccXVec());
    analysisManager->CreateNtupleDColumn(ayname, GetAccYVec());
    analysisManager->CreateNtupleDColumn(azname, GetAccZVec());
    analysisManager->FinishNtuple();

    fFactoryOn = true;
  }

}

void QTOutputManager::Save()
{
  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();

  if (! fFactoryOn) { return; }
  
  analysisManager->Write();
  analysisManager->CloseFile();
  analysisManager->Clear();
  
}

void QTOutputManager::FillNtupleI(G4int which, G4int col, G4int val)
{
  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillNtupleIColumn(which, col, val);
}


void QTOutputManager::FillNtupleD(G4int which, G4int col, G4double val)
{
  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillNtupleDColumn(which, col, val);
}


void QTOutputManager::AddNtupleRow(G4int which)
{
  // Create or get analysis manager
  analysisManager = G4AnalysisManager::Instance();

  analysisManager->AddNtupleRow(which);

  // clear internal vector storage after writing to disk with this method.
  tvec.clear();
  kvec.clear();
  xvec.clear();
  yvec.clear();
  zvec.clear();
  bxvec.clear();
  byvec.clear();
  bzvec.clear();
  axvec.clear();
  ayvec.clear();
  azvec.clear();
}
