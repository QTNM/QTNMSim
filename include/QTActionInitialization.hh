#ifndef QTActionInitialization_h
#define QTActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "globals.hh"

# include <vector>
/// Action initialization class.

class QTActionInitialization : public G4VUserActionInitialization
{
public:
  QTActionInitialization(G4String);
  ~QTActionInitialization() override;

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:

  G4String              foutname;
};

#endif
