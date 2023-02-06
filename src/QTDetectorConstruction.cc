#include "QTDetectorConstruction.hh"

#include "G4SDManager.hh"
#include "QTGasSD.hh"
#include "G4VPhysicalVolume.hh"

#include "QTMagneticFieldSetup.hh"


QTDetectorConstruction::QTDetectorConstruction(const G4GDMLParser& p)
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
  if(!fSD.Get()) {
    const G4GDMLAuxMapType* auxmap = fparser.GetAuxMap();
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
	iter!=auxmap->end(); iter++) 
      {
	G4LogicalVolume* lv = (*iter).first;
	// sensitive volume should be defined separately to
	// CAD input volume since for Gas Hits it is the atom cloud.
	if (lv->GetName() == "Gas_log") {
	  // must be only one(!) auxiliary, otherwise need G4VectorCache
	  auto entry = (*iter).second.front(); // G4GDMLAuxStructType in std::vector
	  if (entry.type=="SensDet") {
	    G4String SDname = entry.value;;
	    QTGasSD* aGasSD = new QTGasSD(SDname,
					  "GasHitsCollection");
	    fSD.Put(aGasSD); // fSD is a single entry cache
	    
	    // Also only add it once to the SD manager!
	    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());
	    
	    lv->SetSensitiveDetector(fSD.Get());
	  }
	}
      }
  }

  // Construct the field creator - this will register the field it creates

  if (!fEmFieldSetup.Get()) {
    QTMagneticFieldSetup* fieldSetup = new QTMagneticFieldSetup();
    G4AutoDelete::Register(fieldSetup); //Kernel will delete the messenger
    fEmFieldSetup.Put(fieldSetup);
  }
  
}

