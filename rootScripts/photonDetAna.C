void photonDetAna(){
  TFile *fin=new TFile("../o_polDetG4.root","READ");
  TTree* t=(TTree*)fin->Get("t");

  int evNr, pID, tID,parentID, volID;
  double uXsec, pXsec, x,y,z,pz,kE,eDep;
  t->SetBranchAddress("evNr",&evNr);
  t->SetBranchAddress("pID",&pID);
  t->SetBranchAddress("trID",&tID);
  //t->SetBranchAddress("parentID",&parentID);
  t->SetBranchAddress("volID",&volID);
  //t->SetBranchAddress("x",&x);
  t->SetBranchAddress("y",&y);
  t->SetBranchAddress("z",&z);
  t->SetBranchAddress("pz",&pz);
  t->SetBranchAddress("kE",&kE);
  t->SetBranchAddress("eDep",&eDep);
  t->SetBranchAddress("uXsec",&uXsec);
  t->SetBranchAddress("pXsec",&pXsec);

  TFile *fout=new TFile("o_photonAna.root","RECREATE");
  TH2F *eReco2D = new TH2F("eReco2D","energy [GeV];reconstructed (sum Edep); truth",40,0,8,40,0,8);
  TH1F *eReco = new TH1F("eReco","energy [GeV];reconstructed (sum Edep); rate",40,0,8);
  TH1F *eTruth = new TH1F("eTruth","energy [GeV];truth; rate",40,0,8);

  TH1F *posRes = new TH1F("posRes","position residual;y_{reco}-y_{truth}",101,-0.2,0.2);

  TH1F *yTruth = new TH1F("yTruth",";truth vertical position [cm]",
			  601,-1,1);
  
  std::vector<TH1F*> posDet, posReco, avgPos;
  for(int i=0;i<20;i++){
    posDet.push_back(new TH1F(Form("posDet%d",i),Form("layer %d;vertical position [cm]",i),
			      601,-1,1));
    posReco.push_back(new TH1F(Form("posReco%d",i),Form("layer %d;vertical position [cm]",i),
			       601,-1,1));
    avgPos.push_back(new TH1F(Form("avgPos%d",i),Form("layer %d;vertical position [cm]",i),
			       201,-1,1));
  }

  double totEdep(0),iniE(-1),iniy(0);
  int currentEv(-1);

  long nEntries = t->GetEntries();
  cout<<"\tTotal events: "<<nEntries<<endl;
  float currentProc=1,procStep=10;

  for(long i=0;i<nEntries;i++){
    t->GetEntry(i);
    if( float(i+1)/nEntries*100 > currentProc){
      cout<<"at tree entry\t"<<i<<"\t"<< float(i+1)/nEntries*100<<endl;
      currentProc+=procStep;
    }
    //if(currentProc>12) break;
    
    /* if(i>109095) */
    /*   cout<<evNr<<" "<<currentEv<<" "<<z<<" "<<volID<<" "<<totEdep<<" "<<iniE<<endl; */

    if(currentEv!=evNr){
      if(iniE<0 || totEdep<=0){
	cout<<"initial energy missing "<<currentEv<<" "<<iniE<<" "<<totEdep<<endl;
	currentEv=evNr;
	if(volID==0 && tID==1 && pz>0){
	  iniE=kE/1000;
	  iniy=y/10;
	}
	continue;
      }
      eTruth->Fill(iniE,uXsec);
      yTruth->Fill(iniy,uXsec);
      eReco->Fill(totEdep,uXsec);
      eReco2D->Fill(totEdep,iniE,uXsec);
      totEdep = 0;
      iniE = -1;

      for(int j=0;j<20;j++){
	if(avgPos[j]->GetEntries()>1 && abs(avgPos[j]->GetMean())>0){
	  if(avgPos[j]->GetMean()==0){
	    cout<<j<<" "<<avgPos[j]->GetEntries()<<" "<<z<<" "<<currentEv<<endl;
	  }
	  posReco[j]->Fill(avgPos[j]->GetMean(),uXsec);
	  avgPos[j]->Reset();
	}
      }

      if(volID==0 && tID==1 && pz>0){
	iniE=kE/1000;
	iniy=y/10;
      }

      currentEv=evNr;

    }else{
      if(volID==0 && tID==1 && pz>0){
	iniE=kE/1000;
	iniy=y/10;
      }
      else if(volID==1)
	totEdep += eDep/1000;
      else{
	int layer = int(z+100)/10;
	//cout<<"asdf "<<layer<<endl;
	if(layer>=20 || layer<0) continue;

	if(abs(pID)==11){
	  avgPos[layer]->Fill(y/10);
	  posDet[layer]->Fill(y/10);
	}
      }
    }
    /* if(i>109095) */
    /*   cout<<"\t"<<currentEv<<" "<<z<<" "<<volID<<" "<<totEdep<<" "<<iniE<<endl; */
    
  }

  fout->cd();
  eReco2D->Write();
  eReco->Write();
  eTruth->Write();
  yTruth->Write();
  for(int i=0;i<20;i++){
    posDet[i]->Write();
    posReco[i]->Write();
  }
  
  fin->Close();
}
