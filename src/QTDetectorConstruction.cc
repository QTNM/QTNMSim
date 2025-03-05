#include "QTDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"
#include "QTGasSD.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AutoDelete.hh"
#include "globals.hh"

#include "QTMagneticFieldSetup.hh"


QTDetectorConstruction::QTDetectorConstruction(const G4GDMLParser& p)
  : G4VUserDetectorConstruction(),
    fUserLimit(nullptr),
    fMessenger(nullptr),
    fMaxTime(100.0*CLHEP::ns),     // default max time 100 ns
    fMinEnergy(10.0*CLHEP::eV),
    fparser(p)
{
  DefineCommands();
}


QTDetectorConstruction::~QTDetectorConstruction()
{
  delete fMessenger;
  delete fUserLimit;
}


G4VPhysicalVolume* QTDetectorConstruction::Construct()
{
  fUserLimit = new G4UserLimits(DBL_MAX,DBL_MAX,fMaxTime,fMinEnergy,0.); // max time limit, min energy
  
  auto* worldLV = fparser.GetVolume("worldLV");
  auto* targetLV = fparser.GetVolume("Gas_log");
  worldLV->SetUserLimits(fUserLimit); // apply limit to world volume
  targetLV->SetUserLimits(fUserLimit); // apply limit to gas volume

  G4cout << "Gas LV number density [1/cm3]: " << targetLV->GetMaterial()->GetTotNbOfAtomsPerVolume()*cm3 << G4endl;
  return fparser.GetWorldVolume();
}


void QTDetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  
  // Only need to construct the (per-thread) SD once
  if(!fSD.Size()) {
    G4String collname;
    const G4GDMLAuxMapType* auxmap = fparser.GetAuxMap();
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
	iter!=auxmap->end(); iter++) 
      {
	G4LogicalVolume* lv = (*iter).first;
	// sensitive volume should be defined separately to
	// CAD input volume since for Gas Hits it is the atom cloud.
        // make two sensitive detectors, cloud and vac.
	if (lv->GetName() == "Gas_log")
          collname = "Gas";
	else if (lv->GetName() == "worldLV")
	  collname = "Vac";
	  // must be only one(!) auxiliary, otherwise need G4VectorCache
	auto entry = (*iter).second.front(); // G4GDMLAuxListType is std::vector
	if (entry.type=="SensDet") {
	  G4String SDname = entry.value; // sensdet by name
	  QTGasSD* aGasSD = new QTGasSD(SDname,
				  collname+"HitsCollection");
	  fSD.Push_back(aGasSD); // fSD is now a vector entry cache
	    
	  // Also only add it once to the SD manager!
	  G4SDManager::GetSDMpointer()->AddNewDetector(aGasSD);
	    
	  lv->SetSensitiveDetector(aGasSD);
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

  auto& energyCmd = fMessenger->DeclarePropertyWithUnit("minenergy", "eV", fMinEnergy,
					      "Set minimum electron transport energy in [eV].");
  energyCmd.SetParameterName("energy", true);
  energyCmd.SetDefaultValue("10 eV");

}
