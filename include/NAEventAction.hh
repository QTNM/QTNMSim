#ifndef NAEventAction_h
#define NAEventAction_h 1

#include "QTGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///
class NAOutputManager;


class NAEventAction : public G4UserEventAction
{
public:
  NAEventAction(NAOutputManager*);
  virtual ~NAEventAction() override;

  virtual void BeginOfEventAction(const G4Event* event) override;
  virtual void EndOfEventAction(const G4Event* event) override;

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  NAOutputManager*      fOutput  = nullptr;
  G4int                 fGID     = -1;
  G4int                 fVID     = -1;

};

#endif
