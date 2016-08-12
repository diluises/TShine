/*
 * TSFunc1DCovolution.h
 *
 *  Created on: May 14, 2016
 *      Author: Silvestro di Luise
 *			    Silvestro.Di.Luise@cern.ch
 *
 *
 *
 */

#ifndef TSFUNC1DCONV_H_
#define TSFUNC1DCONV_H_

#include <iostream>

#include <TMath.h>

#include "TSFunction1D.h"



using std::cout;
using std::endl;
/*
 *
 */


class TSFunc1DConvolution: public TSFunction1D {


public:
	TSFunc1DConvolution(TString name, TString title
			    ,const TSVariable &var_x
			    ,TSFunction1D *f1, TSFunction1D *f2
			    ,double xmin, double xmax
			    );


	virtual ~TSFunc1DConvolution();

	virtual double Evaluate() const;

	double EvalNumConv(double t) const;

protected:

	TSFunction1D *fFunc1;
	TSFunction1D *fFunc2;

	double fXmin;
	double fXmax;

};

#endif /* TSFUNC1DCONV_H_ */
