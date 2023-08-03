#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"

#include "G4THitsMap.hh"
#include "globals.hh"
#include "RunAction.hh"

namespace B4d
{

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Absober and Gap layers
/// stored in the hits collections.

class EventAction : public G4UserEventAction
{
public:
  EventAction(B4::RunAction* runAction);
  ~EventAction() override = default;

  void  BeginOfEventAction(const G4Event* event) override;
  void    EndOfEventAction(const G4Event* event) override;

  G4int GetCurrentEvNr(){return currentEv;}
  
private:

  // data members
  B4::RunAction* fRunAction;
  G4int currentEv;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
