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
  QTEventAction(QTOutputManager*);
  virtual ~QTEventAction() override;

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  // methods
  QTGasHitsCollection*     GetGasHitsCollection(G4int hcID,
						const G4Event* event) const;

  // data members
  // hit data
  QTOutputManager*      fOutput  = nullptr;
  G4int                 fGID     = -1;

};

#endif
