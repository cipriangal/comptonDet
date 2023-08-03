#ifndef B4PrimaryGeneratorAction_h
#define B4PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <fstream>

class G4ParticleGun;
class G4Event;

/*
class TTree;
class TFile;
struct genHit{
  G4int pID;
  G4int trackID;
  G4int parentID;
  G4double x,y,z;
  G4double px,py,pz;
  G4double e,ke;
};
*/

namespace B4
{

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the calorimeter
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;
  G4double GetUxsec(){return uXsec;}
  G4double GetPxsec(){return pXsec;}
  
  // set methods
  void SetRandomFlag(G4bool value);

private:
  G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
  std::ifstream fin;
  // TTree *tin = nullptr;
  // TFile *fin = nullptr;
  // std::vector<genHit>* detHits = nullptr;
  G4double uXsec, pXsec;
  G4int currentEvent;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
