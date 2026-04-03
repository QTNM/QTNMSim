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
// File name:     G4EmMultiModel
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 03.05.2004
//
// Modifications:
// 15-04-05 optimize internal interface (V.Ivanchenko)
// 04-07-10 updated interfaces according to g4 9.4 (V.Ivanchenko)
//
// Class Description:
//
// EM model using several G4VEmModels for the same energy interval

// -------------------------------------------------------------------
//

#ifndef G4QTMultiModel_h
#define G4QTMultiModel_h 1

#include "globals.hh"
#include "G4VEmModel.hh"
#include "G4ParticleDefinition.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4Material.hh"
#include <vector>

class G4DynamicParticle;

class QTMultiModel :  public G4VEmModel
{
public:

  explicit QTMultiModel(const G4String& nam = "QTMultiModel");

  ~QTMultiModel() override;

  void AddModel(G4VEmModel*);

  void Initialise(const G4ParticleDefinition*, const G4DataVector&) override;

  void InitialiseLocal(const G4ParticleDefinition*, G4VEmModel*) override;

  // main method to compute cross section per atom
  G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
				      G4double kinEnergy,
				      G4double Z,
				      G4double A = 0., /* amu */
				      G4double cutEnergy = 0.0,
				      G4double maxEnergy = DBL_MAX) override;

  void SampleSecondaries(std::vector<G4DynamicParticle*>*,
			 const G4MaterialCutsCouple*,
			 const G4DynamicParticle*,
			 G4double tmin,
			 G4double tmax) override;

  G4double MinPrimaryEnergy(const G4Material*, const G4ParticleDefinition*,
                            G4double) override { return 10.0*CLHEP::eV; }

  //  hide assignment operator
  QTMultiModel & operator=(const  QTMultiModel &right) = delete;
  QTMultiModel(const  QTMultiModel&) = delete;

private: 

  G4int                         nModels = 0;
  std::vector<G4VEmModel*>      model;
  std::vector<G4double>         cross_section;

};

#endif

