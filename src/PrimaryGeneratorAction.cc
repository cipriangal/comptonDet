#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

// #include "TTree.h"
// #include "TFile.h"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  auto particleDefinition
    = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  //    = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  //fParticleGun->SetParticleMomentum(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(1000.*MeV);

  fin.open("o_photons.txt", std::ifstream::in);
  /*
  fin = TFile::Open("o_e18Pz59Pxm81_ip6V4_noSync_smearXY_1e6v5.root","READ");
  tin = (TTree*)fin->Get("t");
  tin->SetBranchAddress("q03US_hits",&detHits);
  tin->SetBranchAddress("uXsec",&uXsec);
  tin->SetBranchAddress("pXsec",&pXsec);
  */
  currentEvent=0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  fin.close();
  //fin->Close();
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }

  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }


  G4double x(0),y(0),px(0),py(0),pz(0);
  std::string line;
  std::getline(fin,line);
  std::stringstream data(line);
  data>>x>>y>>px>>py>>pz>>pXsec>>uXsec;
  
  // G4bool eventPass = true;
  // do{
  //   tin->GetEntry(currentEvent);
  //   for(int j=0;j<detHits->size();j++){
  //     if(detHits->at(j).pID==22){
  // 	x = detHits->at(j).x - 880;
  // 	y = detHits->at(j).y;
  // 	kE = detHits->at(j).ke;
  //     }
  //   }
  //   currentEvent++;
  //   if(abs(x)<=10*cm && abs(y)<=10*cm)
  //     eventPass = false;
  // }while(eventPass);
  G4ThreeVector p(px,py,-pz);
  // G4cout<<"asdf "<<currentEvent<<" "<<x<<" "<<y<<" "<<pz<<G4endl;
  // G4cout<<" world "<<worldZHalfLength<<G4endl;
  // G4cout<<"\t"<<p.mag()<<" "<<p.unit()<<G4endl;
  fParticleGun->SetParticleMomentumDirection(p.unit());
  fParticleGun->SetParticleEnergy(p.mag());

  //fParticleGun->SetParticleMomentum(G4ThreeVector(px,py,-pz));

  // Set gun position
  fParticleGun
    ->SetParticlePosition(G4ThreeVector(x - 879.2, y, -worldZHalfLength));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}


