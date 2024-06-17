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

#include "QTeDPWACoulombScatteringModel.hh"

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

QTeDPWACoulombScatteringModel::QTeDPWACoulombScatteringModel(G4bool ismixed, G4bool isscpcor, G4double mumin)
: G4VEmModel("eDPWACoulombScattering"),
  fIsMixedModel(ismixed),
  fIsScpCorrection(isscpcor),
  fMuMin(mumin),
  fTheDCS(nullptr),
  fParticleChange(nullptr)
{
  SetLowEnergyLimit (  0.0*CLHEP::eV);  // ekin = 10 eV   is used if (E< 10  eV)
  SetHighEnergyLimit(100.0*CLHEP::MeV); // ekin = 100 MeV is used if (E>100 MeV)
}


QTeDPWACoulombScatteringModel::~QTeDPWACoulombScatteringModel()
{
  if (IsMaster()) {
    delete fTheDCS;
  }
}


void QTeDPWACoulombScatteringModel::Initialise(const G4ParticleDefinition* pdef,
                                               const G4DataVector& prodcuts)
{
  if(!fParticleChange) {
    fParticleChange = GetParticleChangeForGamma();
  }
  fMuMin        = 0.5*(1.0-std::cos(PolarAngleLimit()));
  fIsMixedModel = (fMuMin > 0.0);
  if(IsMaster()) {
    // clean the G4eDPWAElasticDCS object if any
    delete fTheDCS;
    fTheDCS = new G4eDPWAElasticDCS(pdef==G4Electron::Electron(), fIsMixedModel);
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
    // init scattering power correction
    if (fIsScpCorrection) {
      fTheDCS->InitSCPCorrection(LowEnergyLimit(), HighEnergyLimit());
    }
    // will make use of the cross sections so the above needs to be done before
    InitialiseElementSelectors(pdef, prodcuts);
  }
}


void QTeDPWACoulombScatteringModel::InitialiseLocal(const G4ParticleDefinition*,
                                                    G4VEmModel* masterModel)
{
  SetElementSelectors(masterModel->GetElementSelectors());
  SetTheDCS(static_cast<QTeDPWACoulombScatteringModel*>(masterModel)->GetTheDCS());
}


G4double
QTeDPWACoulombScatteringModel::ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
                                                          G4double ekin,
                                                          G4double Z,
                                                          G4double /*A*/,
                                                          G4double /*prodcut*/,
                                                          G4double /*emax*/)
{
  G4double T_ev = ekin / CLHEP::eV;

  // These values should be set elsewhere
  const G4double bind = 13.6; // eV - binding energy
  const G4int el_no = 1; // Number of electrons
  G4double U = T_ev / bind;
  G4double J = 512375 / bind; // Assumes units of eV

  // MBell constants. Units of eV^2 cm^2
  const G4double mbell_a = 0.525e-13;
  const G4double mbell_b[7] = {-0.510e-13, 0.2000e-13, 0.0500e-13, -0.025e-13, -0.100e-13, 0.00e-13, 0.00e-13};
  const G4int mbell_m = 3;
  const G4double mbell_lambda[3] = {1.270, 0.542, 0.950};

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

  G4double sigma = (mbell_a * std::log(U) + bsum) / (bind * T_ev); // cm^2

  G4cout<< T_ev <<  ", " << sigma * f_ion * gr << G4endl;
  return sigma * f_ion * gr * CLHEP::cm * CLHEP:: cm;
}


void
QTeDPWACoulombScatteringModel::SampleSecondaries(std::vector<G4DynamicParticle*>*,
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
  if (!fIsMixedModel) {
    G4double rndm[3];
    rndmEngine->flatArray(3, rndm);
    cost = fTheDCS->SampleCosineTheta(izet, lekin, rndm[0], rndm[1], rndm[2]);
  } else {
    //sample cost between costMax,costMin where costMax = 1-2xfMuMin;
    const G4double costMax = 1.0-2.0*fMuMin;
    const G4double costMin = -1.0;
    G4double rndm[2];
    rndmEngine->flatArray(2, rndm);
    cost = fTheDCS->SampleCosineThetaRestricted(izet, lekin, rndm[0], rndm[1], costMin, costMax);
  }
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

G4double
QTeDPWACoulombScatteringModel::mbell_gr(G4double U, G4double J)
{
  G4double a = (1.0 + 2.0*J) / (U + 2.0*J);
  G4double b = pow( (U + J) / (1.0 + J), 2);
  G4double c = pow( (1+U)*(U+2.0*J)*pow(1+J,2) / (pow(J,2) * (1.0+2.0*J) + U*(U+2.0*J)* pow(1.0+J,2)) ,3);
  return a*b*std::sqrt(c);
}

G4double
QTeDPWACoulombScatteringModel::mbell_f_ion(G4int el_no, G4int nu, G4double U, G4double m_lambda)
{
  return 1 + 3*pow( (el_no - nu)/(U*el_no), m_lambda);
}





