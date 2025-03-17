#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "globals.hh"

class DetectorConstruction;

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(G4String, DetectorConstruction *);
  ~ActionInitialization() override;

  virtual void BuildForMaster() const override;
  virtual void Build() const override;

private:
  DetectorConstruction *fDetector = nullptr;
  G4String foutname;
};

#endif
