#ifndef SCActionInitialization_h
#define SCActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "globals.hh"

# include <vector>
/// Action initialization class.

class SCActionInitialization : public G4VUserActionInitialization
{
public:
  SCActionInitialization(G4String);
  ~SCActionInitialization() override;

  virtual void BuildForMaster() const override;
  virtual void Build() const override;

private:
  G4String              foutname;
};

#endif
