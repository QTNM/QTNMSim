// Outsource file output, follow Geant4 AnaEx01
#ifndef NAHistoManager_h
#define NAHistoManager_h 1

#include "G4AnalysisManager.hh"
#include "globals.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NAOutputManager
{

public:
  NAOutputManager(G4String);
  ~NAOutputManager();
  
  void Book();
  void Save();
  
  void FillNtupleI(G4int which, G4int col, G4int val);
  void FillNtupleD(G4int which, G4int col, G4double val);
  inline void FillOmVec(G4double val)      { ovec.push_back(val); }
  inline void FillKEVec(G4double val)      { kvec.push_back(val); }
  inline void FillTimeVec(G4double val)    { tvec.push_back(val); }
  inline void FillXVec(G4double val)       { xvec.push_back(val); }
  inline void FillYVec(G4double val)       { yvec.push_back(val); }
  inline void FillZVec(G4double val)       { zvec.push_back(val); }
  inline void FillBetaXVec(G4double val)   { bxvec.push_back(val); }
  inline void FillBetaYVec(G4double val)   { byvec.push_back(val); }
  inline void FillBetaZVec(G4double val)   { bzvec.push_back(val); }
  inline void FillAccXVec(G4double val)    { axvec.push_back(val); }
  inline void FillAccYVec(G4double val)    { ayvec.push_back(val); }
  inline void FillAccZVec(G4double val)    { azvec.push_back(val); }
  void AddNtupleRow(G4int which); // close an ntuple row

  G4String GetFileName() {return fout;}

private:
  // internal methods for booking
  std::vector<G4double>& GetOmVec()        { return ovec; }
  std::vector<G4double>& GetKEVec()        { return kvec; }
  std::vector<G4double>& GetTimeVec()      { return tvec; }
  std::vector<G4double>& GetXVec()         { return xvec; }
  std::vector<G4double>& GetYVec()         { return yvec; }
  std::vector<G4double>& GetZVec()         { return zvec; }
  std::vector<G4double>& GetBetaXVec()     { return bxvec; }
  std::vector<G4double>& GetBetaYVec()     { return byvec; }
  std::vector<G4double>& GetBetaZVec()     { return bzvec; }
  std::vector<G4double>& GetAccXVec()      { return axvec; }
  std::vector<G4double>& GetAccYVec()      { return ayvec; }
  std::vector<G4double>& GetAccZVec()      { return azvec; }

  G4bool    fFactoryOn = false;
  G4String  fout;
  std::vector<G4double> ovec;    // Omega vector
  std::vector<G4double> kvec;    // Kinteic energy vector
  std::vector<G4double> tvec;    // time vector
  std::vector<G4double> xvec;    // x vector
  std::vector<G4double> yvec;    // y vector
  std::vector<G4double> zvec;    // z vector
  std::vector<G4double> bxvec;   // beta x vector
  std::vector<G4double> byvec;   // beta y vector
  std::vector<G4double> bzvec;   // beta z vector
  std::vector<G4double> axvec;   // acc x vector
  std::vector<G4double> ayvec;   // acc y vector
  std::vector<G4double> azvec;   // acc z vector

  G4AnalysisManager* analysisManager = nullptr;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

