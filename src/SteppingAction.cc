#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "TFile.h"
#include "TTree.h"
//#include "ROOT.h"

using namespace B4d;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(B4d::EventAction* eventAction, B4::PrimaryGeneratorAction* primGenAct)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fPrimaryGeneratorAction(primGenAct)
{
  //ROOT::EnableThreadSafety();
  fout=new TFile("o_polDetG4.root","RECREATE");
  t=new TTree("t","step output");
  t->Branch("evNr",&evNr,"evNr/I");
  t->Branch("uXsec",&uXsec,"uXsec/D");
  t->Branch("pXsec",&pXsec,"pXsec/D");
  t->Branch("pID",&pID,"pID/I");
  t->Branch("trID",&trID,"trID/I");
  t->Branch("parentID",&parentID,"parentID/I");
  //t->Branch("copyID",&copyID,"copyID/I");
  t->Branch("volID",&volID,"volID/I");
  t->Branch("x",&x,"x/D");
  t->Branch("y",&y,"y/D");
  t->Branch("z",&z,"z/D");
  // t->Branch("px",&px,"px/D");
  // t->Branch("py",&py,"py/D");
  t->Branch("pz",&pz,"pz/D");
  t->Branch("kE",&kE,"kE/D");
  t->Branch("eDep",&eDep,"eDep/D");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{
  t->Write();
  fout->Close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  
  G4StepPoint* prePoint   = step->GetPreStepPoint();
  G4VPhysicalVolume* prePV= prePoint->GetPhysicalVolume();

  if (!prePV) return;
  //G4cout<<"name "<<prePV->GetName()<<" "<<prePoint->GetPosition().getZ()<<G4endl;

  volID=-1;
  if ( G4StrUtil::contains(prePV->GetName(),"initial") )   volID=0;
  else if ( G4StrUtil::contains(prePV->GetName(),"Abso") ) volID=1;
  else if ( G4StrUtil::contains(prePV->GetName(),"Gap") )  volID=2;

  //G4cout<<volID<<G4endl;
  if (volID<0) return;
  if(volID==2 && (abs(pID)!=11)) return;
  
  pz = prePoint->GetMomentum().getZ();
  parentID = step->GetTrack()->GetParentID();
  if (volID==0 && (pz<0 || parentID !=0) ) return;
  
  //if( parentID !=0 ) return;

  evNr = fEventAction->GetCurrentEvNr();
  uXsec = fPrimaryGeneratorAction->GetUxsec();
  pXsec = fPrimaryGeneratorAction->GetPxsec();
  pID = step->GetTrack()->GetDefinition()->GetPDGEncoding();
  trID = step->GetTrack()->GetTrackID();
  //copyID = prePoint->GetTouchable()->GetVolume()->GetCopyNo();
  x = prePoint->GetPosition().getX();
  y = prePoint->GetPosition().getY();
  z = prePoint->GetPosition().getZ();
  // px = prePoint->GetMomentum().getX();
  // py = prePoint->GetMomentum().getY();
  kE = prePoint->GetKineticEnergy();
  eDep = step->GetTotalEnergyDeposit();
  //G4cout<<"asdf "<<prePV->GetName()<<" >> "<<x<<G4endl;
  t->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

