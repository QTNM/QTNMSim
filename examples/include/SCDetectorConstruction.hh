/// Definition of the SCDetectorConstruction class
//
//
//
//

#ifndef _QTDETECTORCONSTRUCTION_H_
#define _QTDETECTORCONSTRUCTION_H_

#include "G4Cache.hh"
#include "G4GenericMessenger.hh"
#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;
class G4Material;
class G4GlobalMagFieldMessenger;
class QTGasSD;
// class QTMagneticFieldSetup;
//  add a user limit to world = maximum time limit for transport
class G4UserLimits;

class SCDetectorConstruction : public G4VUserDetectorConstruction {
public:
  SCDetectorConstruction();
  ~SCDetectorConstruction() override;

  virtual G4VPhysicalVolume *Construct() override;
  virtual void ConstructSDandField() override;

  void SetMaterial(G4String);
  G4double GetSize() { return fBoxSize; };
  G4Material *GetMaterial() { return fMaterial; };

private:
  void DefineCommands();
  void DefineMaterials();
  G4VPhysicalVolume *ConstructVolumes();

  G4UserLimits *fUserLimit = nullptr; // max time and max step limit
  G4GenericMessenger *fMessenger = nullptr;
  G4double fMaxTime;
  G4double fBoxSize;
  G4VPhysicalVolume *fPBox = nullptr;
  G4LogicalVolume *fLBox = nullptr;
  G4Material *fMaterial = nullptr;

  G4VectorCache<QTGasSD *> fSD;
  // G4Cache<QTMagneticFieldSetup*>  fEmFieldSetup = nullptr;
  G4Cache<G4GlobalMagFieldMessenger *> fFieldMessenger = nullptr;
};
#endif
