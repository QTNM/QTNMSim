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



#ifndef QTeDPWACoulombScatteringModel_h
#define QTeDPWACoulombScatteringModel_h 1

#include "G4VEmModel.hh"
#include "globals.hh"

class G4eDPWAElasticDCS;
class G4ParticleChangeForGamma;
class G4ParticleDefinition;
class G4DataVector;

class QTeDPWACoulombScatteringModel : public G4VEmModel {

public:

  /**
   * Constructor.
   *
   * @param[in] mumin    When the model is used for mixed simulation, Coulomb
   *                     scatterings, resulting in a minimum t_c polar angular
   *                     deflection, modelled explicitly. Therefore, cross
   *                     sections are computed, and angular deflections are
   *                     sampled ina resricted [\theta_c,\pi] interval. The
   *                     minimum of this interval is determined by the mumin
   *                     parameter as:
   *                     \mu_{min} = \mu(\theta_c)=0.5[1-\cos(\theta_c)]
   */
  QTeDPWACoulombScatteringModel(G4double mumin=0.0);

  ~QTeDPWACoulombScatteringModel() override;

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

  void     SetTheDCS(G4eDPWAElasticDCS* theDCS) { fTheDCS = theDCS; }

  G4eDPWAElasticDCS* GetTheDCS() { return fTheDCS; }


private:

  G4double mbell_gr(G4double U, G4double J);
  G4double mbell_f_ion(G4int el_no, G4int nu, G4double U, G4double m_lambda);
  // mu(theta)=0.5[1-cos(theta)]: the model porvides final states \in [fMuMin,1]
  G4double                   fMuMin;
  // the object that provides cross sections and polar angle of scattering
  G4eDPWAElasticDCS*         fTheDCS;
  // particle change
  G4ParticleChangeForGamma*  fParticleChange;
  // Energy space for secondary CDF
  std::vector<G4double> secondary_energy;
  // Theis could be made variable
  static const G4int nESpace = 200;
  std::vector<G4double> logspace(const G4double a, const G4double b, const G4int n);
};

#endif
