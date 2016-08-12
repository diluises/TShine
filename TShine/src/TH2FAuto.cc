
/*
 * TH1FAuto.cc
 *
 *  Created on: May 12, 2014
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */

#include <limits.h>
#include <iostream>

#include <TROOT.h>
#include <TH2F.h>
#include <THLimitsFinder.h>

#include "RootUtils.h"

#include "TH2FAuto.h"


using std::cout;
using std::endl;

TH2FAuto::TH2FAuto()
  : TH2F("","",1,0.,1.,1,0.,1.)
{

	fInit();
}

TH2FAuto::TH2FAuto(const char* name, const char *title)
  : TH2F(name,title,1,0.,1.,1,0.,1.)
{

  fInit();


}



TH2FAuto::TH2FAuto(const char* name, const char *title, double xlow, double xup, double ylow, double yup)
  : TH2F(name,title,1,0.,1.,1,0.,1.)
{


  fInit();

  SetRangeLimits(xlow,xup,ylow,yup);

}


TH2FAuto::TH2FAuto(const char* name, const char *title, int nx, double xmin, double xmax, int ny, double ymin, double ymax)
  : TH2F(name,title,nx,xmin,xmax,ny,ymin,ymax)
{


  fInit();

  fIsBuilt=true;
  fIsAuto=false;

}


TH2FAuto::TH2FAuto(const char* name, const char *title, int nx, double* binsx, int ny, double* binsy)
  : TH2F(name,title,nx,binsx,ny,binsy)
{


  fInit();

  fIsBuilt=true;
  fIsAuto=false;
}



TH2FAuto::TH2FAuto(const TH2FAuto &other) {


	TH2FAuto::Copy(other);

}


void TH2FAuto::Copy( const TH2FAuto &other ){


  //cout<<__FILE__<<"::"<<__FUNCTION__<<": "<<GetName()<<endl;


	if(other.IsBuilt() ){

		TH2F::Copy( *(TH2F*)&other );

	}else{


		fInit();

		if(other.IsBinningXFix() ){

			FixBinningX(other.GetFixedNbinsX(),other.GetFixedBinsArrayX());

		}else{


			if( other.HasLowRangeLimitX() ) {
					SetLowRangeLimitX( other.GetLowRangeLimitX() );
			}

			if( other.HasUpRangeLimitX() ) {
				SetUpRangeLimitX( other.GetUpRangeLimitX() );
			}

		}

		if(other.IsBinningYFix() ){

			FixBinningY(other.GetFixedNbinsY(),other.GetFixedBinsArrayY());


		}else{

			if( other.HasLowRangeLimitY() ) {
				SetLowRangeLimitY( other.GetLowRangeLimitY() );
			}

			if( other.HasUpRangeLimitY() ) {
				SetUpRangeLimitY( other.GetUpRangeLimitY() );
			}

		}


		SetTitle(other.GetTitle());
		GetXaxis()->SetTitle(  other.GetXaxis()->GetTitle() );
		GetYaxis()->SetTitle(  other.GetYaxis()->GetTitle() );


	}//IsNotBuilt






}


TObject* TH2FAuto::Clone(const char* newname) const{

  cout<<__FILE__<<"::"<<__FUNCTION__<<": "<<GetName()<<endl;

  TH2FAuto *h=new TH2FAuto(*this);

  h->SetName(newname);

  return h;


}


TH2FAuto::~TH2FAuto() {



}


int TH2FAuto::BufferEmpty(int action){



	if(action==4){
		//cout<<"calling BufferEmpty: "<<action<<endl;
		FlushBuffer();
		return 1;
	}

	//cout<<"calling BufferEmpty: "<<action<<endl;
	return TH2F::BufferEmpty();
}



void TH2FAuto::Draw(Option_t* opt){

     FlushBuffer();

     TH2F::Draw(opt);
}


int TH2FAuto::Fill(double x, double y) {

  

  if( !fIsBuilt ){

    fFillBuffer(x,y);

    return GetBufferSize();

  }else{

    return TH2F::Fill(x,y);
  }


}

int TH2FAuto::FillBufferTo(TH2FAuto &h){

}



void TH2FAuto::FixBinningX(int n, double low, double up){

	fFixBinningX=true;

	fFixNumOfBins[0]=n;
	fFixLimitsX[0]=low;
	fFixLimitsX[1]=up;

	//fFixedAxisX.Set(n,low,up);

	double *xbins=RootUtils::CreateBinsArray(n,low,up);

	fFixedAxisX.Set(n,xbins);

	delete [] xbins;

	if(fFixBinningY){
	    FlushBuffer();
	}

}


void TH2FAuto::FixBinningX(int n, const double* x){

	fFixBinningX=true;

	fFixNumOfBins[0]=n;
	fFixLimitsX[0]=x[0];
	fFixLimitsX[1]=x[n];

	fFixedAxisX.Set(n,&x[0]);

	fFixAxisVariableBins[1]=true;

	if(fFixBinningY){
		FlushBuffer();
	}

}



void TH2FAuto::FixBinningY(int n, double low, double up){

	fFixBinningY=true;

	fFixNumOfBins[1]=n;
	fFixLimitsY[0]=low;
	fFixLimitsY[1]=up;

	fFixedAxisY.Set(n,low,up);

	double *xbins=RootUtils::CreateBinsArray(n,low,up);

	fFixedAxisY.Set(n,xbins);

	delete [] xbins;


	if(fFixBinningX){
		FlushBuffer();
	}

}


void TH2FAuto::FixBinningY(int n, const double* x){

	fFixBinningY=true;

	fFixNumOfBins[1]=n;
	fFixLimitsY[0]=x[0];
	fFixLimitsY[1]=x[n];

	fFixedAxisY.Set(n,&x[0]);

	fFixAxisVariableBins[1]=true;


	if(fFixBinningX){
		FlushBuffer();
	}

}


void TH2FAuto::FlushBuffer() {


  if(fIsBuilt) return;


  
  if( fBufferX.size()==0 ){
    
    double xmin=0;
    double xmax=1;

    double ymin=0;
    double ymax=1;

    
    if( !fFixBinningX && fFixBinningY ){

      if(fXRangeHasLimit[0]) xmin = fXRangeLimit[0];
      if(fXRangeHasLimit[1]) xmax = fXRangeLimit[1];

      TH2F::SetBins(1,RootUtils::CreateBinsArray(1,xmin,xmax),fFixNumOfBins[1],GetFixedBinsArrayY());

    }


    if( fFixBinningX && !fFixBinningY){

      if(fYRangeHasLimit[0]) ymin = fYRangeLimit[0];
      if(fYRangeHasLimit[1]) ymax = fYRangeLimit[1];

      TH2F::SetBins(fFixNumOfBins[0],GetFixedBinsArrayX(),1,RootUtils::CreateBinsArray(1,ymin,ymax));

    }


    ResetBuffer();

    fIsBuilt=true;
      
    SetStrategy(0);

    return;


  }
  

  if( fFixBinningX && fFixBinningY ){


    TH2F::SetBins(fFixNumOfBins[0],GetFixedBinsArrayX(),fFixNumOfBins[1],GetFixedBinsArrayY());
    

  }else
  if(fStrategy==1){


    TH2F::SetBins(fAutoRangeNBins[0],0.,1.,fAutoRangeNBins[1],0.,1.);

    THLimitsFinder::GetLimitsFinder()->FindGoodLimits(this,fBufferXMinMax[0],fBufferXMinMax[1],fBufferYMinMax[0],fBufferYMinMax[1]);


    int NX = this->GetNbinsX();
    int NY = this->GetNbinsY();

    double xmin = this->GetXaxis()->GetXmin();
    double xmax = this->GetXaxis()->GetXmax();

    double ymin = this->GetYaxis()->GetXmin();
    double ymax = this->GetYaxis()->GetXmax();



    if( fFixBinningX ){

      TH2F::SetBins(fFixNumOfBins[0],GetFixedBinsArrayX(),NY,RootUtils::CreateBinsArray(NY,ymin,ymax));


    }else if( fFixBinningY ){

      TH2F::SetBins(NX,RootUtils::CreateBinsArray(NX,xmin,xmax),fFixNumOfBins[1],GetFixedBinsArrayY());

    }else{

    TH2F::SetBins(NX,xmin,xmax,NY,ymin,ymax);

  }

  }else{

	  if( !fIsBuilt ){
		  cout<<" !!! ERROR !!!: "<<__FILE__<<"::"<<__FUNCTION__<<" No strategy set to build the Histogram: "<<GetName()<<endl;
	  }
	  return;
  }

  //
  // Now fill the histo with data from buffers
  //


  int Nent = fBufferX.size();

  for(int i=0;i<Nent;++i){
    TH2F::Fill(fBufferX[i],fBufferY[i]);
  }


  Nent = fBufferXOut.size();

  for(int i=0;i<Nent;++i){
    TH2F::Fill(fBufferXOut[i],fBufferYOut[i]);
  }
    
  ResetBuffer();

  fIsBuilt=true;
  
  SetStrategy(0);

  return;
}



int TH2FAuto::GetBufferSize() {

  return fBufferX.size() + fBufferXOut.size();

}



int TH2FAuto::GetFixedNbinsX() const{

	return fFixedAxisX.GetNbins();
}

const double* TH2FAuto::GetFixedBinsArrayX() const{

	if(fFixedAxisX.GetXbins())
		return fFixedAxisX.GetXbins()->GetArray();
}


int TH2FAuto::GetFixedNbinsY() const{

	return fFixedAxisY.GetNbins();


}


const double* TH2FAuto::GetFixedBinsArrayY() const{

	if(fFixedAxisY.GetXbins())
				return fFixedAxisY.GetXbins()->GetArray();
}




double TH2FAuto::GetLowRangeLimitX() const{
	return fXRangeLimit[0];
}
double TH2FAuto::GetLowRangeLimitY() const{
	return fYRangeLimit[0];
}
double TH2FAuto::GetUpRangeLimitX() const{
	return fXRangeLimit[1];
}
double TH2FAuto::GetUpRangeLimitY() const{
	return fYRangeLimit[1];
}
bool TH2FAuto::HasLowRangeLimitX() const{
	return fXRangeHasLimit[0];
}
bool TH2FAuto::HasLowRangeLimitY() const{
	return fYRangeHasLimit[0];
}
bool TH2FAuto::HasUpRangeLimitX() const{
	return fXRangeHasLimit[1];
}
bool TH2FAuto::HasUpRangeLimitY() const{
	return fYRangeHasLimit[1];
}

double TH2FAuto::GetMean(int axis) {

	FlushBuffer();

	return TH2F::GetMean(axis);
}

double TH2FAuto::GetRMS(int axis) {


	FlushBuffer();

	return TH2F::GetRMS(axis);

}


void TH2FAuto::Reset() {

	if(fIsBuilt){

		TH2F::Reset();

	}else{

		ResetBuffer();
	}

}


void TH2FAuto::ResetBuffer() {

	  fBufferX.clear();
	  fBufferY.clear();

	  fBufferXOut.clear();
	  fBufferYOut.clear();

	  fBufferX.resize(0);
	  fBufferY.resize(0);

	  fBufferXOut.resize(0);
	  fBufferYOut.resize(0);

	  fBufferX.shrink_to_fit();
	  fBufferY.shrink_to_fit();

	  fBufferXOut.shrink_to_fit();
	  fBufferYOut.shrink_to_fit();
	  
	  fBufferXMinMax[0]=DBL_MAX;
	  fBufferYMinMax[0]=DBL_MAX;
	  fBufferXMinMax[1]=DBL_MIN;
	  fBufferYMinMax[1]=DBL_MIN;


}


void TH2FAuto::SetAutoRangeNumOfBins(int nx, int ny){


  if(nx<=0){
    std::cout<<__FILE__<<"::"<<__FUNCTION__<<" Illegal Num. of Bins: "<<nx<<endl;
    return ;
  }


  if(ny<=0){
    std::cout<<__FILE__<<"::"<<__FUNCTION__<<" Illegal Num. of Bins: "<<ny<<endl;
    return ;
  }



  fAutoRangeNBins[0]=nx;
  fAutoRangeNBins[1]=ny;


}


void TH2FAuto::SetMaxBufferSize(int val) {

  fMaxBufferSize=val;

}



void TH2FAuto::SetLowRangeLimitX(double val){
  
  fXRangeHasLimit[0]=true;
  fXRangeLimit[0]=val;
  
  if( fXRangeHasLimit[1] && fXRangeLimit[0]>=fXRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::SetLowRangeLimitY(double val){
  
  fYRangeHasLimit[0]=true;
  fYRangeLimit[0]=val;
  
  if( fYRangeHasLimit[1] && fYRangeLimit[0]>=fYRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}


void TH2FAuto::SetRangeLimits(double xlow, double xup, double ylow, double yup){
  
  if(xlow>=xup || ylow>=yup){
    fClearRangeLimits();
    return;
  }

  SetRangeLimitsX(xlow,xup);
  SetRangeLimitsY(ylow,yup);
  
}



void TH2FAuto::SetRangeLimitsX(double xlow, double xup){
  
  if(xlow>=xup){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitX(xlow);
  SetUpRangeLimitX(xup);
  
}



void TH2FAuto::SetRangeLimitsY(double ylow, double yup){
  
  if(ylow>=yup){
    fClearRangeLimits();
    return;
  }

  SetLowRangeLimitY(ylow);
  SetUpRangeLimitY(yup);
  
}



void TH2FAuto::SetStrategy(int val) {

  fStrategy=val;

}



void TH2FAuto::SetUpRangeLimitX(double val){
  
  fXRangeHasLimit[1]=true;
  fXRangeLimit[1]=val;

  if( fXRangeHasLimit[0] && fXRangeLimit[0]>=fXRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::SetUpRangeLimitY(double val){
  
  fYRangeHasLimit[1]=true;
  fYRangeLimit[1]=val;

  if( fYRangeHasLimit[0] && fYRangeLimit[0]>=fYRangeLimit[1] ){
    fClearRangeLimits();
    return;
  }

}



void TH2FAuto::fClearRangeLimits() {
  

  fXRangeHasLimit[0]=false;
  fXRangeHasLimit[1]=false;
  fYRangeHasLimit[0]=false;
  fYRangeHasLimit[1]=false;

  fXRangeLimit[0]=0;
  fXRangeLimit[1]=0;
  fYRangeLimit[0]=0;
  fYRangeLimit[1]=0;

    
  return;

}



void TH2FAuto::fFillBuffer(double x, double y) {

  if(fIsBuilt) return;


  if(    (fXRangeHasLimit[0] && x<fXRangeLimit[0]) 
      || (fXRangeHasLimit[1] && x>fXRangeLimit[1])
      || (fYRangeHasLimit[0] && y<fYRangeLimit[0])
      || (fYRangeHasLimit[1] && y>fYRangeLimit[1])

      ){

   
    fBufferXOut.push_back(x);
    fBufferYOut.push_back(y);
    

  }else{


    fBufferX.push_back(x);
    fBufferY.push_back(y);

    if(x<fBufferXMinMax[0]) fBufferXMinMax[0]=x;
    if(x>fBufferXMinMax[1]) fBufferXMinMax[1]=x;
    
    if(y<fBufferYMinMax[0]) fBufferYMinMax[0]=y;
    if(y>fBufferYMinMax[1]) fBufferYMinMax[1]=y;


  }


  if( fMaxBufferSize>0 && fBufferX.size()==fMaxBufferSize ){

    FlushBuffer();

  }


}



void TH2FAuto::fInit(){


  fIsBuilt=false;
  fIsAuto=true;

  fFixBinningX=false;
  fFixBinningY=false;
  fFixAxisVariableBins[0]=false;
  fFixAxisVariableBins[1]=false;

  SetStrategy(1);
  SetMaxBufferSize(1000);

  SetAutoRangeNumOfBins(100,100);

  fBufferXMinMax[0]=DBL_MAX;
  fBufferYMinMax[0]=DBL_MAX;
  fBufferXMinMax[1]=DBL_MIN;
  fBufferYMinMax[1]=DBL_MIN;

  fClearRangeLimits();


}


int TH2FAuto::Write(const char* name, Int_t option, Int_t bufsize){

	/**
		 *
		 *  Override the TObject method.
		 *  The const version is not yet implemented
		 *  considering that FlushBuffer is not const
		 *
		 *
		 */

		FlushBuffer();

		return TH2F::Write(name,option,bufsize);

}

