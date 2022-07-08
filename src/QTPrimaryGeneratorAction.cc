// us
#include "QTPrimaryGeneratorAction.hh"

// geant
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RandomTools.hh"


QTPrimaryGeneratorAction::QTPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
, fMessenger(nullptr)
, fMean(18.575)
, fStdev(5.e-4)
, fSpot(0.5)
, fGunType(true)
{
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  auto particleTable = G4ParticleTable::GetParticleTable();

  // default electron particle kinematics
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("e-"));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.)); // z-axis

  DefineCommands();
}

QTPrimaryGeneratorAction::~QTPrimaryGeneratorAction()
{
  delete fMessenger;
  delete fParticleGun;
}

void QTPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore: assumes name is World_log!
  // Check: Name requirement for GDML file AND axis assumption!
  //
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World_log");

  if (fGunType) {
    G4Box* worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
    G4double worldZHalfLength = worldBox->GetZHalfLength();

    // random spot location [mm]
    G4TwoVector loc = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
    fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()*mm, loc.y()*mm, -worldZHalfLength + 1.*cm));
    
    // Gaussian random energy [keV]
    G4double en = G4RandGauss::shoot(fMean, fStdev);
    fParticleGun->SetParticleEnergy(en * keV);
  }
  // else implement tritium beta decay event generator

  fParticleGun->GeneratePrimaryVertex(event);
}


void QTPrimaryGeneratorAction::DefineCommands()
{
  // Define /QT/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/QT/generator/", "Primary generator control");

  // gun type command
  auto& typeCmd = fMessenger->DeclareProperty("energy", fGunType,
					      "Boolean gun type choice: true=electron gun; false=TBetaDecay.");
  typeCmd.SetParameterName("t", true);
  typeCmd.SetDefaultValue("true");

  // energy command
  auto& energyCmd = fMessenger->DeclareProperty("energy", fMean,
                                               "Mean Gun energy [keV].");
  energyCmd.SetParameterName("d", true);
  energyCmd.SetRange("d>=1.");
  energyCmd.SetDefaultValue("18.575");

  // width command
  auto& widthCmd = fMessenger->DeclareProperty("width", fStdev,
                                               "Gun energy standard deviation [keV].");
  widthCmd.SetParameterName("w", true);
  widthCmd.SetRange("w>=0.");
  widthCmd.SetDefaultValue("5.e-4");

  // spot size command
  auto& spotCmd = fMessenger->DeclareProperty("spot", fSpot,
                                               "Gun spot diameter [mm].");
  spotCmd.SetParameterName("s", true);
  spotCmd.SetRange("s>=0.");
  spotCmd.SetDefaultValue("0.5");
}
