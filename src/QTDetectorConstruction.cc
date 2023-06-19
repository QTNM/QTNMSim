#include "QTDetectorConstruction.hh"

#include "G4UserLimits.hh"
#include "G4SDManager.hh"
#include "QTGasSD.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AutoDelete.hh"
#include "globals.hh"

#include "QTMagneticFieldSetup.hh"


QTDetectorConstruction::QTDetectorConstruction(const G4GDMLParser& p)
  : G4VUserDetectorConstruction(),
    fMaxTimeLimit(nullptr),
    fMessenger(nullptr),
    fMaxTime(100.0*CLHEP::ns),     // default max time 100 ns
    fparser(p)
{
  DefineCommands();
}


QTDetectorConstruction::~QTDetectorConstruction()
{
  delete fMessenger;
  delete fMaxTimeLimit;
}


G4VPhysicalVolume* QTDetectorConstruction::Construct()
{
  fMaxTimeLimit = new G4UserLimits(DBL_MAX,DBL_MAX,fMaxTime,0.,0.); // max time limit
  
  auto* worldLV = fparser.GetVolume("worldLV");
  worldLV->SetUserLimits(fMaxTimeLimit); // apply limit to world volume

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

void QTDetectorConstruction::DefineCommands()
{
  // Define /QT/transport command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/QT/transport/", "transport control");

  // gun type command
  auto& timeCmd = fMessenger->DeclarePropertyWithUnit("maxtime", "ns", fMaxTime,
					      "Set maximum electron transport time in [ns].");
  timeCmd.SetParameterName("time", true);
  timeCmd.SetDefaultValue("100 ns");

}
