/*
 * RootUtils.h
 *
 *  Created on: Nov 9, 2013
 *      Author: Silvestro di Luise
 *      Silvestro.Di.Luise@cern.ch

 *
 */

#ifndef ROOTUTILS_H
#define ROOTUTILS_H

#include <iostream>

#include <limits>
#include <string>
#include <TString.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

#include <TFile.h>
#include <TObject.h>
#include <TMath.h>
#include <TRandom.h>

class TH2;
class TH1;
class TH2F;
class TH1F;
class TH2Poly;

using namespace std;
 
using std::cout;
using std::endl;

namespace RootUtils {


	TGraph* BuildGraph(TString,TString, int N, double *x, double *y, int closed);

	TGraph* BuildGraph(TString,TString, double xmin,double xmax,double ymin,double ymax);

	bool CompareGraphs(TGraph &g1, TGraph &g2);

	double ComputeEffEntries(TH1&,TString opt="");
	double ComputeIntegral(TH1&,TString opt="");

	double* CreateBinsArray(int, double xmin, double xmax);

	std::vector<TString> DirFileList(TString path, TString ext,TString opt);

	void FillHistoFast(TH1F &, double x, double w);
	void FillHistoFast(TH2F &, double x, double y, double w);

	int GetBin(int nx,int binx);

	int GetBin(int nx,int binx,int ny, int biny);

	TString GetBinRangeLabel(TH1 &h, int bin, TString axis);

	TObject* GetObjFromFile(TString file, TString obj);

	float GetOutliers(TH1&);

	TGraphAsymmErrors* GetPoissonErrors(TH1 &h, TString ="");

	void ImportBinning(TH2F *h2, TH1F *hx, TH1F *hy);

	void ImportBinning(TH1F *h, TH1F *hx);
	template <typename T> bool IsTotalInside(T xp, T yp, Int_t np, T *x, T *y);

	bool IsInsideRectangular(TGraph *g, double x, double y);

	bool IsTH1(const TObject& );
	bool IsTH2(const TObject& );
	bool IsTH2Poly(const TObject& );

	bool IsTH1F(const TObject& );
	bool IsTH2F(const TObject& );

	TH2F* InvertAxis(TH2 &,int opt=0);

	void MeanAndRMS(std::vector<double> *val, std::vector<double> *w, double &mean, double &rms);
	void MeanAndRMSOld(std::vector<double> *val, std::vector<double> *w, double &mean, double &rms);

	double Normalize(TH1&,double norm,TString opt="");

	void Poisson(TH1 &in, TH1 &out, int opt=0);

	void Poisson(TH1 &, int opt=0);

	TH1F* newTH1F(TString name, TString title, int nbins, double low, double up);
	TH2F* newTH2F(TString name, TString title, int nbinsx, double lowx, double upx
											  ,int nbinsy, double lowy, double upy);


	bool OverlappingRectangulars(TGraph &g1, TGraph &g2);

	void PrintHistoInfo(TObject *h);

	TH1F* RatioHist1D(TString name,TString tit,TH1F &hnum, TH1F& hden, double hmin,double hmax,TString opt="");

	bool SameBinning(TH1 &h1, TH1 &h2);


	//Math Utils
	template<typename T>
	bool IsZero(T val){
		return TMath::AreEqualRel(val,0.,std::numeric_limits<T>::epsilon());
	}


	static TRandom fRandom;

	static TString STR_ERROR = " !!!!!! ";
	static TString STR_WARNING = " ###### ";

}









template <typename T> bool RootUtils::IsTotalInside(T xp, T yp, Int_t np, T *x, T *y)
{

   // Adapted from TMath::IsInside
   // In this version the point must lie totally inside the a
   // closed polygon (cannot be on a vertex or along a side)

	//TMath::IsInside:
   // Function which returns kTRUE if point xp,yp lies inside the
   // polygon defined by the np points in arrays x and y, kFALSE otherwise.
   // Note that the polygon may be open or closed.

   Int_t i, j = np-1 ;
   Bool_t oddNodes = kFALSE;

   for (i=0; i<np; i++) {

	   bool y_range_1_1=(y[i]<yp);
	   bool y_range_1_2=(y[j]>yp);

	   bool y_range_2_1=(y[j]<yp);
	   bool y_range_2_2=(y[i]>yp);

	   //workaround...to be fixed properly
	   //example: the first inequality holds even though y[i] and yp are equal
	   //this further check seems to work
	   if( y_range_1_1 && TMath::AreEqualRel(y[i],yp,1e-5) ) y_range_1_1=false;
	   if( y_range_1_2 && TMath::AreEqualRel(y[j],yp,1e-5) ) y_range_1_2=false;

	   if( y_range_2_1 && TMath::AreEqualRel(y[j],yp,1e-5) ) y_range_2_1=false;
	   if( y_range_2_2 && TMath::AreEqualRel(y[i],yp,1e-5) ) y_range_2_2=false;

	   bool y_range_1 = y_range_1_1 && y_range_1_2;
	   bool y_range_2 = y_range_2_1 && y_range_2_2;


	   if(  y_range_1 || y_range_2  ){//
	   //if ((y[i]<yp && y[j]>yp) || (y[j]<yp && y[i]>yp)) { //problem with inequalities
    	  // if ((y[i]<yp && y[j]>=yp) || (y[j]<yp && y[i]>=yp)) {// Original TMath::IsInside

		   if (x[i]+(yp-y[i])/(y[j]-y[i])*(x[j]-x[i])<xp) {
            oddNodes = !oddNodes;
		   }
	   }
      j=i;
   }


   return oddNodes;

}

#endif /* ROOTUTILS_H */
