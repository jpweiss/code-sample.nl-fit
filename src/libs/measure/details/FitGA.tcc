// -*- C++ -*-
// Implementation of class FitGA
//
// Copyright (C) 2008-2009 by John Weiss
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
//FitGA_cc__="RCS $Id: FitGA.tcc 1887 2009-10-13 20:05:57Z candide $";
#ifndef _FitGA_TCC_
#define _FitGA_TCC_


// Includes
//

#ifdef DEBUG_GA
#include <iostream>
#include "Manips.h"
#endif
#include <algorithm>
#include <limits>
#include "jpw_nld.h"
#include "FORTLib.h" // For namespace sort & ivector_t
#include "BarrierModels.h"

#include "details/BarrierModels.tcc"


// Wrapper for parent namespace
namespace jpw_nld {
namespace optimize {

using std::numeric_limits;

using namespace jpw_nld::fortlib;
using namespace jpw_math::statistics;


// Convenience Macros for defining Template member functions.
//
#define TEMPL_M_FitGA template<class FIT_FN, \
                               class DATA_T, \
                               unsigned POPULATION_SIZE, \
                               unsigned N_GENERATIONS, \
                               unsigned N_MUTATE, \
                               unsigned N_ELITE, \
                               unsigned WIDE_XOVER_WEIGHT_X_1000>

#define CT_M_FitGA FitGA<FIT_FN, \
                         DATA_T, \
                         POPULATION_SIZE, \
                         N_GENERATIONS, \
                         N_MUTATE, \
                         N_ELITE, \
                         WIDE_XOVER_WEIGHT_X_1000>


//
// Typedefs
//


//
// Static Member Variables
//


TEMPL_M_FitGA
const double CT_M_FitGA::WIDE_XOVER_WEIGHT(WIDE_XOVER_WEIGHT_X_1000*0.001);


/////////////////////////

//
// Preprocessor Macros:  Debugging messages
//

#ifdef DEBUG_GA
 const bool IS_DEBUG_GA_ON=true;
#else
 const bool IS_DEBUG_GA_ON=false;
#endif


/////////////////////////

//
// Tracing Class
//


template<bool F> struct TraceGA
{
    typedef vector<dvector_t> popVec_t;

    TraceGA()
    {
        using namespace std;
        cout << endl << "Beginning minimization:" << endl << endl;
    }

    void BlankLines()
    {
        using namespace std;
        cout << endl << endl;
    }

    ~TraceGA()
    {
        this->BlankLines();
    }

    void StartGeneration(const step_t k)
    {
        using namespace std;
        cout << "Generation #" << k+1 << endl << "--------" << endl << endl
             << "Parameters:" << endl;
    }

    void CostRating(const step_t i,
                    const dvector_t& chiPop, const dvector_t& chiRating)
    {
        using namespace std;
        using namespace outManips;
        cout << setw(2) << i << "::   Cost=" << scientific << chiPop[i]
             << "\trating=" << fixed << chiRating[i] << reset << endl;
    }

    void ParamPop(const step_t i, const index_t n,
                  const popVec_t& lastParamPop)
    {
        using namespace std;
        using namespace outManips;
        cout << setw(2) << i << "::   " << setprecision(3) << scientific;
        for(step_t j=0; j<n; ++j) {
            cout << lastParamPop[i][j] << "\t";
        }
        cout << reset << endl;
    }
};


template<> struct TraceGA<false>
{
    typedef vector<dvector_t> popVec_t;

    TraceGA() {}
    ~TraceGA() {}
    void BlankLines() {}
    void StartGeneration(const step_t) {}
    void CostRating(const step_t, const dvector_t&, const dvector_t&) {}
    void ParamPop(const step_t, const index_t, const popVec_t&) {}
};


/////////////////////////

//
// General Function Definitions
//


/////////////////////////

//
// FitGA::PopVecFiller Member Functions
//


TEMPL_M_FitGA
inline void
CT_M_FitGA::PopVecFiller::operator()(popVec_t& populationVector)
{
    std::for_each(populationVector.begin(), populationVector.end(), *this);
}


TEMPL_M_FitGA
inline void
CT_M_FitGA::PopVecFiller::operator()(dvector_t& val)
{
    std::fill(val.begin(), val.end(), 0.0);
}


/////////////////////////

//
// FitGA Member Functions
//


TEMPL_M_FitGA
void CT_M_FitGA::initializePopulation(popVec_t& ppop,
                                      const FitData_t& theData,
                                      Model_t& theModel)
{
    for(step_t k=0; k<INITIAL_POPULATION_SIZE; ++k)
    {
        theModel.randomParams(m__wrkPop0[k]);
        m__wrkChiPop0[k] = theModel.chiSquared(theData, m__wrkPop0[k]);
    }

    // Get the sort indices for Chi^2, in ascending order.
    // [N.B.:  sort::byIndex() erases its second arg.]
    sort::byIndex(m__wrkChiPop0, m__wrkSortPop0, +1);

    // Sort according to best-fit, i.e. Chi^2.  Note that this is *not* the
    // fitness-metric used by the GA proper.
    for(step_t i=0; i<POPULATION_SIZE; ++i) {
        ppop[i] = m__wrkPop0[m__wrkSortPop0[i]-1];
    }
}


TEMPL_M_FitGA
void CT_M_FitGA::breedAndMutate(popVec_t& newpop,
                                const popVec_t& oldpop,
                                const dvector_t& ratingv)
{
    // Get the sort indices for the ratings, in descending order.
    // [N.B.:  sort::byIndex() erases its second arg.]
    //
    // NOTE:
    // The 'ratingv' is NOT Chi^2.  It's actually the normalized reciprocal of
    // Chi^2.  This is why we sort in descending order - the smallest Chi^2
    // produces the largest 'ratingv'.
    sort::byIndex(ratingv, m__wrkSortChiSq, -1);

    // Replicate the best ones.
    for(step_t i=0; i<N_BREEDING; ++i) {
        // FIXME:  The '-1' deals with FORTRAN-offset indices.  It won't be
        // needed if we cut over to a C++ sorting routine.
        newpop[i] = oldpop[m__wrkSortChiSq[i]-1];
    }

    // Now use crossover on the very best population members to produce the
    // remaining new members.
    double randa, oldx1, oldx2;
    for(step_t i=N_BREEDING; i<POPULATION_SIZE; i+=2)
    {
        index_t ri1 = randidx(N_BREEDING);
        index_t ri2 = randidx(N_BREEDING);
        for(index_t j=0; j<N_PARAMETERS; ++j) {
            randa = drand48()*WIDE_XOVER_WEIGHT;
            oldx1 = newpop[ri1][j];
            oldx2 = newpop[ri2][j];
            newpop[i][j] = randa*oldx1 + (1.0-randa)*oldx2;
            newpop[i+1][j] = (1.0-randa)*oldx1 + randa*oldx2;
        }
        Model_t::limitParams(newpop[i]);
        Model_t::limitParams(newpop[i+1]);
    }


    // Do N_MUTATE mutations per parameter, randomly picking the
    // individual to mutate.  Make an exception for the top N_ELITE
    // individuals, who never mutate.
    for(step_t i=0; i<N_MUTATE; ++i)
    {
        // Not a random individual, but a vector of mutation genes.
        Model_t::randomParams(m__wrkMutantDNA);
        // Instead of randomly picking the gene to mutate and somehow making
        // sure that we don't mutate the same gene twice, just pick an
        // individual for each gene and mutate that one gene.
        for(index_t j=0; j<N_PARAMETERS; ++j) {
            index_t ri1 = randidx(N_NONELITE) + N_ELITE;
            newpop[ri1][j] = m__wrkMutantDNA[j];
        }
    }
}


TEMPL_M_FitGA
double CT_M_FitGA::operator()(dvector_t& parm_min,
                              const FitData_t& theData,
                              Model_t& theModel)
{
    TraceGA<IS_DEBUG_GA_ON> debugMsg;

    // Clear the contents of a few members.  No need to touch m__LastParamPop;
    // initializePopulation() will take care of that for you.
    m__zeroingFunctor(m__NextParamPop);
    initializePopulation(m__LastParamPop, theData, theModel);

    double net_inv_chi;
    for(step_t k=0; k<N_GENERATIONS-1; ++k)
    {
        debugMsg.StartGeneration(k);
        net_inv_chi = 0.0;
        for(step_t i=0; i<POPULATION_SIZE; ++i) {
            m__ChiPop[i] = theModel.chiSquared(theData, m__LastParamPop[i]);
            net_inv_chi += 1.0/m__ChiPop[i];
            debugMsg.ParamPop(i, N_PARAMETERS, m__LastParamPop);
        }
        debugMsg.BlankLines();

        for(step_t i=0; i<POPULATION_SIZE; ++i) {
            m__ChiRating[i] = 1.0/m__ChiPop[i]/net_inv_chi;
            debugMsg.CostRating(i, m__ChiPop, m__ChiRating);
        }

        breedAndMutate(m__NextParamPop, m__LastParamPop, m__ChiRating);

        // Lastly, swap the objects containing the "old" and "new" members,
        // which disposes of the old population vector without
        // deallocating/reallocating.
        m__LastParamPop.swap(m__NextParamPop);
    } // for(k ...)

    // At the very end, search for the population minimum.
    double min_chi = numeric_limits<double>::max();
    step_t n = 0;
    for(step_t i=0; i<POPULATION_SIZE; ++i)
    {
        m__ChiPop[i] = theModel.chiSquared(theData, m__LastParamPop[i]);
        if(m__ChiPop[i] < min_chi) {
            min_chi = m__ChiPop[i];
            n = i;
        }
    }

    // Return the most-successful member of the population and its
    // corresponding Chi^2
    parm_min = m__LastParamPop[n];
    return(min_chi);
}


// End namespace wrapper decls.
}; //end namespace optimize
}; //end namespace jpw_nld


#endif //_FitGA_TCC_
/////////////////////////
//
// End
