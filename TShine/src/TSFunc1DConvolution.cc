/*
 *
 *
 * TSFunc1DConvolution.cc
 *
 *  Created on: May 14, 2016
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <cassert>

#include <TMath.h>
#include <Math/Integrator.h>
#include <Math/IntegratorMultiDim.h>
#include <Math/IntegratorOptions.h>
#include <Math/GaussIntegrator.h>
#include <Math/GaussLegendreIntegrator.h>
#include <Math/AdaptiveIntegratorMultiDim.h>
#include <Math/Functor.h>
#include <TVirtualFFT.h>
#include <TClass.h>


#include "MessageMgr.h"

#include "TSFunc1DConvolution.h"



class TSFunc1DConvolution_EvalWrapper {

  TSFunction1D *f1;
  TSFunction1D *f2;
  double ft;
    
public:
 
  TSFunc1DConvolution_EvalWrapper(TSFunction1D *f1, TSFunction1D *f2, double t)
        : f1(f1), f2(f2), ft(t)
     {
    }
     double operator()(double x) const
     {
        
        return f1->Eval(x) * f2->Eval(ft-x);
     }

};


TSFunc1DConvolution::TSFunc1DConvolution(TString name, TString title
					 ,const TSVariable &var_x
					 ,TSFunction1D *f1, TSFunction1D *f2 
					 ,double xmin, double xmax
					)

:TSFunction1D(name,title,var_x)
,fFunc1(f1)
,fFunc2(f2)
,fXmin(xmin)
,fXmax(xmax)
{



}

TSFunc1DConvolution::~TSFunc1DConvolution() {


}


double TSFunc1DConvolution::Evaluate() const{
  /**


   **/

  double t = fGetCachedX();

  
  double result = EvalNumConv(t);

  return result;

}


double TSFunc1DConvolution::EvalNumConv(double t) const {


  TSFunc1DConvolution_EvalWrapper fconv(fFunc1,fFunc2,t);
  

  ROOT::Math::IntegratorOneDim integrator(fconv, ROOT::Math::IntegratorOneDimOptions::DefaultIntegratorType(), 1e-9, 1e-9);

  double result = 0;


  if (fXmin != - TMath::Infinity() && fXmax != TMath::Infinity() )
    result =  integrator.Integral(fXmin, fXmax);
  else if (fXmin == - TMath::Infinity() && fXmax != TMath::Infinity() )
    result = integrator.IntegralLow(fXmax);
  else if (fXmin != - TMath::Infinity() && fXmax == TMath::Infinity() )
    result = integrator.IntegralUp(fXmin);
  else if (fXmin == - TMath::Infinity() && fXmax == TMath::Infinity() )
    result = integrator.Integral();


  return result;
}


