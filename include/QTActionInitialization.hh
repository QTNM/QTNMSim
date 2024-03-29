#ifndef QTActionInitialization_h
#define QTActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "globals.hh"

# include <vector>
/// Action initialization class.

class QTActionInitialization : public G4VUserActionInitialization
{
public:
  QTActionInitialization(G4String, std::vector<G4double>);
  ~QTActionInitialization() override;

  virtual void BuildForMaster() const override;
  virtual void Build() const override;

private:
  std::vector<G4double> angles;
  G4String              foutname;
};

#endif
