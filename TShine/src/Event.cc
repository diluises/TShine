/*
 * Event.cc
 *
 *  Created on: May, 2015
 *      Author: Silvestro di Luise
 *				Silvestro.Di.Luise@cern.ch
 *				
 */


#include <iostream>

#include <TROOT.h>
#include <TFolder.h>

#include "DataContainer.h"
#include "Event.h"

#include "MessageMgr.h"



//ClassImp(EDM::Event)



TFolder* EDM::Event::BuildFolder(TFolder *fld_top, TString name, TString title) {
/**
 *
 */


	TFolder* fld = (TFolder*)fld_top->FindObject(name);


	if ( fld!=NULL && fld->IsFolder() ){

		return fld;
	}


	fld = fld_top->AddFolder(name,title);


	fld->SetOwner();


	return fld;
}



namespace EDM {


Event::Event():
	/**
	 *
	 *
	 *
	 */

	fFldTop(0),
	fFldMC(0),
	fFldDet(0),
	fFldDetSim(0),
	fFldRaw(0),
	fTree(0),
	fFile(0)

	{



	//TFolder* rootFld = new TFolder("top","top");//gROOT->GetRootFolder();

	fFldTop = new TFolder("event_top","event top");//Event::BuildFolder(rootFld, "EDM", "Event Data");

	fFldTop->SetOwner();

	fFldMC    = Event::BuildFolder(fFldTop,"MC",  "MC");
	fFldDet   = Event::BuildFolder(fFldTop,"Det", "Det");
	fFldDetSim   = Event::BuildFolder(fFldTop,"DetSim", "DetSim");
	fFldRaw   = Event::BuildFolder(fFldTop,"Raw", "Raw");

}



Event::~Event() {
/**
 *
 */

	std::cout<<__FUNCTION__<<" in "<<std::endl;

	this->Clear();

	std::cout<<__FUNCTION__<<" out "<<std::endl;
}



void Event::Clear() {
	/**
	 *
	 */

	DataContainer dc(Top());

	dc.ClearArrays();

}




void Event::Init() {
	/**
	 *
	 */


}


void Event::Store(TString opt) {
	/**
	 *
	 */
	std::cout<<__PRETTY_FUNCTION__<<" "<<fTree->GetEntries()<<std::endl;


	if(!fFile){
		MSG::ERROR(__PRETTY_FUNCTION__," Output File not specified ");
		return;
	}

	if(!fTree){
		MSG::ERROR(__PRETTY_FUNCTION__," Output Tree not specified ");
		return;
	}


	fFile->cd();

	fTree->Fill();


	fTree->Print();

	EDM::DataContainer dc(this->DetSim());

	dc.Print();

	cout<<__PRETTY_FUNCTION__<<" end "<<endl;


}

void Event::SetStore(TTree *t, TFile *f) {
	/**
	 *
	 */

	fTree = dynamic_cast<TTree*>(t);
	fFile = dynamic_cast<TFile*>(f);



}


} /* namespace EDM */
