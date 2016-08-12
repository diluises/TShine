/*
 * TSDataSetMgr.cc
 *
 *  Created on: Jun 7, 2014
 *      Author: Silvestro di Luise
 *      		Silvestro.Di.Luise@cern.ch
 */


#include <iostream>

#include "MessageMgr.h"

#include "RootUtils.h"
#include "StringUtils.h"

#include "TSDataSetMgr.h"



using std::cout;
using std::endl;



TSDataSetMgr::TSDataSetMgr(TString name, TString tag, TString title)
:TSNamed(name,tag,title)
{

	Init();

	SetTag(tag);
}


TSDataSetMgr::~TSDataSetMgr() {


	// delete Map as it is local
	delete fPhaseSpaceMap;



	for(std::map<int,fVarSettings*>::iterator it=fVarSetsList.begin(), end=fVarSetsList.end();
		   it!=end;
		   ++it){

			delete it->second;
	}



	for(std::map<int,TSDataBin*>::iterator it=fDataBinList.begin(), end=fDataBinList.end();
	it!=end;
	++it){

		delete it->second;
	}



}



void TSDataSetMgr::SetBinning(const TSVariable& var_binned,const TSVariable& var_space, int Nbins, TGraph& glow,TGraph& gup){

  /**
   
   
     


   */



  if( !HasPhaseSpace() ){
    
    MSG::WARNING(__FILE__,"::",__FUNCTION__," No Phase Space set for DataSet: ",GetName());
    return;
  }


  
  int var_space_axis = -1;


  for(int ia=1; ia<=fPhaseSpaceMap->GetDimension();++ia){

    const TSVariable *var = fPhaseSpaceMap->GetVariable(ia);

    if( var == &var_space ){
      
      var_space_axis = ia;
      break;
    }
    
  }

  if( var_space_axis == -1 ){

    MSG::ERROR(__FILE__,"::",__FUNCTION__," DataSet Phase Space has no variable: ",var_space.GetName());
    return;

  }

  if( glow.GetN()==0 ){
    MSG::ERROR(__FILE__,"::",__FUNCTION__," Lower Bounds Graph is empty");
    return;
  }


  if( gup.GetN()==0 ){
    MSG::ERROR(__FILE__,"::",__FUNCTION__," Upper Bounds Graph is empty");
    return;
  }

  
  TString var_binned_name = var_binned.GetName();
  

  double var_space_low_min = TMath::MinElement(glow.GetN(),glow.GetX());
  double var_space_low_max = TMath::MaxElement(glow.GetN(),glow.GetX());

  double var_space_up_min = TMath::MinElement(gup.GetN(),gup.GetX());
  double var_space_up_max = TMath::MaxElement(gup.GetN(),gup.GetX());
  


  //
  //-- loop over phase space volumes
  //


  for( int vol=1; vol<= fPhaseSpaceMap->GetNumOfVolumes(); ++vol){



    TSPhaseSpaceVolume *volume = fPhaseSpaceMap->GetVolume(vol);

    double var_space_center = volume->GetCenter(var_space_axis);

    
    double xval;

    xval = TMath::Min(var_space_center,var_space_low_max); 
    xval = TMath::Max(xval,var_space_low_min); 

    double min_new = glow.Eval(xval);


    xval = TMath::Min(var_space_center,var_space_up_max); 
    xval = TMath::Max(xval,var_space_up_min); 

    double max_new = gup.Eval(xval);
    
    

    if(!fPhaseSpaceMap->VolumeIsSelected(vol) ){

      //cout<<"not sel range "<<vol<<"; "<<var_space_center<<"; "<<var_space_up_min<<" "<<var_space_up_max<<"; "<<xval<<"; "<<min_new<<" "<<max_new<<endl;

      continue;
    }

    

    TSDataBin* vol_data_bin = GetDataBin(vol);

    

    TSHistogramFld* hFld = vol_data_bin->GetHistoFolder();
    
    for(int ih=1; ih<=hFld->GetSize();++ih){
    
      //cout<<ih<<" "<<hFld->GetHistoName(ih)<<" x: "<<hFld->GetHistoNameX(ih)<<" y: "<<hFld->GetHistoNameY(ih)<<endl;

      TString var_x = hFld->GetHistoNameX(ih);
      TString var_y = hFld->GetHistoNameY(ih);
      
      TObject *obj = hFld->GetObject(ih);

      TH1* h = ((TH1*)obj);

      if( hFld->IsTH1(*obj) && var_x == var_binned_name ){


	if(Nbins<=0) Nbins = ((TH1*)obj)->GetNbinsX();
 
	cout<<"rebinning x of "<<obj->GetName()<<" -old- : "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	  <<" -new-: "<<Nbins<<" "<<min_new<<" "<<max_new<<endl;

	h->SetBins(Nbins,min_new,max_new);
	

      }


      if( hFld->IsTH2(*obj) && var_x == var_binned_name ){


	if(Nbins<=0) Nbins = ((TH2*)obj)->GetNbinsX();

	cout<<"rebinning x of "<<obj->GetName()
	    <<" -old- x:"<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	    <<" y: "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	    <<" -new- x:"<<Nbins<<" "<<min_new<<" "<<max_new
	    <<" y: "<<h->GetNbinsY()<<" "<<h->GetYaxis()->GetXmin()<<" "<<h->GetYaxis()->GetXmax()
	    <<endl;
	
	if(h->GetYaxis()->GetXbins()->GetArray()){
	  h->SetBins(Nbins,RootUtils::CreateBinsArray(Nbins,min_new,max_new), h->GetNbinsY(),h->GetYaxis()->GetXbins()->GetArray() );
	}else{
	  h->SetBins(Nbins,min_new,max_new,h->GetNbinsY(),h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
	}

	

      }


      if( hFld->IsTH2(*obj) && var_y == var_binned_name ){


	if(Nbins<=0) Nbins = ((TH2*)obj)->GetNbinsY();

	cout<<"rebinning y of "<<obj->GetName()
	    <<" -old- x:"<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	    <<" y: "<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	    <<" -new- x:"<<h->GetNbinsX()<<" "<<h->GetXaxis()->GetXmin()<<" "<<h->GetXaxis()->GetXmax()
	    <<" y: "<<Nbins<<" "<<min_new<<" "<<max_new
	    <<endl;
       

	if(h->GetXaxis()->GetXbins()->GetArray()){
	  h->SetBins(h->GetNbinsX(),h->GetXaxis()->GetXbins()->GetArray(),Nbins,RootUtils::CreateBinsArray(Nbins,min_new,max_new));
	}else{
	  h->SetBins(h->GetNbinsX(),h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax(), Nbins,min_new,max_new );
	}


      }



      

    }
    
    

    cout<<" "<<endl;
  
  }//volumes
  




}


int TSDataSetMgr::Fill(const TSVariable &v){


	int vol=0;

	if( HasPhaseSpace() ){

		vol = fPhaseSpaceMap->FindVolume();

		if( !vol ){
			return vol;
		}


		// is it needed here?
		fPhaseSpaceFld->Fill();


		if( GetDataBin(vol) ){

			GetDataBin(vol)->Fill(v);

		}else{
			MSG::ERROR(__FILE__,"::",__FUNCTION__," request to fill DataBin for volume number: ",vol," bin not found");
		}

	}


	return vol;


}

int TSDataSetMgr::Fill(const TSArgList &list){


	int vol=0;

	if( HasPhaseSpace() ){

		vol = fPhaseSpaceMap->FindVolume();

		if( !vol ){
			return vol;
		}


		// is it needed here?
		fPhaseSpaceFld->Fill();


		if( GetDataBin(vol) ){

			GetDataBin(vol)->Fill(list);

		}else{
			MSG::ERROR(__FILE__,"::",__FUNCTION__," request to fill DataBin for volume number: ",vol," bin not found");
		}

	}


	return vol;


}


int TSDataSetMgr::Fill() {


	int vol=0;

	if( HasPhaseSpace() ){

		vol = fPhaseSpaceMap->FindVolume();


		if( !vol ){
			return vol;
		}

		//
		fPhaseSpaceFld->Fill();


		if( GetDataBin(vol) ){


			GetDataBin(vol)->Fill();
		}else{
			MSG::ERROR(__FILE__,"::",__FUNCTION__," request to fill DataBin for volume number: ",vol," bin not found");
		}


	}


    return vol;




}



int TSDataSetMgr::FillHistosFromFile(TFile *file, TString opt){


	if( !file ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," null ptr to file");
		return 0;
	}


	for(int i=1; i<=GetNumOfDataBins(); ++i){

		GetDataBin(i)->GetHistoFolder()->FillHistosFromFile(file,opt);
	}



	return 1;
}

TSDataBin* TSDataSetMgr::GetDataBin(int bin) const{

	if( fDataBinList.count(bin)==1 ){

		return fDataBinList.find(bin)->second;

	}else{

		return 0;
	}


}



void TSDataSetMgr::Init() {

	fNumOfDataBins=0;
	fNumOfVars=0;

	fHasDataBin = false;
	fHasPhaseSpace = false;

	SetTag("");

	fPhaseSpaceMap = 0;
	fPhaseSpaceFld = 0;

}



bool TSDataSetMgr::SetDataBin(TSDataBin& DataBin) {

	/**
	 *
	 *  Setup the prototype of the Data Bin Folder
	 *
	 *  Tag, Label?
	 *
	 *  Histogram Renaming
	 *
	 *  If a PhaseSpace (PS) Map is set to the DataMgr then
	 *  several DataBin will be created, one for each, PS Volume.
	 *  Is the case the DataBin-prototype has an internal PS Volume
	 *  already set that will be overridden.
	 *  -->At the moment this option is not implemented..the data bin must be provided
	 *  whitout PSV
	 *
	 *
	 *
	 */

	cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	if( DataBin.HasPhaseSpaceVolume() && HasPhaseSpace() ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__,", data bin: ", DataBin.Name(),
					" associated PhaseSpaceVolume conflicts with local PhaseSpaceMap");
		return 0;
	}


	//
	// Extract variables
	//


	for(int ivar=1; ivar <= DataBin.GetNumOfVars(); ++ivar){

		const TSVariable *var = DataBin.GetVariable(ivar);

		//opt == 1 data variable (not phase space)
		fSetVariable(*var,1);

	}


	//
	// Copy the Data Bin Prototype structure
	// to the local Data Bin
	//

	fNumOfDataBins++;

	TSDataBin *data_bin= new TSDataBin( DataBin );

	TString name="DB_"; name+= Name();
	name+="_"; name+=fNumOfDataBins;

	data_bin->SetName(name);


    //
    //what about tag?
    //

    //? what is the phase space?
    fDataBinList[fNumOfDataBins] = data_bin;

    cout<<__FILE__<<"::"<<__FUNCTION__<<" -- out --"<<endl;

    return 1;

}




bool TSDataSetMgr::SetPhaseSpace(TSPhaseSpaceMap& map) {

	/**
	 *
	 * Import the phase space map
	 *
	 *
	 *
	 */

	//cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	if( HasPhaseSpace() ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," PhaseSpaceMap already Set for: ", Name());
		return 0;
	}


	if( map.GetDimension()==0 || map.GetNumOfVolumes()==0 ){

		MSG::ERROR(__FILE__,"::",__FUNCTION__," Illegal PhaseSpaceMap:  ", map.Name());
		if(map.GetDimension()==0)     MSG::ERROR(" Map Dimension is zero ");
		if(map.GetNumOfVolumes()==0)  MSG::ERROR(" Map Number of volumes is zero ");
		

		fHasPhaseSpace = false;

		return 0;
	}


	bool status = 1;


	fHasPhaseSpace = true;


	//
	// Extract variables
	//


	for(int ivar=1; ivar<= map.GetDimension(); ++ivar){

		const TSVariable *var = map.GetVariable(ivar);

		// opt == 2 phase space variable

		fSetVariable( *var, 2);
	}


	//
	// Copy PhaseSpace Structure to local Object
	// Shall we just assign the pointer instead???

	fPhaseSpaceMap = new TSPhaseSpaceMap( map );

	TString map_name = "PSMap_"; map_name+=GetName();
	fPhaseSpaceMap->SetName(map_name);

	//
	// Build a Phase Space Folder
	//

	//TODO: naming?
	fPhaseSpaceFld = new TSPhaseSpaceFld(GetName(),GetTag(),GetTitle(),*fPhaseSpaceMap);


    //
    // DataBins
    //
    status = fBuildDataBins();


    //cout<<__FILE__<<"::"<<__FUNCTION__<<" -- out -- "<<endl;

	return status;

}


void TSDataSetMgr::SetTag(TString tag){

	fTag=tag;
	if( fTag.IsWhitespace() ){
		fHasTag=false;
	}
	else fHasTag=true;

}




void TSDataSetMgr::Print(TString opt) const{


	cout<<" == TSDataSetMgr::Print() "<<Name()<<" "<<GetTag()<<" "<<Title()<<" ==== "<<endl;

	cout<<" Num. of Vars: "<<GetNumOfVars()<<endl;

	PrintVariables();

	cout<<" Num. of Data Bins: "<<GetNumOfDataBins()<<endl;

	cout<<" ===== "<<Name()<<" ==== "<<endl;

}


void TSDataSetMgr::PrintVariables(TString opt) const {


	int n=0;

	for(std::map<int,const TSVariable*>::const_iterator it=fVarList.begin(), end=fVarList.end()
			;it!=end;++it ){
		n++;

		TString type= fVarIsPhaseSpace(n) ? "phase-space": "data";

		cout<<n<<" "<<it->second->GetTitleXY("")<<" type: "<< type <<endl;


	}



}






bool TSDataSetMgr::fBuildDataBins() {

	/**
	 *
	 *
	 *
	 *  Propagate DataBins Over PhaseSpace Volumes
	 *
	 *
	 *
	 *
	 */

	cout<<__FILE__<<"::"<<__FUNCTION__<<endl;

	if( !HasPhaseSpace() ){
		return 0;
	}


	if( GetNumOfDataBins() > 1){

		MSG::ERROR(__FILE__,"::",__FUNCTION__,"; In ",Name(),
				" More than one DataBin found, cannot loop over PhaseSpaceMap");
		return 0;

	}


	if( GetNumOfDataBins() ==1 ){

		TSDataBin *data_bin = GetDataBin(1);

		if( ! data_bin->HasPhaseSpaceVolume() ){

			//Propagate DataBins Over PhaseSpace Volumes
			for( int vol=2; vol<= fPhaseSpaceMap->GetNumOfVolumes(); ++vol){

			  //cout<<"copying bin of vol: "<<vol<<endl;
				TSDataBin* vol_data_bin = new TSDataBin(*data_bin);

				fNumOfDataBins++;

				fDataBinList[fNumOfDataBins] = vol_data_bin;
			}

			//Associate PhaseSpaceVolume
			for( int vol=1; vol<= fPhaseSpaceMap->GetNumOfVolumes(); ++vol){

				TSPhaseSpaceVolume *volume = fPhaseSpaceMap->GetVolume(vol);

				TSDataBin* vol_data_bin = GetDataBin(vol);

				TString bin_name="DB_"; bin_name+=Name(); bin_name+="_";
				bin_name+= vol;

				//SetName will also rename the histos including bin_name in the histo name
				vol_data_bin->SetName(bin_name);

				vol_data_bin->SetPhaseSpaceVolume( *volume );
			}


		}else{
			MSG::ERROR(__FILE__,"::",__FUNCTION__,", data bin: ", data_bin->Name(),
	    							" associated PhaseSpaceVolume conflicts with local PhaseSpaceMap");
			return 0;
		}

	}


	//cout<<__FILE__<<"::"<<__FUNCTION__<<" -- out -- "<<endl;

	return 1;

}



bool TSDataSetMgr::fSetVariable(const TSVariable& var, int opt) {

/**
 *
 *  opt==1
 *  Is a Data Variable
 *
 *  opt==2
 *  Is a Phase Space Variable
 *
 *
 */


	for(int i=1; i<=fNumOfVars; ++i){

		if( fVarList[i] == &var ){
			//MSG::LOG(__FILE__,"::",__FUNCTION__," variable: ",var.GetName()," already present: not added");
			return 0;
		}
	}




	fNumOfVars++;

	fVarList[fNumOfVars] = &var;


    fVarSettings *fSet= new fVarSettings();

    if(opt==1){
      fSet->IsData=true;
      fSet->IsPhaseSpace=false;
    }

    if(opt==2){
       fSet->IsData=false;
       fSet->IsPhaseSpace=true;
    }

    fVarSetsList[fNumOfVars] = fSet;

    //cout<<"DB: "<<var.Name()<<" "<<fSet->IsData<<" "<<fSet->IsPhaseSpace<<endl;


}



bool TSDataSetMgr::fVarIsPhaseSpace(int i) const{


	if( fVarSetsList.count(i) ){

		return fVarSetsList.find(i)->second->IsPhaseSpace;

	}

	return 0;
}



bool TSDataSetMgr::fVarIsData(int i) const{


	if( fVarSetsList.count(i) ){

		return fVarSetsList.find(i)->second->IsData;

	}

	return 0;


}
