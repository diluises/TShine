/*
 * TH1FAuto.cc
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <limits.h>
#include <iostream>

#include <TROOT.h>
#include <TH1F.h>
#include <THLimitsFinder.h>

#include "TH1FAuto.h"

#include "MessageMgr.h"

//ClassImp(TH1FAuto);


using std::cout;
using std::endl;

TH1FAuto::TH1FAuto()
:TH1F("","",1,0.,1.)
{

	fInit();
}

TH1FAuto::TH1FAuto(const char* name, const char *title)
: TH1F(name,title,1,0.,1.)
{

  fInit();

}



TH1FAuto::TH1FAuto(const char* name, const char *title, double low, double up)
: TH1F(name,title,1,0.,1.)
{


  fInit();

  SetRangeLimitsX(low,up);

}



TH1FAuto::TH1FAuto(const char* name, const char *title, int n, double xmin, double xmax)
: TH1F(name,title,n,xmin,xmax)
{


  fInit();

  fIsBuilt=true;

  fIsAuto=false;
}




TH1FAuto::TH1FAuto(const char* name, const char *title, int n, double* bins)
: TH1F(name,title,n,bins)
{


  fInit();

  fIsBuilt=true;

}


TH1FAuto::TH1FAuto(const TH1FAuto& other)
{

	TH1FAuto::Copy(other);


}


void TH1FAuto::Copy(const TH1FAuto& other)
{


  //cout<<__FILE__<<"::"<<__FUNCTION__<<": "<<GetName()<<endl;

	if(other.IsBuilt() ){

		  cout<<__FILE__<<"::"<<__FUNCTION__<<":"<<GetName()<<endl;

		  TH1F::Copy( *(TH1F*)&other );

	  }else{


		  fInit();

		  if( other.HasLowRangeLimitX() ){
			  SetLowRangeLimitX( other.GetLowRangeLimitX() );
		  }
		  if( other.HasUpRangeLimitX() ){
			  SetUpRangeLimitX( other.GetUpRangeLimitX() );
		  }


	  }

}


TObject* TH1FAuto::Clone(const char* newname) const{


  cout<<__FILE__<<"::"<<__FUNCTION__<<": "<<GetName()<<endl;

  TH1FAuto *h=new TH1FAuto(*this);

  h->SetName(newname);

  return h;

}


TH1FAuto::~TH1FAuto() {


}



int TH1FAuto::BufferEmpty(int action){



	if(action==4){
		//cout<<"calling BufferEmpty: "<<action<<endl;
		FlushBuffer();
		return 1;
	}

	//cout<<"calling BufferEmpty: "<<action<<endl;
	return TH1F::BufferEmpty();
}



void TH1FAuto::Draw(Option_t* opt){

     FlushBuffer();

     TH1F::Draw(opt);
}



int TH1FAuto::Fill(double x) {

  

  if( !fIsBuilt ){

    fFillBuffer(x);

    return GetBufferSize();

  }else{

    return TH1F::Fill(x);
  }


}



void TH1FAuto::FillBufferTo(double *array){



}


void TH1FAuto::FillBufferTo(TH1FAuto &h){


}


void TH1FAuto::FlushBuffer() {

  
  if(fIsBuilt) return;


  
  if( fBufferX.size()==0 ){

    double xmin = 0;
    double xmax = 1;

    if(fRangeHasLimit[0]) xmin = fRangeLimit[0];
    if(fRangeHasLimit[1]) xmax = fRangeLimit[1];

    TH1F::SetBins(1,xmin,xmax);

    ResetBuffer();
    fIsBuilt=true;

    SetStrategy(0);

    return;

  }
  

  if( fStrategy==1 ){


    double binlow=0;
    double binhigh=0;

    double xmin = fBufferMinMax[0];
    double xmax = fBufferMinMax[1];
    
    //cout<<" Flush "<<xmin<<" "<<xmax<<endl;

    TH1F::SetBins(fAutoRangeNBins,0,1);

    THLimitsFinder::GetLimitsFinder()->FindGoodLimits(this,xmin,xmax);

    //TH1F::SetBins(fAutoRangeNBins,binlow,binhigh);

    int N = fBufferX.size();

    for(int i=0; i<N; ++i){
      TH1F::Fill( fBufferX.at(i) );
    }

    N = fBufferXOut.size();

    if(N>0){

      for(int i=0; i<N; ++i){
	TH1F::Fill( fBufferXOut.at(i) );
      }

    }//



  }else{

    if( !fIsBuilt ){
      cout<<" !!! ERROR !!!: "<<__FILE__<<"::"<<__FUNCTION__<<" No strategy set to build the Histogram"<<endl;
    }
    return;


  }


  ResetBuffer();

  fIsBuilt=true;

  SetStrategy(0);
  


  return ;

}




int TH1FAuto::GetBufferSize() const{

  return fBufferX.size() + fBufferXOut.size();



}

double TH1FAuto::GetLowRangeLimitX() const{

	return fRangeLimit[0];

}

 double TH1FAuto::GetUpRangeLimitX() const {

	 return fRangeLimit[1];

}

bool TH1FAuto::HasLowRangeLimitX() const{

	return fRangeHasLimit[0];

 }

 bool TH1FAuto::HasUpRangeLimitX() const {

	 return fRangeHasLimit[1];

 }


double TH1FAuto::GetMean(int axis){


	FlushBuffer();

	return TH1F::GetMean(axis);

}

double TH1FAuto::GetRMS(int axis){

	FlushBuffer();

	return TH1F::GetRMS(axis);

}

void TH1FAuto::Reset() {


	if(fIsBuilt){

		TH1F::Reset();

	}else{
		ResetBuffer();
	}

}


void TH1FAuto::ResetBuffer() {

	fBufferX.clear();
	fBufferXOut.clear();

	fBufferX.resize(0);
	fBufferXOut.resize(0);
       
	fBufferX.shrink_to_fit();
	fBufferXOut.shrink_to_fit();


	fBufferMinMax[0]=DBL_MAX;
	fBufferMinMax[1]=DBL_MIN;
	

}


void TH1FAuto::SetMaxBufferSize(int val) {

  fMaxBufferSize=val;

}


void TH1FAuto::SetAutoRangeNumOfBins(int n){


  if(n<=0){
    std::cout<<__FILE__<<"::"<<__FUNCTION__<<" Illegal Num. of Bins: "<<n<<endl;
    return ;
  }

  fAutoRangeNBins=n;

}


void TH1FAuto::SetLowRangeLimitX(double val){

  fRangeHasLimit[0]=true;
  fRangeLimit[0]=val;

  if( fRangeHasLimit[1] && fRangeLimit[0]>=fRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH1FAuto::SetRangeLimitsX(double low, double up){

  if(low>=up){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitX(low);
  SetUpRangeLimitX(up);

}



void TH1FAuto::SetStrategy(int val) {

  fStrategy=val;

}



void TH1FAuto::SetUpRangeLimitX(double val){

  fRangeHasLimit[1]=true;
  fRangeLimit[1]=val;

  if( fRangeHasLimit[0] && fRangeLimit[0]>=fRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH1FAuto::fClearRangeLimits() {

  fRangeHasLimit[0]=false;
  fRangeHasLimit[1]=false;

  fRangeLimit[0]=0;
  fRangeLimit[1]=0;


  return;

}



void TH1FAuto::fFillBuffer(double x) {

  if(fIsBuilt){
    return;
  }

 

  if( fRangeHasLimit[0] && x<fRangeLimit[0] ){


    fBufferXOut.push_back(x);

  }else if( fRangeHasLimit[1] && x>fRangeLimit[1] ){


    fBufferXOut.push_back(x);

  }else{

 
    fBufferX.push_back(x);

    if(x<fBufferMinMax[0]) fBufferMinMax[0]=x;
    if(x>fBufferMinMax[1]) fBufferMinMax[1]=x;

    //cout<<"fillbuffer "<<x<<" "<<fBufferX.size()<<endl;

  }


  if( fMaxBufferSize>0 && fBufferX.size()==fMaxBufferSize ){

    FlushBuffer();

  }


}



void TH1FAuto::fInit(){

  fIsBuilt=false;
  fIsAuto=true;

  SetStrategy(1);
  SetMaxBufferSize(1000);
  

  fBufferMinMax[0]=DBL_MAX;
  fBufferMinMax[1]=DBL_MIN;

  SetAutoRangeNumOfBins(100);
  

  fClearRangeLimits();
}



int TH1FAuto::Write(const char* name, Int_t option, Int_t bufsize) {

	/**
	 *
	 *  Override the TObject method.
	 *  The const version is not yet implemented
	 *  considering that FlushBuffer is not const
	 *
	 *  N.B.:you must provide a name
	 *
	 *
	 */

	FlushBuffer();

	return TH1F::Write(name,option,bufsize);

}

