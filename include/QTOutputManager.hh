// Outsource file output, follow Geant4 AnaEx01
#ifndef HistoManager_h
#define HistoManager_h 1

#include "G4AnalysisManager.hh"
#include "globals.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class QTOutputManager
{

public:
  QTOutputManager(G4String);
  ~QTOutputManager();
  
  void Book();
  void Save();
  
  void FillNtupleI(G4int which, G4int col, G4int val);
  void FillNtupleD(G4int which, G4int col, G4double val);
  inline void FillAntennaVec(G4int val)    { avec.push_back(val); }
  inline void FillTimeVec(G4double val)    { tvec.push_back(val); }
  inline void FillVoltageVec(G4double val) { vvec.push_back(val); }
  inline void FillOmVec(G4double val)      { ovec.push_back(val); }
  void AddNtupleRow(G4int which); // close an ntuple row

  G4String GetFileName() {return fout;}

private:
  // internal methods for booking
  std::vector<G4int>&    GetAntennaID()    { return avec; }
  std::vector<G4double>& GetTimeVec()      { return tvec; }
  std::vector<G4double>& GetVoltageVec()   { return vvec; }
  std::vector<G4double>& GetOmVec()        { return ovec; }

  G4bool    fFactoryOn = false;
  G4String  fout;
  std::vector<G4int>    avec;    // antenna vector
  std::vector<G4double> tvec;    // time vector
  std::vector<G4double> vvec;    // voltage vector
  std::vector<G4double> ovec;    // Omega vector

  G4AnalysisManager* analysisManager = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

