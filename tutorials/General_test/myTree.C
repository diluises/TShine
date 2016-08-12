// myTree4.C
// S. Stave
// July, 2009
//
// --------------------------------------------------
{

  TCanvas *c1 = new TCanvas("c1","c1",50,50,800,600);
  c1->SetFillColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  
  TChain *nt = new TChain("tree");
  
  nt->Add("myNtuple.root");
  
  Float_t nt_x, nt_y, nt_z;
  
  nt->SetBranchAddress("x",&nt_x);
  nt->SetBranchAddress("y",&nt_y);
  nt->SetBranchAddress("z",&nt_z);
  
  TH3F *h3 = new TH3F("h3","h3",
		      20,-10,10,
		      20,-10,10,
		      20,-10,10);

  TH3F *h3_cut = new TH3F("h3_cut","Cut h3",
			  20,-10,10,
			  20,-10,10,
			  20,-10,10);
  TH3F *h3_cut2 = new TH3F("h3_cut","Cut h3",
			  20,-10,10,
			  20,-10,10,
			  20,-10,10);

  h3->SetMarkerStyle(8);
  h3_cut->SetMarkerStyle(8);
  h3_cut2->SetMarkerStyle(8);
  h3->SetMarkerSize(0.5);
  h3_cut->SetMarkerSize(0.5);
  h3_cut2->SetMarkerSize(0.5);

  h3_cut->SetMarkerColor(2);
  h3_cut2->SetMarkerColor(3);
  

  Int_t nentries,nbytes;
  nentries=(Int_t)nt->GetEntries();
  Int_t i;
  
  Double_t R;
  
  for (i = 0;i<nentries;i++) {
    nbytes = nt->GetEntry(i);

    R=sqrt(pow(nt_x,2)+pow(nt_y,2)+pow(nt_z,2));

    //Uncut histogram
    h3->Fill(nt_x,nt_y,nt_z);

    //Cut histogram
    if (R<5){
      h3_cut->Fill(nt_x,nt_y,nt_z);
    }
    
    if (nt_x>0&&nt_y>0&&nt_z>0){
      h3_cut2->Fill(nt_x,nt_y,nt_z);
    }      

  
  }

  TLegend *leg = new TLegend(0.8,0.8,0.98,0.98);
  leg->AddEntry(h3,"Uncut","P");
  leg->AddEntry(h3_cut,"Cut","P");
  leg->AddEntry(h3_cut2,"Cut2","P");

  c1->cd();
  h3->Draw();
  h3_cut->Draw("same");
  h3_cut2->Draw("same");
  leg->Draw();

}
