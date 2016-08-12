/*
 * Event.h
 *
 *  Created on: May, 2015
 *      Author: Silvestro di Luise
 *				Silvestro.Di.Luise@cern.ch
 *
 *				
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <TObject.h>
#include <TFile.h>
#include <TTree.h>

#include "EventHeader.h"


class TString;
class TFolder;


namespace EDM {

class Event: public TObject {

public:
	Event();
	virtual ~Event();

	void Clear();
	void Init();

	EDM::EventHeader& GetHeader() { return fEventHeader; }

	static TFolder* BuildFolder(TFolder *fld_top, TString name, TString title);

	TFolder* Top() {return fFldTop; }
	TFolder* MC() { return fFldMC;}
	TFolder* Det() { return fFldDet;}
	TFolder* DetSim() { return fFldDetSim;}
	TFolder* Raw() { return fFldRaw;}

	TFolder* fFldTop;
	TFolder* fFldDetSim;
	TFolder* fFldMC;
	TFolder* fFldRaw;
	TFolder* fFldDet;

	void Store(TString opt="");
	void SetStore(TTree*, TFile*);

private:

	EDM::EventHeader fEventHeader;

	TTree *fTree;
	TFile *fFile;

	//ClassDef(EDM::Event,1);
};

} /* namespace EDM */


#endif /* EVENT_H_ */
