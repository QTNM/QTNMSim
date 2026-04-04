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
// GEANT4 Class file
//
//
// File name:   G4EmMultiModel
//
// Author:        Vladimir Ivanchenko
// 
// Creation date: 03.05.2004
//
// Modifications: 
// 15-04-05 optimize internal interface (V.Ivanchenko)
// 04-07-10 updated interfaces according to g4 9.4 (V.Ivanchenko)
//


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QTMultiModel.hh"
#include "Randomize.hh"
#include "G4EmParameters.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QTMultiModel::QTMultiModel(const G4String& nam)
  : G4VEmModel(nam)
{
  //  model.clear();
  cross_section.clear();
  cross_section.resize(2, 0.0);

  // add common limits to multimodel
  SetLowEnergyLimit (  0.0*CLHEP::eV);  // ekin = 10 eV   is used if (E< 10  eV)
  SetHighEnergyLimit(100.0*CLHEP::MeV); // ekin = 100 MeV is used if (E>100 MeV)

  // fixed models in wrapper
  es = new G4eDPWACoulombScatteringModel(false, false);
  es->SetPolarAngleLimit(0.0); // No mixed model
  inel = new QTNMeImpactIonisation();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//QTMultiModel::~QTMultiModel() = default;
QTMultiModel::~QTMultiModel()
{
  delete es;
  delete inel;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void QTMultiModel::AddModel(G4VEmModel* p)
// {
//   cross_section.push_back(0.0);
//   model.push_back(p);
//   ++nModels;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QTMultiModel::Initialise(const G4ParticleDefinition* p, 
                                const G4DataVector& cuts)
{
  G4EmParameters* param = G4EmParameters::Instance();
  // Our models wrap Initialise() in master clause
  if (IsMaster()) {
    //  G4int verb = IsMaster() ? param->Verbose() : param->WorkerVerbose();
    G4int verb = param->Verbose();
    if(verb > 0) {
      G4cout << "### Initialisation of EM MultiModel " << GetName() << G4endl;
	//	     << " including following list of " << nModels << " models:" << G4endl;
    }
    //    for(G4int i=0; i<nModels; ++i) {
    //      G4cout << "    " << (model[i])->GetName();
    G4cout << "    " << es->GetName() << "    " << inel->GetName();
      //      (model[i])->SetParticleChange(pParticleChange, GetModelOfFluctuations());
      //      (model[i])->Initialise(p, cuts);
    es->Initialise(p, cuts);
    inel->Initialise(p, cuts);
    //    }
    if(verb > 0) { G4cout << G4endl; }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void QTMultiModel::InitialiseLocal(const G4ParticleDefinition* p,
// 				   G4VEmModel* masterModel)
// {
//   // for(G4int i=0; i<nModels; ++i) {
//   //   (model[i])->InitialiseLocal(p, model[i]);
//   // }
//   es->InitialiseLocal(p, static_cast<QTMultiModel*>(masterModel)->GetElasticPtr());
//   inel->InitialiseLocal(p, static_cast<QTMultiModel*>(masterModel)->GetInelasticPtr());
// }
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double QTMultiModel::ComputeCrossSectionPerAtom(const G4ParticleDefinition* p,
						  G4double kinEnergy,
						  G4double Z,
						  G4double A,
						  G4double cutEnergy,
						  G4double maxEnergy)
{
  G4double cross = 0.0;
  //  for(G4int i=0; i<nModels; ++i) {
    //    (model[i])->SetCurrentCouple(CurrentCouple());
  cross += es->ComputeCrossSectionPerAtom(p, kinEnergy, Z, A, 
					  cutEnergy, maxEnergy);
  cross += inel->ComputeCrossSectionPerAtom(p, kinEnergy, Z, A, 
					    cutEnergy, maxEnergy);
    //  } 
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QTMultiModel::SampleSecondaries(std::vector<G4DynamicParticle*>* vdp,
				     const G4MaterialCutsCouple* couple,
				     const G4DynamicParticle* dp,
				     G4double minEnergy,
				     G4double maxEnergy)
{
  SetCurrentCouple(couple);
  //  if(nModels > 0) {
  //    G4int i;
  G4double cross = 0.0;
  //  for(i=0; i<nModels; ++i) {
  cross += es->CrossSection(couple, dp->GetParticleDefinition(), 
			    dp->GetKineticEnergy(), minEnergy, maxEnergy);
  cross_section[0] = cross;
  cross += inel->CrossSection(couple, dp->GetParticleDefinition(), 
			      dp->GetKineticEnergy(), minEnergy, maxEnergy);
  cross_section[1] = cross;
      //    }

  cross *= G4UniformRand();

  //    for(i=0; i<nModels; ++i) {
  if(cross <= cross_section[0]) {
    es->SampleSecondaries(vdp, couple, dp, minEnergy, maxEnergy);
    return;
  }
  else {
    inel->SampleSecondaries(vdp, couple, dp, minEnergy, maxEnergy);
    return;
  }
      //    }
      //  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

