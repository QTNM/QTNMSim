// Outsource file output, follow Geant4 AnaEx01
#ifndef HistoManager_h
#define HistoManager_h 1

#include "G4AnalysisManager.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class QTOutputManager
{

public:
  QTOutputManager(G4String);
  virtual ~QTOutputManager();
  
  void Book();
  void Save();
  
  void FillNtupleI(G4int which, G4int col, G4int val);
  void FillNtupleD(G4int which, G4int col, G4double val);
  void AddNtupleRow(G4int which); // close an ntuple row

  G4String GetFileName() {return fout;}
  void     SetIDVec(std::vector<G4int>& a)      {avec = a;}
  void     SetTimeVec(std::vector<G4double>& t)    {tvec = t;}
  void     SetVoltageVec(std::vector<G4double>& v) {vvec = v;}

private:
  // internal methods for booking
  std::vector<G4int>&    GetAntennaID()    { return avec; }
  std::vector<G4double>& GetTimeVec()      { return tvec; }
  std::vector<G4double>& GetVoltageVec()   { return vvec; }

  G4bool    fFactoryOn = false;
  G4String  fout;
  std::vector<G4int>    avec;    // vector container address
  std::vector<G4double> tvec;
  std::vector<G4double> vvec;

  G4AnalysisManager* analysisManager = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

