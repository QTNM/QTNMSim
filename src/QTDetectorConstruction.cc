#include "QTDetectorConstruction.hh"

#include "G4SDManager.hh"
#include "QTGasSD.hh"
#include "G4VPhysicalVolume.hh"


QTDetectorConstruction::QTDetectorConstruction(const G4GMDLParser& p)
  : G4VUserDetectorConstruction(),
    fparser(p)
{
}


G4VPhysicalVolume* QTDetectorConstruction::Construct()
{
  return fparser.GetWorldVolume();
}


void QTDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Get())
  {
    G4String SD1name  = "GasSD";
    QTGasSD* aGasSD = new QTGasSD(SD1name,
                                  "GasHitsCollection");
    fSD.Put(aGasSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("Gas_log", fSD.Get());
  }

}

