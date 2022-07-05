#ifndef QTActionInitialization_h
#define QTActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

/// Action initialization class.

class QTActionInitialization : public G4VUserActionInitialization
{
public:
  QTActionInitialization(G4String name);
  virtual ~QTActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4String foutname;
};

#endif
