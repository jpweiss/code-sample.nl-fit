// -*- C++ -*-
// Implementation of class FitLM
//
// Copyright (C) 2007-2008, 2015 by John Weiss
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Artistic License, included as the file
// "LICENSE" in the source code archive.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// You should have received a copy of the file "LICENSE", containing
// the License John Weiss originally placed this program under.
//
static const char* const
FitLM_cc__="RCS $Id: FitLM.cc 1864 2009-08-18 02:50:20Z candide $";


// Includes
//
#include "FitLM.h"
#include "jpw_nld.h"
#include "MathTools.h"
#include "Matrix.h"

#include "details/Matrix.tcc"
#include "details/MatrixRef_Writer.h"


using namespace jpw_nld::fortlib;
using jpw_nld::index_t;


//
// Static variables
//


//
// Typedefs
//


/////////////////////////


//
// External FORTRAN Functions
//


#ifdef __cplusplus
extern "C" {
#endif
    extern void lmder_(FitLM::fit_function_ptr_t fitfnptr,
                       fort_ivar_t m_f, fort_ivar_t n_f,
                       fort_dvec_t x_f, fort_dvec_t fvec_f,
                       fort_dmat_t fjac_f, fort_ivar_t ldfjac_f,
                       fort_dvar_t ftol_f, fort_dvar_t xtol_f,
                       fort_dvar_t gtol_f,fort_ivar_t maxdev_f,
                       fort_dvec_t diag_f, fort_ivar_t mode_f,
                       fort_dvar_t factor_f, fort_ivar_t nprint_f,
                       fort_ivar_t info_f, fort_ivar_t nfev_f,
                       fort_ivar_t njev_f,fort_ivec_t ipvt_f,
                       fort_dvec_t qtf_f, fort_dvec_t wa1_f,
                       fort_dvec_t wa2_f, fort_dvec_t wa3_f,
                       fort_dvec_t wa4_f);
#ifdef __cplusplus
}; /* end extern "C" */
#endif


/////////////////////////

//
// FitLM Member Functions
//


FitLM::FitLM(fit_function_ptr_t ffp, index_t ndata_max, index_t nparm)
    : m__ffp(ffp)
    , m__ndataMax(ndata_max)
    , m__nparams(nparm)
    , m__jac_requiresUpdate(false)
    , m__wrksz(m__ndataMax + 5*m__nparams)
    , m__ipvt(new int[m__nparams])
    , m__workbuf(new double[m__wrksz])
    , m__diag(new double[m__nparams])
    , m__qtf(new double[m__nparams])
    , m__wa1(new double[m__nparams])
    , m__wa2(new double[m__nparams])
    , m__wa3(new double[m__nparams])
    , m__wa4(new double[ndata])
    , m__f_jac_flat(m__nparams*m__ndataMax)
    , m__f_vec(m__ndataMax)
    , m__f_jac_out(m__nparams, m__ndataMax)
{ }


FitLM::~FitLM()
{
    delete[] m__ipvt;
    delete[] m__workbuf;
    delete[] m__diag;
    delete[] m__qtf;
    delete[] m__wa1;
    delete[] m__wa2;
    delete[] m__wa3;
    delete[] m__wa4;
}


FitLMStatus_t
FitLM::operator()(int mm, dvector_t& xv, double errtol, double ptol,
                  int maxiter, double factor)
{
    double gtol=0.0;
    int mode=1, nprint=0;
    int nfev, njev;
    int inf=0;

    // Check to see if the parameters are correct.
    if( xv.empty() ||
        (xv.size() < static_cast<unsigned>(m__nparams)) ||
        (mm < m__nparams) || (m__ndataMax < mm) ||
        (errtol < 0.0) || (ptol < 0.0) || (maxiter < 0) ||
        (m__wrksz < (mm+5*m__nparams)) || (factor < 0.0) )
    {
        return(InputError);
    }

    if(maxiter==0) {
        maxiter = 100*(m__nparams + 1);
    }

    if(factor < 0.1) {
        factor=100.0;
    }

    lmder_(m__ffp, &mm, &m__nparams, &xv[0], &m__f_vec[0], &m__f_jac_flat[0],
           &m__ndataMax, &errtol, &ptol, &gtol, &maxiter, m__diag,
           &mode, &factor, &nprint, &inf, &nfev, &njev, m__ipvt, m__qtf,
           m__wa1, m__wa2, m__wa3, m__wa4);

    m__jac_requiresUpdate = true;

    if(inf == MachinePrec_LMAlg) {
        return MachinePrec;
    } // else
    return(static_cast<FitStatus_t>(inf));
}


/////////////////////////
//
// End
