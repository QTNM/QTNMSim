#include "SCPhysicsList.hh"

#include "G4EmBuilder.hh"
#include "G4EmParameters.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

// photons
#include "G4BetheHeitler5DModel.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4KleinNishinaModel.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LowEPComptonModel.hh"
#include "G4PEEffectFluoModel.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"

// e+-
#include "G4CoulombScattering.hh"
#include "G4Generator2BN.hh"
#include "G4Generator2BS.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4PenelopeIonisationModel.hh"
#include "G4SeltzerBergerModel.hh"
#include "G4UniversalFluctuation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eDPWACoulombScatteringModel.hh"
#include "G4eIonisation.hh"
#include "G4ePairProduction.hh"

#include "G4eplusAnnihilation.hh"

// ions
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4ionIonisation.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4GenericIon.hh"
#include "G4Positron.hh"

#include "G4BuilderType.hh"
#include "G4GammaGeneralProcess.hh"
#include "G4PhysicsListHelper.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(SCPhysicsList);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SCPhysicsList::SCPhysicsList(G4int ver, const G4String &)
    : G4VPhysicsConstructor("SCPhysics"), verbose(ver) {
  G4EmParameters *param = G4EmParameters::Instance();
  param->SetDefaults();
  param->SetVerbose(verbose);
  param->SetMinEnergy(100 * CLHEP::eV);
  param->SetLowestElectronEnergy(100 * CLHEP::eV);
  param->SetNumberOfBinsPerDecade(20);
  param->ActivateAngularGeneratorForIonisation(true);
  param->SetStepFunction(0.2, 10 * CLHEP::um);
  param->SetStepFunctionMuHad(0.1, 50 * CLHEP::um);
  param->SetStepFunctionLightIons(0.1, 20 * CLHEP::um);
  param->SetStepFunctionIons(0.1, 1 * CLHEP::um);
  param->SetMuHadLateralDisplacement(true);
  param->SetFluo(true);
  param->SetMscThetaLimit(0.0);
  param->SetAuger(true);
  param->SetAugerCascade(true);
  param->SetPixe(true);
  param->SetUseICRU90Data(true);
  param->SetMaxNIELEnergy(1 * CLHEP::MeV);
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SCPhysicsList::~SCPhysicsList() = default;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SCPhysicsList::ConstructParticle() {
  // minimal set of particles for EM physics
  G4EmBuilder::ConstructMinimalEmSet();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SCPhysicsList::ConstructProcess() {
  if (verbose > 1) {
    G4cout << "### " << GetPhysicsName() << " Construct Processes " << G4endl;
  }
  G4EmBuilder::PrepareEMPhysics();

  G4PhysicsListHelper *ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // e-
  G4ParticleDefinition *particle = G4Electron::Electron();

  // single scattering
  G4CoulombScattering *ss = new G4CoulombScattering();
  ss->AddEmModel(0, new G4eDPWACoulombScatteringModel(false, false, 0.0));

  ph->RegisterProcess(ss, particle);
}
