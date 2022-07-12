#ifndef QTEventAction_h
#define QTEventAction_h 1

#include "QTGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

class QTEventAction : public G4UserEventAction
{
public:
  QTEventAction()          = default;
  virtual ~QTEventAction() = default;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  G4int                 fGID    = -1;
  
};

#endif
