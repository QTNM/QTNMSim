#include "QTGasHit.hh"

G4ThreadLocal G4Allocator<QTGasHit>* QTGasHitAllocator=0;

QTGasHit::QTGasHit()
 : G4VHit(),
   fTrackID(0),
   fEdep(0.),
   fTime(0.),
   fKine(0.),
   fPx(0.),
   fPy(0.),
   fPz(0.),
   fPosx(0.),
   fPosy(0.),
   fPosz(0.)
{}

QTGasHit::~QTGasHit() = default;


QTGasHit::QTGasHit(const QTGasHit& right)
  : G4VHit()
{
  fTrackID      = right.fTrackID;
  fEdep         = right.fEdep;
  fTime         = right.fTime;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;
}

const QTGasHit& QTGasHit::operator=(const QTGasHit& right)
{
  fTrackID      = right.fTrackID;
  fEdep         = right.fEdep;
  fTime         = right.fTime;
  fKine         = right.fKine;
  fPx           = right.fPx;
  fPy           = right.fPy;
  fPz           = right.fPz;
  fPosx         = right.fPosx;
  fPosy         = right.fPosy;
  fPosz         = right.fPosz;

  return *this;
}

G4bool QTGasHit::operator==(const QTGasHit& right) const
{
  return ( this == &right ) ? true : false;
}

void QTGasHit::Draw()
{ ; }

void QTGasHit::Print()
{ ; }

