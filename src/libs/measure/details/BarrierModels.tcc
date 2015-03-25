// -*- C++ -*-
// Implementation of class BarrierModels
//
// Copyright (C) 2009-2010, 2015 by John Weiss
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
//static const char* const
//BarrierModels_cc__="RCS $Id: BarrierModels.tcc 1906 2010-03-03 20:32:43Z candide $";
#ifndef _BarrierModels_TCC_
#define _BarrierModels_TCC_


// Includes
//

#include <cstdlib>
#include "statistics.h"
#include "PersistenceMap.h"


// Wrapper for parent namespace
namespace jpw_nld {
namespace measure {


/////////////////////////

//
// BarrierModel<policy::Full> Member Functions
//


// Note:  Must use the typedef for BarrierModel<policy::Full>.  The compiler
// gets confused otherwise.


template<>
inline void
BarrierModel<policy::Full>::randomParams(dvector_t& params)
{
    // Beta (:==position);  Range: [0, 1]
    params[0] = drand48();

    // alpha = aCos(2A-1).  This does the same thing.
    double tmprand = jpw_math::statistics::range_rand(1.0);
    params[1] = acos(tmprand);

    // Rho (:== scaled  tau);  Range:  (0, 10]
    // Range:  (0, 1]
    // Add on a small nonzero piece to avoid a singularity.
    tmprand=0.999*drand48()+0.001;
    params[2] = scale_width(tmprand);

    // Rho (:== scaled  tau);  Range:  (0, 10]
    // Again, add on a small nonzero piece to avoid a singularity.
    tmprand=9.999*drand48()+0.001;
    params[3] = scale_tau(tmprand);
}


template<>
inline void
BarrierModel<policy::Full>::limitParams(dvector_t& params)
{
    // The 3rd parameter in the persistence-map model is the scaled-width.
    // The normal barrier width is restricted to the range, [-1, 1].  Since
    // the scaled width is based on 1/width, it must remain outside of this
    // range or at the boundaries.
    if( (-1.0 < params[2]) && (params[2] < 1.0) ) {
        params[2] = 1.0;
    }
}


template<>
template<typename VT> void
BarrierModel<policy::Full>::calculate(const PersistenceMap& theMap,
                                      VT& fitParams,
                                      VT& deltas, VT& fnJacob,
                                      int actionCode)
{
    typedef PersistenceMap::size_type data_size_t;
    typedef PersistenceMap::const_vector_type const_vector_t;

    // Aliases for the vectors underlying the theMap member matrices.
    const_vector_t& theMapV_data = theMap.as_1D();
    const_vector_t& theMapV_phases = theMap.phases_as_1D();
    const_vector_t& theMapV_lags = theMap.lags_as_1D();

    // Common setup.  Also limits parameter values.
    data_size_t nData = theMap.size();

    if(fitParams[0] > 1.0) {
        fitParams[0] = jpw_math::MOD_1(fitParams[0]);
    } else if(fitParams[0] < 0.0) {
        fitParams[0] = jpw_math::MOD_1(fitParams[0]) + 1.0;
    }
    double width = jpw_math::SQR(fitParams[2]);
    double dwidth = 2*fitParams[2];
    double lrho = jpw_math::SQR(fitParams[3]);
    double dlrho = 2*fitParams[3];
    double alph = descale_ampl(fitParams[1]);
    double onema = 1-alph;
    double dalph = -0.5*sin(fitParams[1]);

    // The extra 2, below, is there for cases when kappa1/width < 1 and to
    // take roundoff into account.
    int ne = static_cast<int>(KAPPA1/width) + 2;
    int ne_p1 = ne + 1;

    // Storage vars, set inside of for-loops.
    double pmb, pmlmb, pmb_j, pmlmb_j, sumh, sumdhde, sumdhdb, dhdx1, dhdx2;
    double e_lrho;

    // actionCode == "evaluate model"
    if(actionCode == FitLM::ComputeFunction)
    {
        for(data_size_t i=0; i<nData; ++i)
        {
            pmb = theMapV_phases[i] - fitParams[0];
            pmlmb = theMapV_phases[i] - fitParams[0] - theMapV_lags[i];

            // Do the "dangling term" in the sum
            pmlmb_j = (pmlmb + ne_p1);
            sumh = tanh(pmlmb_j*width);

            for(int j=-ne; j<ne_p1; ++j)
            {
                pmb_j = (pmb + j);
                pmlmb_j = (pmlmb + j);

                sumh += tanh(pmlmb_j*width) - tanh(pmb_j*width);
            }

            deltas[i] = ( (alph*sumh + onema*exp(-lrho*theMapV_lags[i]))
                        - theMapV_data[i] );
        } // end i
    } // end "evaluate model"

    // actionCode == "compute model deriv"
    if(actionCode == FitLM::ComputeJacobian)
    {
        data_size_t offset1 = nData;
        data_size_t offset2 = offset1 + nData;
        data_size_t offset3 = offset2 + nData;

        for(data_size_t i=0; i<nData; ++i)
        {
            pmb = theMapV_phases[i] - fitParams[0];
            pmlmb = theMapV_phases[i] - fitParams[0] - theMapV_lags[i];
            e_lrho = exp(-lrho*theMapV_lags[i]);

            // Do the "dangling term" in the sums
            pmlmb_j = (pmlmb + ne_p1);
            dhdx1 = jpw_math::SQR(1.0/cosh(pmlmb_j*width));
            sumh = tanh(pmlmb_j*width);
            sumdhdb = dhdx1;
            sumdhde = dhdx1*pmlmb_j;

            for(int j=-ne; j<ne_p1; ++j)
            {
                pmb_j = (pmb + j);
                pmlmb_j = (pmlmb + j);
                dhdx1 = jpw_math::SQR(1.0/cosh(pmlmb_j*width));
                dhdx2 = jpw_math::SQR(1.0/cosh(pmb_j*width));

                sumh += tanh(pmlmb_j*width) - tanh(pmb_j*width);
                sumdhdb += dhdx1 - dhdx2;
                sumdhde += dhdx1*pmlmb_j - dhdx2*pmb_j;
            }

            fnJacob[i] = -alph*sumdhdb*width;
            fnJacob[i+offset1] = dalph*(sumh - e_lrho);
            fnJacob[i+offset2] = alph*dwidth*sumdhde;
            fnJacob[i+offset3] = -dlrho*theMapV_lags[i]*onema*e_lrho;
        } // end i
    } // end "compute model deriv"

    ++m__callcount;
}


/////////////////////////

//
// BarrierModel<policy::MarkovOnly> Member Functions
//


// Note:  Must use the typedef for BarrierModel<policy::MarkovOnly>.  The
// compiler gets confused otherwise.


template<>
inline void
BarrierModel<policy::MarkovOnly>::randomParams(dvector_t& params)
{
    // Range:  (0, 10]
    // Add on a small nonzero piece to avoid a singularity.
    params[0] = scale_tau(9.999*drand48()+0.001);
}


template<>
inline void
BarrierModel<policy::MarkovOnly>::limitParams(dvector_t&)
{
}


template<>
template<typename VT> void
BarrierModel<policy::MarkovOnly>::calculate(const PersistenceMap& theMap,
                                            VT& fitParams,
                                            VT& deltas, VT& fnJacob,
                                            int actionCode)
{
    typedef PersistenceMap::size_type data_size_t;
    typedef PersistenceMap::const_vector_type const_vector_t;

    // Aliases for the vectors underlying the theMap member matrices.
    const_vector_t& theMapV_data = theMap.as_1D();
    const_vector_t& theMapV_lags = theMap.lags_as_1D();

    // Common setup.  There is only one parameter for this model, rho.
    double lrho = jpw_math::SQR(fitParams[0]);
    double dlrho = 2*fitParams[0];
    data_size_t nData = theMap.size();

    // actionCode == "evaluate model"
    if(actionCode == FitLM::ComputeFunction) {
        for(data_size_t i=0; i<nData; ++i) {
            deltas[i] = exp(-lrho*theMapV_lags[i]) - theMapV_data[i];
        }
    }

    // actionCode == "compute model deriv"
    if(actionCode == FitLM::ComputeJacobian) {
        for(data_size_t i=0; i<nData; ++i) {
            fnJacob[i] = -dlrho*theMapV_lags[i]*exp(-lrho*theMapV_lags[i]);
        }
    }

    ++m__callcount;
}


/////////////////////////

//
// BarrierModel<policy::BarrierOnly> Member Functions
//


// Note:  Must use the typedef for BarrierModel<policy::MarkovOnly>.  The
// compiler gets confused otherwise.


template<>
inline void
BarrierModel<policy::BarrierOnly>::randomParams(dvector_t& params)
{
    // Beta (:==position);  Range: [0, 1]
    params[0] = drand48();

    // Rho (:== scaled  tau);  Range:  (0, 10]
    // Range:  (0, 1]
    // Add on a small nonzero piece to avoid a singularity.
    params[2] = scale_width(0.999*drand48()+0.001);
}


template<>
inline void
BarrierModel<policy::BarrierOnly>::limitParams(dvector_t& params)
{
    // This has the same implementation as the policy::Full specialization.
    // It's small, so we'll hand-inline it.
    if( (-1.0 < params[2]) && (params[2] < 1.0) ) {
        params[2] = 1.0;
    }
}


template<>
template<typename VT> void
BarrierModel<policy::BarrierOnly>::calculate(const PersistenceMap& theMap,
                                             VT& fitParams,
                                             VT& deltas, VT& fnJacob,
                                             int actionCode)
{
    typedef PersistenceMap::size_type data_size_t;
    typedef PersistenceMap::const_vector_type const_vector_t;

    // Aliases for the vectors underlying the theMap member matrices.
    const_vector_t& theMapV_data = theMap.as_1D();
    const_vector_t& theMapV_phases = theMap.phases_as_1D();
    const_vector_t& theMapV_lags = theMap.lags_as_1D();

    // Common setup.  Also limits parameter values.  (There are two parameters
    // for this model:  beta and width.)
    data_size_t nData = theMap.size();

    if(fitParams[0] > 1.0) {
        fitParams[0] = jpw_math::MOD_1(fitParams[0]);
    } else if(fitParams[0] < 0.0) {
        fitParams[0] = jpw_math::MOD_1(fitParams[0]) + 1.0;
    }
    double width = jpw_math::SQR(fitParams[1]);
    double dwidth = 2*fitParams[1];

    // The extra 2, below, is there for cases when kappa1/width < 1 and to
    // take roundoff into account.
    int ne = static_cast<int>(KAPPA1/width) + 2;
    int ne_p1 = ne + 1;

    // Storage vars, set inside of for-loops.
    double pmb, pmlmb, pmb_j, pmlmb_j, sumh, sumdhde, sumdhdb, dhdx1, dhdx2;

    // actionCode == "evaluate model"
    if(actionCode == FitLM::ComputeFunction)
    {
        for(data_size_t i=0; i<nData; ++i)
        {
            pmb = theMapV_phases[i] - fitParams[0];
            pmlmb = theMapV_phases[i] - fitParams[0] - theMapV_lags[i];

            // Do the "dangling term" in the sum
            pmlmb_j = (pmlmb + ne_p1);
            sumh = tanh(pmlmb_j*width);

            for(int j=-ne; j<ne_p1; ++j)
            {
                pmb_j = (pmb + j);
                pmlmb_j = (pmlmb + j);

                sumh += tanh(pmlmb_j*width) - tanh(pmb_j*width);
            }

            deltas[i] = ( sumh - theMapV_data[i] );
        } // end i
    } // end "evaluate model"

    // actionCode == "compute model deriv"
    if(actionCode == FitLM::ComputeJacobian)
    {
        data_size_t offset1 = nData;

        for(data_size_t i=0; i<nData; ++i)
        {
            pmb = theMapV_phases[i] - fitParams[0];
            pmlmb = theMapV_phases[i] - fitParams[0] - theMapV_lags[i];

            // Do the "dangling term" in the sums
            pmlmb_j = (pmlmb + ne_p1);
            dhdx1 = jpw_math::SQR(1.0/cosh(pmlmb_j*width));
            sumh = tanh(pmlmb_j*width);
            sumdhdb = dhdx1;
            sumdhde = dhdx1*pmlmb_j;

            for(int j=-ne; j<ne_p1; ++j)
            {
                pmb_j = (pmb + j);
                pmlmb_j = (pmlmb + j);
                dhdx1 = jpw_math::SQR(1.0/cosh(pmlmb_j*width));
                dhdx2 = jpw_math::SQR(1.0/cosh(pmb_j*width));

                sumh += tanh(pmlmb_j*width) - tanh(pmb_j*width);
                sumdhdb += dhdx1 - dhdx2;
                sumdhde += dhdx1*pmlmb_j - dhdx2*pmb_j;
            }

            fnJacob[i] = -sumdhdb*width;
            fnJacob[i+offset1] = dwidth*sumdhde;
        } // end i
    } // end "compute model deriv"

    ++m__callcount;
}


// End namespace wrapper decls.
}; //end namespace measure
}; //end namespace jpw_nld



#endif //_BarrierModels_TCC_
/////////////////////////
//
// End
