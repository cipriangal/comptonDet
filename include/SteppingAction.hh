#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

class G4LogicalVolume;
class TTree;
class TFile;

/// Stepping action class
/// 
class SteppingAction : public G4UserSteppingAction{
public:
  SteppingAction(B4d::EventAction* eventAction,B4::PrimaryGeneratorAction* primGenAct);
  ~SteppingAction();
  
  // method from the base class
  virtual void UserSteppingAction(const G4Step*);
  
private:
  B4d::EventAction*  fEventAction;
  B4::PrimaryGeneratorAction *fPrimaryGeneratorAction;
  G4LogicalVolume* fScoringVolume;

  TTree *t;
  TFile *fout;
  G4int evNr;
  G4double uXsec,pXsec;
  G4int pID, trID, parentID, copyID, volID;
  G4double x,y,z;
  G4double px,py,pz;
  G4double kE,eDep;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
