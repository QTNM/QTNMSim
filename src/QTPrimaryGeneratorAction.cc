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
#include "G4Tubs.hh"
#include "G4RandomTools.hh"


QTPrimaryGeneratorAction::QTPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
, fMessenger(nullptr)
, fMean(18.575)
, fStdev(5.e-4)
, fSpot(0.5)
, fGunType(true) // calibration: true=electron gun
, fOrder(true)   // neutrino hierarchie: true=normal
, fNumass(1.0e-4)
, fSterilemass(0.0)
, fSterilemixing(0.0)
{
  static long seed = CLHEP::HepRandom::getTheSeed();
  generator.seed(seed); // using std random

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
  // from G4LogicalVolumeStore: assumes source name is Source_log!
  // Check: Name requirement for GDML file AND axis assumption!
  // Check: G4Tubs assumption for atom cloud in GDML.
  //
  using pld_type = std::piecewise_linear_distribution<double>;
  auto worldLV  = G4LogicalVolumeStore::GetInstance()->GetVolume("World_log");
  auto sourceLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Source_log");

  if (fGunType) {
    G4Box* worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid()); // assume a box
    G4double worldZHalfLength = worldBox->GetZHalfLength();

    // random spot location [mm]
    G4TwoVector loc = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
    fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()*mm, loc.y()*mm, -worldZHalfLength + 1.*cm));
    
    // Gaussian random energy [keV]
    G4double en = G4RandGauss::shoot(fMean, fStdev);
    fParticleGun->SetParticleEnergy(en * keV);
  }
  else {  // else implement tritium beta decay event generator

    // distribution parameter
    int nw = 10000; // nw - number of bins
    double lbound = 0.2; // lower energy bound [keV]
    double ubound = TBeta::endAt(fNumass, 1); // max energy
    
    // create distribution
    pld_type ed(nw, lbound, ubound, betaGenerator(forder, fNumass, 
						  fSterilemass, fSterilemixing));
    
    // random vertex location in cloud [mm]
    G4Tubs* atomTubs = dynamic_cast<G4Tubs*>(sourceLV->GetSolid()); // assume a cylinder
    G4double atomZHalfLength = atomTubs->GetZHalfLength();
    G4double atomRadius      = atomTubs->GetOuterRadius();

    // Beta decay random energy [keV]
    G4double en = ed(generator); // this is with std random
    fParticleGun->SetParticleEnergy(en * keV);
  }

  fParticleGun->GeneratePrimaryVertex(event);
}


void QTPrimaryGeneratorAction::DefineCommands()
{
  // Define /QT/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/QT/generator/", "Primary generator control");

  // gun type command
  auto& typeCmd = fMessenger->DeclareProperty("calibration", fGunType,
					      "Boolean gun type choice: true=electron gun; false=TBetaDecay.");
  typeCmd.SetParameterName("t", true);
  typeCmd.SetDefaultValue("true");

  // energy command
  auto& energyCmd = fMessenger->DeclareProperty("gunEnergy", fMean,
                                               "Mean Gun energy [keV].");
  energyCmd.SetParameterName("d", true);
  energyCmd.SetRange("d>=1.");
  energyCmd.SetDefaultValue("18.575");

  // width command
  auto& widthCmd = fMessenger->DeclareProperty("gunWidth", fStdev,
                                               "Gun energy standard deviation [keV].");
  widthCmd.SetParameterName("w", true);
  widthCmd.SetRange("w>=0.");
  widthCmd.SetDefaultValue("5.e-4");

  // spot size command
  auto& spotCmd = fMessenger->DeclareProperty("gunSpot", fSpot,
                                               "Gun spot diameter [mm].");
  spotCmd.SetParameterName("s", true);
  spotCmd.SetRange("s>=0.");
  spotCmd.SetDefaultValue("0.5");

  // order command
  auto& orderCmd = fMessenger->DeclareProperty("order", fOrder,
					      "Boolean neutrino order choice: true=normal; false=inverted.");
  orderCmd.SetParameterName("o", true);
  orderCmd.SetDefaultValue("true");

  // neutrino mass command
  auto& numuCmd = fMessenger->DeclareProperty("numass", fNumass,
                                               "Neutrino mass [keV].");
  numuCmd.SetParameterName("m", true);
  numuCmd.SetRange("m>=0.");
  numuCmd.SetDefaultValue("1.0e-4");

  // neutrino mass command
  auto& numuCmd = fMessenger->DeclareProperty("mN", fSterilemass,
                                               "Sterile neutrino mass [keV].");
  numuCmd.SetParameterName("n", true);
  numuCmd.SetRange("n>=0.");
  numuCmd.SetDefaultValue("0.0");

  // neutrino mass command
  auto& numuCmd = fMessenger->DeclareProperty("eta", fSterilemixing,
                                               "Sterile neutrino mixing (0.0-1.0).");
  mixCmd.SetParameterName("x", true);
  mixCmd.SetRange("x>=0.");
  mixCmd.SetDefaultValue("0.0");


}
