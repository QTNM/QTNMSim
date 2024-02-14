#ifndef QTGasSD_h
#define QTGasSD_h 1

#include "G4VSensitiveDetector.hh"

#include "QTGasHit.hh"

class G4Step;
class G4HCofThisEvent;

/// QTGas sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class QTGasSD : public G4VSensitiveDetector
{
  public:
    QTGasSD(const G4String& name, 
            const G4String& hitsCollectionName);
    ~QTGasSD() override;
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

  private:
    QTGasHitsCollection* fHitsCollection;
};

#endif

