#include "SCDetectorConstruction.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"

#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "G4UniformMagField.hh"

#include "globals.hh"

#include "QTGasSD.hh"
#include "QTMagneticFieldSetup.hh"

SCDetectorConstruction::SCDetectorConstruction()
    : G4VUserDetectorConstruction(), fUserLimit(nullptr), fMessenger(nullptr),
      fMaxTime(DBL_MAX), fBoxSize(1.0 * CLHEP::m) {
  DefineMaterials();
  SetMaterial("HydrogenGas");
  DefineCommands();
}

SCDetectorConstruction::~SCDetectorConstruction() {
  delete fMessenger;
  delete fUserLimit;
}

G4VPhysicalVolume *SCDetectorConstruction::Construct() {
  return ConstructVolumes();
}

G4VPhysicalVolume *SCDetectorConstruction::ConstructVolumes() {
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  fUserLimit = new G4UserLimits(DBL_MAX, DBL_MAX, fMaxTime, 0.,
                                0.); // max time limit only

  G4Box *sBox =
      new G4Box("Container",                               // its name
                fBoxSize / 2, fBoxSize / 2, fBoxSize / 2); // its dimensions

  fLBox = new G4LogicalVolume(sBox,                  // its shape
                              fMaterial,             // its material
                              fMaterial->GetName()); // its name

  fPBox = new G4PVPlacement(0,                    // no rotation
                            G4ThreeVector(),      // at (0,0,0)
                            fLBox,                // its logical volume
                            fMaterial->GetName(), // its name
                            0,                    // its mother  volume
                            false,                // no boolean operation
                            0);                   // copy number

  // G4cout << "Gas LV number density [1/cm3]: " <<
  // targetLV->GetMaterial()->GetTotNbOfAtomsPerVolume()*cm3 << G4endl;

  // worldLV->SetUserLimits(fUserLimit); // apply limit to world volume
  // targetLV->SetUserLimits(fUserLimit); // apply limit to gas volume

  // always return the root volume
  //
  return fPBox;
}

void SCDetectorConstruction::ConstructSDandField() {
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // // Only need to construct the (per-thread) SD once
  // if(!fSD.Size()) {
  //   G4String collname;
  //   const G4GDMLAuxMapType* auxmap = fparser.GetAuxMap();
  //   for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
  // 	iter!=auxmap->end(); iter++)
  //     {
  // 	G4LogicalVolume* lv = (*iter).first;
  // 	// sensitive volume should be defined separately to
  // 	// CAD input volume since for Gas Hits it is the atom cloud.
  //       // make two sensitive detectors, cloud and vac.
  // 	if (lv->GetName() == "Gas_log")
  //         collname = "Gas";
  // 	else if (lv->GetName() == "worldLV")
  // 	  collname = "Vac";
  // 	  // must be only one(!) auxiliary, otherwise need G4VectorCache
  // 	auto entry = (*iter).second.front(); // G4GDMLAuxListType is std::vector
  // 	if (entry.type=="SensDet") {
  // 	  G4String SDname = entry.value; // sensdet by name
  // 	  QTGasSD* aGasSD = new QTGasSD(SDname,
  // 				  collname+"HitsCollection");
  // 	  fSD.Push_back(aGasSD); // fSD is now a vector entry cache

  // 	  // Also only add it once to the SD manager!
  // 	  G4SDManager::GetSDMpointer()->AddNewDetector(aGasSD);

  // 	  lv->SetSensitiveDetector(aGasSD);
  // 	}
  //     }
  // }

  // Field setup
  if (!fFieldMessenger.Get()) {
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue = G4ThreeVector();
    G4GlobalMagFieldMessenger *msg = new G4GlobalMagFieldMessenger(fieldValue);
    msg->SetVerboseLevel(1);
    G4AutoDelete::Register(msg);
    fFieldMessenger.Put(msg);
  }
}

void SCDetectorConstruction::DefineMaterials() {
  //
  // define Materials
  //
  G4Element *H = new G4Element("Hydrogen", "H", 1., 1.008 * g / mole);
  G4Material *HMat = new G4Material("HydrogenGas", 1e-8 * g / cm3, 1);
  HMat->AddElement(H, 1);

  G4Element *D = new G4Element("Deuterium", "D", 1., 2.0141 * g / mole);
  G4Material *DMat = new G4Material("DeuteriumGas", 1e-8 * g / cm3, 1);
  DMat->AddElement(D, 1);

  G4Element *T = new G4Element("Tritium", "T", 1., 3.016 * g / mole);
  G4Material *TMat = new G4Material("TritiumGas", 1e-8 * g / cm3, 1);
  TMat->AddElement(T, 1);

  // Add others as appropriate
}

void SCDetectorConstruction::SetMaterial(G4String materialChoice) {
  // search the material by its name, or build it from nist data base
  G4Material *pttoMaterial =
      G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fMaterial = pttoMaterial;
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
           << materialChoice << " not found" << G4endl;
  }
}

void SCDetectorConstruction::DefineCommands() {
  // Define /QT/transport command directory using generic messenger class
  fMessenger =
      new G4GenericMessenger(this, "/QT/transport/", "transport control");

  // gun type command
  auto &timeCmd = fMessenger->DeclarePropertyWithUnit(
      "maxtime", "ns", fMaxTime,
      "Set maximum electron transport time in [ns].");
  timeCmd.SetParameterName("time", true);
  timeCmd.SetDefaultValue("100 ns");
}
