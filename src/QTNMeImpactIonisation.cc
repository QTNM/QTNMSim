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

#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

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
: G4VEmModel("eImpactIonisation"),
  fParticleChange(nullptr)
{
  SetLowEnergyLimit (  0.0*CLHEP::eV);  // ekin = 10 eV   is used if (E< 10  eV)
  SetHighEnergyLimit(100.0*CLHEP::MeV); // ekin = 100 MeV is used if (E>100 MeV)
  secondary_energy.reserve(nESpace);
  // Angular sampling points for secondary scatter
  const G4double a = CLHEP::pi / 180.0;
  std::generate(angle.begin(), angle.end(), [n = 0, &a]() mutable { return n++ * a; });
  generator.seed(rd()); // using random seed
}


QTNMeImpactIonisation::~QTNMeImpactIonisation()
{
}


void QTNMeImpactIonisation::Initialise(const G4ParticleDefinition* pdef,
                                               const G4DataVector& prodcuts)
{
  fParticleChange = GetParticleChangeForGamma();
  if(IsMaster()) {
    // init only for the elements that are used in the geometry
    G4ProductionCutsTable* theCpTable = G4ProductionCutsTable::GetProductionCutsTable();
    G4int numOfCouples = (G4int)theCpTable->GetTableSize();
    for(G4int j=0; j<numOfCouples; ++j) {
      const G4Material* mat = theCpTable->GetMaterialCutsCouple(j)->GetMaterial();
      const G4ElementVector* elV = mat->GetElementVector();
      std::size_t numOfElem = mat->GetNumberOfElements();
      for (std::size_t ie = 0; ie < numOfElem; ++ie) {
	G4int Z = (*elV)[ie]->GetZasInt();
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
  SetBindingEnergies(static_cast<QTNMeImpactIonisation*>(masterModel)->GetBindingEnergies());
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

  // Z, int
  G4int z_int = (int) Z;
  if (z_int > z_max) return 0.0;

  // Get binding energies for material
  std::vector<G4double> bind_vals = get_ionisation_energies(z_int);

  // Number of electrons interior and up to current shell
  G4int n_ele_int = 0;
  G4double sigma = 0;
  for (G4int i = 0; i < bind_vals.size(); ++i) {
    G4double bind = bind_vals[i];

    // Quantum number n
    const G4int n = table_n[i];
    // Quantum number l
    const G4int l = table_l[i];

    // Number of electrons in this orbit
    G4int n_ele;
    if (l == 0) {
      // 2 Electrons
      n_ele = std::min(2, z_int - n_ele_int);
      // Always increment n_ele_int in this case
      n_ele_int += 2;
    } else {
      // Number of electrons with distint j
      n_ele = std::min(2*l + 1, z_int - n_ele_int);
      // Only add electrons if subshell above has different n,l
      // i.e. we have processed j=|l+1/2| and j=|l-1/2|
      if (table_n[i+1] != n || table_l[i+1] != l) {
	n_ele_int += 4*l + 2;
      }
    }

    n_ele_int = std::min(z_int, n_ele_int);

    // Effective charge
    // Z - # electrons up to and including this orbit
    const G4int z_eff = z_int - n_ele_int;

    G4double U = T_ev / bind;

    if (U < 1.0) {
      continue;
    }
    G4double J = 512375 / bind; // Assumes units of eV

    G4double gr = mbell_gr(U, J);
    G4double f_ion = mbell_f_ion(z_eff, U, z_int, mbell_lambda[l]);

    G4double bsum = 0.0;
    for (int i=0; i<7; i++) {
      bsum += mbell_b[n-1][l][i] * pow(1.0 - 1.0 / U, i+1);
    }

    sigma += n_ele * f_ion * gr * (mbell_a[n-1][l] * std::log(U) + bsum) / (bind * T_ev); // cm^2
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

  // Incident energy dependent terms
  const G4double mc2_ev = 511e3;
  const G4double t_prime = T_ev / mc2_ev;
  const G4double beta_t2 = 1 - 1 / pow(1 + t_prime,2);

  // Physical constants
  const G4double alpha = CLHEP::fine_structure_const;
  const G4double aB = 5.29e-11;

  std::vector<G4double> cdf(nESpace);

  std::vector<G4double> bind_vals = get_ionisation_energies(izet);

  // Maximum energy of secondary particle
  const G4double emax = 0.5 * (T_ev - *(std::min_element(bind_vals.begin(), bind_vals.end())));
  secondary_energy = logspace(std::log10(1e-6*T_ev), std::log10(emax), nESpace);

  const G4int nShells = bind_vals.size();

  for (G4int i = 0; i < nESpace; i++) {
    G4double cdf_sum = 0.0;
    for (G4int j = 0; j <nShells; ++j) {
      G4double bind = bind_vals[j];
      if(T_ev < bind) continue;

      //RBEB terms
      G4double b_prime = bind / mc2_ev;
      G4double t = T_ev / bind;
      G4double w = secondary_energy[i] / bind;
      G4double beta_b2 = 1 - 1./ pow(1 + b_prime, 2);
      G4double beta2 = beta_t2 + 2*beta_b2;

      // Pre-factor terms which depend on B or N
      G4double pre_fac = 0.5 * (1 + beta2 / beta_t2);
      G4double fac = 2 * CLHEP::pi * pow(aB,2) * pow(alpha,4) * nShells / (beta2 * b_prime);

      // Calculate CDF contribution from this shell for this W
      G4double A1 = 0.5 * (pow(t - w,-2) - pow(w + 1,-2) - pow(t,-2) + 1);
      G4double A2 = std::log(beta_t2 / (1 - beta_t2)) - beta_t2 - std::log(2*b_prime);
      G4double A3 = 1/(t - w) - 1/(w + 1) - 1/t + 1;
      G4double A4 = pow(b_prime,2) / pow(1 + 0.5*t_prime,2) * w;
      G4double A5 = std::log((w + 1)/(t - w)) - std::log(1/t);
      G4double A6 = (1 + 2*t_prime) / pow(1 + 0.5*t_prime, 2) / (t+1);

      cdf_sum += pre_fac * fac * (A1*A2 + A3 + A4 - A5*A6);
    }
    cdf.push_back(cdf_sum);
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

  // Original direction of particle in lab frame
  G4ThreeVector dir_lab = dp->GetMomentumDirection();

  // Primary energy post collision
  G4double prim_new = T_ev - enew - bind_vals[0];
  G4double w = prim_new / bind_vals[0];
  G4double t = T_ev / bind_vals[0];
  for (G4int i = 1; i < nAngles; i++) {
    angle_pdf[i] = fbe(w, t, angle[i]) + fb(w, t, angle[i]);
  }

  std::piecewise_constant_distribution<> dist(angle.begin(),
					      angle.end(),
					      angle_pdf.begin());

  // Deflection of primary particle
  G4double cost = std::cos(dist(generator));
  G4double sint = std::sqrt((1.0-cost)*(1.0+cost));
  G4double phi  = CLHEP::twopi*rndmEngine->flat();
  G4ThreeVector theNewDirection(sint*std::cos(phi), sint*std::sin(phi), cost);

  // get original direction in lab frame and rotate new direction to lab frame
  theNewDirection.rotateUz(dir_lab);

  // Set new direction
  fParticleChange->ProposeMomentumDirection(theNewDirection);
  // Reduce kinetic energy
  fParticleChange->SetProposedKineticEnergy(prim_new * CLHEP::eV);
  fParticleChange->ProposeLocalEnergyDeposit(bind_vals[0] * CLHEP::eV);

  // Add secondary particle

  w = enew / bind_vals[0];
  for (G4int i = 1; i < angle.size(); i++) {
    angle_pdf[i] = fbe(w, t, angle[i]) + fb(w, t, angle[i]);
  }

  std::piecewise_constant_distribution<> dist2(angle.begin(),
					       angle.end(),
					       angle_pdf.begin());

  cost = std::cos(dist2(generator));
  sint = std::sqrt((1.0-cost)*(1.0+cost));
  phi  = CLHEP::twopi*rndmEngine->flat();
  theNewDirection.set(sint*std::cos(phi), sint*std::sin(phi), cost);
  theNewDirection.rotateUz(dir_lab);
  auto newp = new G4DynamicParticle (G4Electron::Electron(), theNewDirection, enew * CLHEP::eV);
  fvect->push_back(newp);

}

G4double
QTNMeImpactIonisation::fbe(G4double w, G4double t, G4double theta)
{
  G4double G2 = std::sqrt((w+1)/t);
  G4double G3 = 0.6 * std::sqrt((1 - pow(G2,2)) / w);
  return 1.0 / pow(1.0 + (std::cos(theta) - G2) / G3, 2);
}

G4double
QTNMeImpactIonisation::fb(G4double w, G4double t, G4double theta)
{
  G4double G5 = 1.0 / 3.0;
  G4double gamma = 10.0;
  G4double G4 = gamma * pow(1 - w/t,3) / t / (w + 1);
  return G4 / pow(1.0 + (std::cos(theta) + 1) / G5, 2);
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
QTNMeImpactIonisation::mbell_f_ion(G4int z_eff, G4double U, G4int Z, G4double m_lambda)
{
  return 1 + mbell_m * pow( z_eff/(U*Z), m_lambda);
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
  if (binding_energies.count(Z) > 0) return;
  auto fname = project_root + "/src/tables/binding_energy/be_" + std::to_string(Z);
  std::ifstream input;
  input.open(fname);
  // Skip Header
  input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::string line;
  while (std::getline(input >> std::ws, line))
    {
      // Skip first character, the ionisation level
      std::istringstream iss(line.substr(1, line.size()-1));
      // Convert remaining numbers into a vector, and delete 0 elements.
      std::vector<G4double> elevels = std::vector<double>{std::istream_iterator<double>(iss), std::istream_iterator<double>()};
      elevels.erase(std::remove(elevels.begin(), elevels.end(), 0.0), elevels.end());
      binding_energies[Z] = elevels;
      // Could at this stage read in levels for ions
      // but break instead
      break;
    }

  if (Z > z_max) {
    std::ostringstream msg;
    msg << "Impact ionisation cross section unavailable for Z  = "
	<< Z
	<< ". Cross section will be set to 0.0";
    G4Exception("QTNMeImpactIonisation::load_ionisation_energies:",
		"No Cross Section", JustWarning, msg);
  }
}
