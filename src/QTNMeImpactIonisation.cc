//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4eDPWACoulombScatteringModel
//
// Author:        Mihaly Novak
//
// Creation date: 02.07.2020
//
// Modifications:
//
// -------------------------------------------------------------------

#include "QTNMeImpactIonisation.hh"

#include "G4eDPWAElasticDCS.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4DataVector.hh"

#include "G4ProductionCutsTable.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"

#include "G4Electron.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4ThreeVector.hh"

#include "G4AnalysisManager.hh"

QTNMeImpactIonisation::QTNMeImpactIonisation()
: G4VEmModel("eDPWACoulombScattering"),
  fTheDCS(nullptr),
  fParticleChange(nullptr)
{
  SetLowEnergyLimit (  0.0*CLHEP::eV);  // ekin = 10 eV   is used if (E< 10  eV)
  SetHighEnergyLimit(100.0*CLHEP::MeV); // ekin = 100 MeV is used if (E>100 MeV)
}


QTNMeImpactIonisation::~QTNMeImpactIonisation()
{
  if (IsMaster()) {
    delete fTheDCS;
  }
}


void QTNMeImpactIonisation::Initialise(const G4ParticleDefinition* pdef,
                                               const G4DataVector& prodcuts)
{
  fParticleChange = GetParticleChangeForGamma();
  if(IsMaster()) {
    // clean the G4eDPWAElasticDCS object if any
    delete fTheDCS;
    fTheDCS = new G4eDPWAElasticDCS(pdef==G4Electron::Electron(), false);
    // init only for the elements that are used in the geometry
    G4ProductionCutsTable* theCpTable = G4ProductionCutsTable::GetProductionCutsTable();
    G4int numOfCouples = (G4int)theCpTable->GetTableSize();
    for(G4int j=0; j<numOfCouples; ++j) {
      const G4Material* mat = theCpTable->GetMaterialCutsCouple(j)->GetMaterial();
      const G4ElementVector* elV = mat->GetElementVector();
      std::size_t numOfElem = mat->GetNumberOfElements();
      for (std::size_t ie = 0; ie < numOfElem; ++ie) {
	G4int Z = (*elV)[ie]->GetZasInt();
        fTheDCS->InitialiseForZ(Z);
	// Load the ionisation energies
	load_ionisation_energies(Z);
      }
    }
    // will make use of the cross sections so the above needs to be done before
    InitialiseElementSelectors(pdef, prodcuts);
  }
}


void QTNMeImpactIonisation::InitialiseLocal(const G4ParticleDefinition*,
                                                    G4VEmModel* masterModel)
{
  SetElementSelectors(masterModel->GetElementSelectors());
  SetTheDCS(static_cast<QTNMeImpactIonisation*>(masterModel)->GetTheDCS());
}


G4double
QTNMeImpactIonisation::ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
                                                          G4double ekin,
                                                          G4double Z,
                                                          G4double /*A*/,
                                                          G4double /*prodcut*/,
                                                          G4double /*emax*/)
{
  G4double T_ev = ekin / CLHEP::eV;

  // MBell constants. Units of eV^2 cm^2
  const G4double mbell_a = 0.525e-13;
  const G4double mbell_b[7] = {-0.510e-13, 0.2000e-13, 0.0500e-13, -0.025e-13, -0.100e-13, 0.00e-13, 0.00e-13};
  const G4int mbell_m = 3;
  const G4double mbell_lambda[3] = {1.270, 0.542, 0.950};

  // Potentially dangerous conversion to int?
  std::vector<G4double> bind_vals = get_ionisation_energies((int) Z);

  G4double sigma = 0;
  for (G4double bind : bind_vals) {
    // Set elsewhere?
    const G4int el_no = 1; // Number of electrons
    G4double U = T_ev / bind;
    G4double J = 512375 / bind; // Assumes units of eV

    // These should be shell dependent
    const G4int nu = 1;
    const G4int n = 1;
    const G4int l = 0;

    G4double gr = mbell_gr(U, J);
    G4double f_ion = mbell_f_ion(el_no, nu, U, mbell_lambda[l]);

    G4double bsum = 0.0;
    for (int i=0; i<7; i++) {
      bsum += mbell_b[i] * pow(1.0 - 1.0 / U, i+1);
    }

    sigma += f_ion * gr * (mbell_a * std::log(U) + bsum) / (bind * T_ev); // cm^2
  }

  // G4cout<< T_ev <<  ", " << sigma * f_ion * gr << G4endl;
  return sigma * CLHEP::cm * CLHEP:: cm;
}


void
QTNMeImpactIonisation::SampleSecondaries(std::vector<G4DynamicParticle*>* fvect,
                                                 const G4MaterialCutsCouple* cp,
                                                 const G4DynamicParticle* dp,
                                                 G4double, G4double)
{
  const G4double    ekin   = dp->GetKineticEnergy();
  const G4double    lekin  = dp->GetLogKineticEnergy();
  const G4Element*  target = SelectTargetAtom(cp, dp->GetParticleDefinition(), ekin, lekin);
  const G4int       izet   = target->GetZasInt();

  const G4double T_ev = ekin / CLHEP::eV;

  G4double bind = get_ionisation_energies(izet)[0];

  if(T_ev < bind) return;

  // Maximum energy of secondary particle
  const G4double emax = 0.5 * (T_ev / bind - 1.0) * bind;
  // G4cout << emax << G4endl;
  secondary_energy = logspace(std::log10(1e-6*T_ev), std::log10(emax), nESpace);

  // Incident energy dependent terms
  const G4double mc2_ev = 511e3;
  const G4double t_prime = T_ev / mc2_ev;
  const G4double beta_t2 = 1 - 1 / pow(1 + t_prime,2);

  // Physical constants
  const G4double alpha = CLHEP::fine_structure_const;
  const G4double aB = 5.29e-11;

  // Number of shells
  const G4int N = 1;

  std::vector<G4double> cdf;
  cdf.reserve(nESpace);

  for (int i = 0; i < nESpace; i++) {
    //RBEB terms
    G4double b_prime = bind / mc2_ev;
    G4double t = T_ev / bind;
    G4double w = secondary_energy[i] / bind;
    G4double beta_b2 = 1 - 1./ pow(1 + b_prime, 2);
    G4double beta2 = beta_t2 + 2*beta_b2;

    // Pre-factor terms which depend on B or N
    G4double pre_fac = 0.5 * (1 + beta2 / beta_t2);
    G4double fac = 2 * CLHEP::pi * pow(aB,2) * pow(alpha,4) * N / (beta2 * b_prime);

    // Calculate CDF contribution from this shell for this W
    G4double A1 = 0.5 * (pow(t - w,-2) - pow(w + 1,-2) - pow(t,-2) + 1);
    G4double A2 = std::log(beta_t2 / (1 - beta_t2)) - beta_t2 - std::log(2*b_prime);
    G4double A3 = 1/(t - w) - 1/(w + 1) - 1/t + 1;
    G4double A4 = pow(b_prime,2) / pow(1 + 0.5*t_prime,2) * w;
    G4double A5 = std::log((w + 1)/(t - w)) - std::log(1/t);
    G4double A6 = (1 + 2*t_prime) / pow(1 + 0.5*t_prime, 2) / (t+1);

    cdf.push_back(pre_fac * fac * (A1*A2 + A3 + A4 - A5*A6));
  }

  // Normalise
  for (int i = 0; i < nESpace; i++) {
   cdf[i] /= cdf[nESpace-1];
  }

  CLHEP::HepRandomEngine* rndmEngine = G4Random::getTheEngine();
  G4double rndm = rndmEngine->flat();

  auto lower = std::lower_bound(cdf.begin(), cdf.end(), rndm);

  G4double fac1 = (*lower - rndm) / (*lower - *(lower-1));
  G4double fac2 = 1.0 - fac1;

  G4int i2 = std::distance(cdf.begin(), lower);
  G4double enew = fac1 * secondary_energy[i2-1] + fac2 * secondary_energy[i2];

  // Original direction of particle
  G4ThreeVector dir = dp->GetMomentumDirection();
  // set new direction
  // fParticleChange->ProposeMomentumDirection(theNewDirection);
  // G4cout << T_ev << ", " << enew << G4endl;
  // New electron
  auto newp = new G4DynamicParticle (G4Electron::Electron(),dir,enew * CLHEP::eV);
  fvect->push_back(newp);

  fParticleChange->SetProposedKineticEnergy((T_ev - enew - bind) * CLHEP::eV);
  fParticleChange->ProposeLocalEnergyDeposit(bind * CLHEP::eV);
}

G4double
QTNMeImpactIonisation::mbell_gr(G4double U, G4double J)
{
  G4double a = (1.0 + 2.0*J) / (U + 2.0*J);
  G4double b = pow( (U + J) / (1.0 + J), 2);
  G4double c = pow( (1+U)*(U+2.0*J)*pow(1+J,2) / (pow(J,2) * (1.0+2.0*J) + U*(U+2.0*J)* pow(1.0+J,2)) ,3);
  return a*b*std::sqrt(c);
}

G4double
QTNMeImpactIonisation::mbell_f_ion(G4int el_no, G4int nu, G4double U, G4double m_lambda)
{
  return 1 + 3*pow( (el_no - nu)/(U*el_no), m_lambda);
}


std::vector<G4double>
QTNMeImpactIonisation::logspace(const G4double a, const G4double b, const G4int n)
{
  std::vector<G4double> _logspace;
  _logspace.reserve(nESpace);
  const G4double fac = (b - a) / (n - 1);
  for (int i = 0; i < n; i++) {
    _logspace.push_back(pow(10, i * fac + a));
  }
  return _logspace;
}

void
QTNMeImpactIonisation::load_ionisation_energies(G4int Z)
{
  if (Z != 0) {
    std::ostringstream msg;
    msg << "Invalid material with Z  = "
	<< Z
	<< " used for QTNM Impact Ionisation ";
    G4Exception("QTNMeImpactIonisation::load_ionisation_energies:",
		"InvalidMaterial", FatalException, msg);
  }
}
