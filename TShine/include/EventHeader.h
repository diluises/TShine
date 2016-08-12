/*
 * EventHeader.h
 *
 *  Created on: May 7, 2015
 *      Author: Silvestro di Luise
 *				Silvestro.Di.Luise@cern.ch
 *				
 */

#ifndef EVENTHEADER_H_
#define EVENTHEADER_H_

#include <iostream>

#include <TObject.h>
#include <TTimeStamp.h>


#include "EventHeader.h"



namespace EDM {

class EventHeader: public TObject {

public:
	EventHeader() { }
	virtual ~EventHeader() { }

	int RunNum() const { return fRunNum; }
	int EvtNum() const { return fEvtNum; }
	TTimeStamp&  TimeStamp() { return fTimeStamp; }

	void SetRunNum(int n) {fRunNum = n;}
	void SetEvtNum(int n) {fEvtNum = n;}

	void Print(Option_t *option="") const {

		std::cout<<" EventHeader "<<std::endl;
		std::cout<<" Run: "<<fRunNum<<" Event: "<<fEvtNum<<std::endl;
		fTimeStamp.Print();
		std::cout<<" "<<std::endl;
	}

private:
	TTimeStamp fTimeStamp;

	int fRunNum;
	int fEvtNum;

	//ClassDef(EDM::EventHeader, 1)
};

} /* namespace EDM */

#endif /* EVENTHEADER_H_ */


