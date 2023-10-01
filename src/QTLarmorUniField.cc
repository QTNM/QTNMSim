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
// from G4UniformElectricField implementation
//
// Created: V.Grichine, 30.01.1997
// -------------------------------------------------------------------

#include "QTLarmorUniField.hh"
#include "G4PhysicalConstants.hh"

QTLarmorUniField::
QTLarmorUniField(const G4ThreeVector& FieldVector)
{
  fFieldComponents[0] = FieldVector.x();
  fFieldComponents[1] = FieldVector.y();
  fFieldComponents[2] = FieldVector.z();
  fFieldComponents[3] = 0.0;
  fFieldComponents[4] = 0.0;
  fFieldComponents[5] = 0.0;
}

QTLarmorUniField::~QTLarmorUniField() = default;

QTLarmorUniField::
QTLarmorUniField (const QTLarmorUniField& p)
   : QTLarmorEMField(p)
{
   for (auto i=0; i<6; ++i)
   {
     fFieldComponents[i] = p.fFieldComponents[i];
   }
}

QTLarmorUniField&
QTLarmorUniField::operator = (const QTLarmorUniField& p)
{
  if (&p == this) { return *this; }
  QTLarmorEMField::operator=(p); 
  for (auto i=0; i<6; ++i)
  {
    fFieldComponents[i] = p.fFieldComponents[i];
  }
  return *this;
}

G4Field* QTLarmorUniField::Clone() const
{
   return new QTLarmorUniField( G4ThreeVector(fFieldComponents[0],
                                              fFieldComponents[1],
                                              fFieldComponents[2]));
}

// ------------------------------------------------------------------------

void QTLarmorUniField::SetFieldValue (const G4ThreeVector& newField)
{
  fFieldComponents[0] = newField.x();
  fFieldComponents[1] = newField.y();
  fFieldComponents[2] = newField.z();
}


void QTLarmorUniField::GetFieldValue (const G4double[4],
                                      G4double* fieldBandE) const 
{
   fieldBandE[0] = fFieldComponents[0];
   fieldBandE[1] = fFieldComponents[1];
   fieldBandE[2] = fFieldComponents[2];
   fieldBandE[3] = 0.0;
   fieldBandE[4] = 0.0;
   fieldBandE[5] = 0.0;
}
