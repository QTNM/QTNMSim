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
  QTEventAction(G4int na);
  virtual ~QTEventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  std::vector<G4int>&    GetAntennaID()    { return avec; }
  std::vector<G4double>& GetTimeVec()      { return tvec; }
  std::vector<G4double>& GetVoltageVec()   { return vvec; }

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID     = -1;
  G4int                 nAntenna = -1;

  std::vector<G4int>    avec;    // vector container
  std::vector<G4double> tvec;    // vector container
  std::vector<G4double> vvec;    // vector container
  
};

#endif
