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

#include "QTNMElasticModel.hh"

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


QTNMElasticModel::QTNMElasticModel()
: G4VEmModel("eElasticScattering"),
  fTheDCS(nullptr),
  fParticleChange(nullptr)
{
  SetLowEnergyLimit (  0.0*CLHEP::eV);  // ekin = 10 eV   is used if (E< 10  eV)
  SetHighEnergyLimit(100.0*CLHEP::MeV); // ekin = 100 MeV is used if (E>100 MeV)

  fTheDCS = new G4eDPWAElasticDCS(true, false); // init for electron
}


QTNMElasticModel::~QTNMElasticModel()
{
  if (IsMaster()) {
    delete fTheDCS;
  }
}


void QTNMElasticModel::Initialise(const G4ParticleDefinition* pdef,
                                               const G4DataVector& prodcuts)
{
  if(!fParticleChange) {
    fParticleChange = GetParticleChangeForGamma();
  }
  // init only for the elements that are used in the geometry
  G4ProductionCutsTable* theCpTable = G4ProductionCutsTable::GetProductionCutsTable();
  G4int numOfCouples = (G4int)theCpTable->GetTableSize();
  for(G4int j=0; j<numOfCouples; ++j) {
    const G4Material* mat = theCpTable->GetMaterialCutsCouple(j)->GetMaterial();
    const G4ElementVector* elV = mat->GetElementVector();
    std::size_t numOfElem = mat->GetNumberOfElements();
    for (std::size_t ie = 0; ie < numOfElem; ++ie) {
      fTheDCS->InitialiseForZ((*elV)[ie]->GetZasInt());
    }
  }

  // will make use of the cross sections so the above needs to be done before
  if(IsMaster()) {
    InitialiseElementSelectors(pdef, prodcuts);
  }
}


void QTNMElasticModel::InitialiseLocal(const G4ParticleDefinition*,
                                                    G4VEmModel* masterModel)
{
  SetElementSelectors(masterModel->GetElementSelectors());
}


G4double
QTNMElasticModel::ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
					     G4double ekin,
					     G4double Z,
					     G4double /*A*/,
					     G4double /*prodcut*/,
					     G4double /*emax*/)
{
  // Cross sections are computed by numerical integration of the pre-computed
  // DCS data between the muMin, muMax limits where mu(theta)=0.5[1-cos(theta)].
  // In case of single scattering model (i.e. when fMuMin=0): [muMin=0, muMax=1]
  // In case of mixed simulation model  (i.e. when fMuMin>0): [fMuMin , muMax=1]
  // NOTE: cross sections will be zero if the kinetic enrgy is out of the
  //       [10 eV-100 MeV] range for which DCS data has been computed.
  //
  G4double elCS  = 0.0;          // elastic cross section
  G4double tr1CS = 0.0;          // first transport cross section
  G4double tr2CS = 0.0;          // second transport cross section
  const G4double muMin = 0.0;
  const G4double muMax = 1.0;
  fTheDCS->ComputeCSPerAtom((G4int)Z, ekin, elCS, tr1CS, tr2CS, muMin, muMax);

  return std::max(0.0, elCS);
}


void
QTNMElasticModel::SampleSecondaries(std::vector<G4DynamicParticle*>*,
				    const G4MaterialCutsCouple* cp,
				    const G4DynamicParticle* dp,
				    G4double, G4double)
{
  const G4double    ekin   = dp->GetKineticEnergy();
  const G4double    lekin  = dp->GetLogKineticEnergy();
  const G4Element*  target = SelectTargetAtom(cp, dp->GetParticleDefinition(), ekin, lekin);
  const G4int       izet   = target->GetZasInt();
  // sample cosine of the polar scattering angle in (hard) elastic insteraction
  CLHEP::HepRandomEngine* rndmEngine = G4Random::getTheEngine();
  G4double cost = 1.0;
  G4double rndm[3];
  rndmEngine->flatArray(3, rndm);
  cost = fTheDCS->SampleCosineTheta(izet, lekin, rndm[0], rndm[1], rndm[2]);

  // compute the new direction in the scattering frame
  const G4double sint = std::sqrt((1.0-cost)*(1.0+cost));
  const G4double phi  = CLHEP::twopi*rndmEngine->flat();
  G4ThreeVector theNewDirection(sint*std::cos(phi), sint*std::sin(phi), cost);
  // get original direction in lab frame and rotate new direction to lab frame
  G4ThreeVector theOrgDirectionLab = dp->GetMomentumDirection();
  theNewDirection.rotateUz(theOrgDirectionLab);
  // set new direction
  fParticleChange->ProposeMomentumDirection(theNewDirection);
}

G4double QTNMElasticModel::MinPrimaryEnergy(const G4Material*,
					    const G4ParticleDefinition*,G4double) { return 10.0*CLHEP::eV; }
