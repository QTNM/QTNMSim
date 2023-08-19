// us
#include "QTPrimaryGeneratorAction.hh"

#include <cmath>

// geant
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4RandomTools.hh"
#include "G4RandomDirection.hh"


QTPrimaryGeneratorAction::QTPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
, fParticleGun(nullptr)
, fParticleGPS(nullptr)
, fMessenger(nullptr)
, fMean(18.575) // energy [keV]
, fStdev(5.e-4) // for E-gun
, fSpot(0.5)    // for E-gun
, fTritium(false) // switch to Trtium beta decay generator
, fEGun(false) // calibration: true=electron gun
, fTestElectron(false)  // 90 degree electron for testing, override other guns
, fOrder(true)   // neutrino hierarchie: true=normal
, fNumass(1.0e-4)
, fSterilemass(0.0)
, fSterilemixing(0.0)
{
  generator.seed(rd()); // using random seed

  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);
  fParticleGPS       = new G4GeneralParticleSource(); // can now be used with macro commands

  auto particleTable = G4ParticleTable::GetParticleTable();

  // default electron particle for gun and GPS
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("e-"));
  // set up a simple default, if macro gives no instructions
  fParticleGPS->SetParticleDefinition(particleTable->FindParticle("e-"));
  fParticleGPS->SetNumberOfParticles(nofParticles);
  fParticleGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
  fParticleGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0.0,0.0,0.0));
  fParticleGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fMean * keV);
  fParticleGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(1.0,0.0,0.0));

  DefineCommands();
}

QTPrimaryGeneratorAction::~QTPrimaryGeneratorAction()
{
  delete fMessenger;
  delete fParticleGun;
  delete fParticleGPS;
}

void QTPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore: assumes name is worldLV!
  // from G4LogicalVolumeStore: assumes source name is Gas_log!
  // Check: Name requirement for GDML file AND axis assumption!
  // Check: G4Tubs assumption for atom cloud in GDML.
  //
  using pld_type = std::piecewise_linear_distribution<double>;
  auto worldLV  = G4LogicalVolumeStore::GetInstance()->GetVolume("worldLV");
  auto sourceLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Gas_log");

  if (fTestElectron) { // a 90 degree emission electron for testing, default: false
    fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0)); // origin
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1.0, 0.0, 0.0)); // x-direction
    fParticleGun->SetParticleEnergy(fMean * keV); // defaults to 18.575 keV
    fParticleGun->GeneratePrimaryVertex(event);
    return;
  }

  else if (fEGun) { // electron gun
    G4Box* worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid()); // assume a box
    G4double worldZHalfLength = worldBox->GetZHalfLength();

    // random spot location [mm]
    G4TwoVector loc = G4RandomPointInEllipse(fSpot/2.0, fSpot/2.0); // circle
    fParticleGun->SetParticlePosition(G4ThreeVector(loc.x()*mm, loc.y()*mm, -worldZHalfLength + 1.*cm));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0)); // z-direction

    // Gaussian random energy [keV]
    G4double en = G4RandGauss::shoot(fMean, fStdev);
    fParticleGun->SetParticleEnergy(en * keV);
    fParticleGun->GeneratePrimaryVertex(event);
    return;
  }
  else if (fTritium) {  // use tritium beta decay event generator

    // distribution parameter
    int nw = 10000; // nw - number of bins
    double lbound = 1.5; // lower energy bound [keV]
    double ubound = TBeta::endAt(fNumass, 1); // max energy

    // create distribution
    pld_type ed(nw, lbound, ubound, betaGenerator(fOrder, fNumass,
						  fSterilemass, fSterilemixing));

    // random vertex location in atom cloud [mm]
    G4Tubs* atomTubs = dynamic_cast<G4Tubs*>(sourceLV->GetSolid()); // assume a cylinder
    G4double atomZHalfLength = atomTubs->GetZHalfLength();
    G4double atomRadius      = atomTubs->GetOuterRadius();
    G4double phi             = CLHEP::twopi * G4UniformRand();
    G4double rad             = G4UniformRand() * atomRadius;
    G4double zpos            = -atomZHalfLength + 2.0*atomZHalfLength*G4UniformRand();
    fParticleGun->SetParticlePosition(G4ThreeVector(rad*std::cos(phi)*mm, rad*std::sin(phi)*mm, zpos*mm));
    fParticleGun->SetParticleMomentumDirection(G4RandomDirection()); // 4 pi solid angle

    // Beta decay random energy [keV]
    G4double en = ed(generator); // this is with std random
    fParticleGun->SetParticleEnergy(en * keV);
    fParticleGun->GeneratePrimaryVertex(event);
    return;
  }
  // if nothing else, use the GPS
  fParticleGPS->GeneratePrimaryVertex(event);
}


void QTPrimaryGeneratorAction::DefineCommands()
{
  // Define /QT/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/QT/generator/", "Primary generator control");

  // gun type command
  auto& testCmd = fMessenger->DeclareProperty("test", fTestElectron,
					      "Boolean gun testing choice: true=electron at 90 degrees.");
  testCmd.SetParameterName("tt", true);
  testCmd.SetDefaultValue("false");

  // gun type command
  auto& gunCmd = fMessenger->DeclareProperty("egun", fEGun,
					      "Boolean true=use electron gun.");
  gunCmd.SetParameterName("t", true);
  gunCmd.SetDefaultValue("false");

  // gun type command
  auto& typeCmd = fMessenger->DeclareProperty("tritium", fTritium,
					      "Boolean true=use TBetaDecay.");
  typeCmd.SetParameterName("ttt", true);
  typeCmd.SetDefaultValue("false");

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
  auto& numuCmd1 = fMessenger->DeclareProperty("numass", fNumass,
                                               "Neutrino mass [keV].");
  numuCmd1.SetParameterName("m", true);
  numuCmd1.SetRange("m>=0.");
  numuCmd1.SetDefaultValue("1.0e-4");

  // neutrino mass command
  auto& numuCmd2 = fMessenger->DeclareProperty("mN", fSterilemass,
                                               "Sterile neutrino mass [keV].");
  numuCmd2.SetParameterName("n", true);
  numuCmd2.SetRange("n>=0.");
  numuCmd2.SetDefaultValue("0.0");

  // neutrino mass command
  auto& mixCmd = fMessenger->DeclareProperty("eta", fSterilemixing,
                                               "Sterile neutrino mixing (0.0-1.0).");
  mixCmd.SetParameterName("x", true);
  mixCmd.SetRange("x>=0.");
  mixCmd.SetDefaultValue("0.0");


}
