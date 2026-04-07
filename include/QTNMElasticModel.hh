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
//
// Modified for QTNM project
// Y. Ramachers
// University of Warwick, 2026
//
// -------------------------------------------------------------------



#ifndef QTNMElasticModel_h
#define QTNMElasticModel_h 1

#include "G4VEmModel.hh"
#include "globals.hh"

class G4eDPWAElasticDCS;
class G4ParticleChangeForGamma;
class G4ParticleDefinition;
class G4DataVector;

class QTNMElasticModel : public G4VEmModel {

public:

  /**
   * Constructor.
   */
  QTNMElasticModel();

  ~QTNMElasticModel() override;

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
                            G4double) override;


private:

  // the object that provides cross sections and polar angle of scattering
  G4eDPWAElasticDCS*         fTheDCS;
  // particle change
  G4ParticleChangeForGamma*  fParticleChange;

};

#endif
