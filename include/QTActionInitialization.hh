#ifndef QTActionInitialization_h
#define QTActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

# include <vector>
/// Action initialization class.

class QTActionInitialization : public G4VUserActionInitialization
{
public:
  QTActionInitialization(G4String, G4int, std::vector<G4ThreeVector>);
  virtual ~QTActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  std::vector<G4ThreeVector> positions;
  G4String      foutname;
  G4int         nAntenna;
};

#endif
