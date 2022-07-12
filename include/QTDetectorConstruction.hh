/// Definition of the QTDetectorConstruction class
//
//
//
//

#ifndef _QTDETECTORCONSTRUCTION_H_
#define _QTDETECTORCONSTRUCTION_H_

#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"

/// Detector construction allowing to use the geometry read from the GDML file
class EGGasSD;
class G4VPhysicalVolume;

class QTDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  QTDetectorConstruction(G4VPhysicalVolume *setWorld = 0)
  {   
    fWorld = setWorld;
  }
  
  virtual G4VPhysicalVolume* Construct()
  {
    return fWorld;
  }
  virtual void               ConstructSDandField();
  
  
private:
  
  G4VPhysicalVolume *fWorld;
  G4Cache<QTGasSD*>  fSD = nullptr;
};
#endif
