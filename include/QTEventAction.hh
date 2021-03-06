#ifndef QTEventAction_h
#define QTEventAction_h 1

#include "QTGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>

/// Event action class
///

class QTEventAction : public G4UserEventAction
{
public:
  QTEventAction(G4int);
  virtual ~QTEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  std::vector<G4double>& GetTimeVec(G4int i)    { return tvec[i]; }
  std::vector<G4double>& GetVoltageVec(G4int i) { return vvec[i]; }

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;
  G4int                 nAntenna;

  std::array<std::vector<G4double>, nAntenna> tvec;
  std::array<std::vector<G4double>, nAntenna> vvec;
  
};

#endif
