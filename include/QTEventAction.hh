#ifndef QTEventAction_h
#define QTEventAction_h 1

#include "QTGasHit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///
class QTOutputManager;


class QTEventAction : public G4UserEventAction
{
public:
  QTEventAction(G4int, QTOutputManager*);
  virtual ~QTEventAction() override;

  virtual void BeginOfEventAction(const G4Event* event) override;
  virtual void EndOfEventAction(const G4Event* event) override;

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  QTOutputManager*      fOutput  = nullptr;
  G4int                 fGID     = -1;
  G4int                 nAntenna = -1;

};

#endif
