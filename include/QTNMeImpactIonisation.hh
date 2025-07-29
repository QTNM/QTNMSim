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
// Class Description:
//
// e-/e+ Coulomb scattering model based on numerical Differential Cross Sections
// (DCS) obtained by Dirac Partial Wave Analysis (DPWA) and supplied by the
// G4eDPWAElasticDCS class.
// The model contains the possibility to incorporate the effects of angular
// deflections of sub-threshold ionisation intercations when it's described by
// the condensed history model. Note, this must be inactivated (by setting the
// `isscpcor` input argument of the CTR to false) when ionisation is described
// with a classical, event by event based simulation model instead of usign the
// condensed history approach (otherwise, the corresponding angular defelctions
// will be "double counted").
//
// -------------------------------------------------------------------
#define STRING(x) #x
#define XSTRING(x) STRING(x)


#ifndef QTNMeImpactIonisation_h
#define QTNMeImpactIonisation_h 1

#include "G4VEmModel.hh"
#include "globals.hh"
#include <map>
#include <random>

class G4ParticleChangeForGamma;
class G4ParticleDefinition;
class G4DataVector;

class QTNMeImpactIonisation : public G4VEmModel {

public:

  /**
   * Constructor.
   *
   */
  QTNMeImpactIonisation();

  ~QTNMeImpactIonisation() override;

  //
  // Interface methods:

  void     Initialise(const G4ParticleDefinition*, const G4DataVector&) override;

  void     InitialiseLocal(const G4ParticleDefinition*, G4VEmModel*) override;

  G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition*, G4double ekin,
                                      G4double Z, G4double A, G4double prodcut,
                                      G4double emax) override;

  void     SampleSecondaries(std::vector<G4DynamicParticle*>*,
                             const G4MaterialCutsCouple*,
                             const G4DynamicParticle*,
                             G4double tmin,
                             G4double maxEnergy) override;

  G4double MinPrimaryEnergy(const G4Material*, const G4ParticleDefinition*,
                            G4double) override { return 10.0*CLHEP::eV; }

  std::map<int, std::vector<G4double>> GetBindingEnergies() { return binding_energies; }
  void SetBindingEnergies(std::map<int, std::vector<G4double>> be) { binding_energies = be; }

private:

  void                      load_ionisation_energies(G4int Z);
  std::vector<G4double>     get_ionisation_energies(G4int Z) { return binding_energies[Z]; };
  G4double                  mbell_gr(G4double U, G4double J);
  G4double                  mbell_f_ion(G4int z_eff, G4double U, G4int Z, G4double m_lambda);
  G4double                  fbe(G4double w, G4double t, G4double theta);
  G4double                  fb(G4double w, G4double t, G4double theta);
  // particle change
  G4ParticleChangeForGamma*  fParticleChange;
  // Energy space for secondary CDF
  std::vector<G4double> secondary_energy;
  // Theis could be made variable
  static const G4int nESpace = 200;
  std::vector<G4double> logspace(const G4double a, const G4double b, const G4int n);
  // Angular sampling
  static const G4int nAngles = 181;
  std::vector<G4double> angle = std::vector<G4double>(nAngles);
  std::vector<G4double> angle_pdf = std::vector<G4double>(nAngles);
  // Ionisation energies
  std::map<int, std::vector<G4double>> binding_energies;
  // Parameters for MBELL model
  const G4int mbell_m = 3; // Fixed upto 3P
  const G4double mbell_lambda[3] = {1.270, 0.542, 0.950};  // Function of l. MBELL beyond l=1 unwise
  // a, b units of eV^2 cm^2
  const G4double mbell_a[3][2] = { {0.525e-13, 0.0} , {0.53e-13, 0.6e-13} , {0.13e-13, 0.388e-13}};
  const G4double mbell_b[3][2][7] =
    { { {-0.510e-13, 0.2000e-13, 0.0500e-13, -0.025e-13, -0.100e-13, 0.00e-13, 0.00e-13},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0} },
      { {-0.410e-13,  0.1500e-13,  0.1500e-13, -0.200e-13, -0.150e-13,  0.00e-13,  0.00e-13},
	{-0.400e-13, -0.7100e-13,  0.6550e-13,  0.425e-13, -0.750e-13,  0.00e-13,  0.00e-13} },
      { {0.250e-13, -1.5000e-13,  2.4000e-13,  3.220e-13, -3.667e-13,  0.00e-13,  0.00e-13},
	{-0.200e-13, -0.2356e-13,  0.5355e-13,  3.150e-13, -8.500e-13,  5.05e-13,  0.37e-13}}};
  const G4int table_n[29] = {1, 2, 2, 2, 3, 3, 3, 4, 3, 3, 4, 4, 5, 4, 4, 5, 5, 6, 4, 4, 5, 5, 6, 6, 7, 5, 5, 6, 6};
  const G4int table_l[29] = {0, 0, 1, 1, 0, 1, 1, 0, 2, 2, 1, 1, 0, 2, 2, 1, 1, 0, 3, 3, 2, 2, 1, 1, 0, 3, 3, 2, 2};
  std::string project_root = XSTRING(SOURCE_ROOT);
  // Maximum Z allowable
  const G4int z_max = 18;

  std::ranlux24       generator;
  std::random_device  rd; // for random seeds
};

#endif
