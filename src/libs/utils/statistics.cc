// -*- C++ -*-
// Implementation of class xFOOx
//
// Copyright (C) 2009-2010 by John Weiss
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
xFOOx_cc__="RCS $Id: statistics.cc 1887 2009-10-13 20:05:57Z candide $";


// Includes
//
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <boost/static_assert.hpp>

#include "jpw_nld.h"
#include "Matrix.h"
#include "Manips.h"
#include "MathTools.h"
#include "statistics.h"

#include "details/Matrix.tcc"


using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::scientific;
using std::setprecision;
using std::vector;
using outManips::reset;
using jpw_nld::index_t;

using namespace jpw_math;
using namespace jpw_math::statistics;


//
// Static variables
//


namespace {
 static bool g__pRNG_Initialized(false);
};


//
// Typedefs
//


typedef unsigned short seed48v_t[3];


/////////////////////////

//
// Inline Template Function Definitions
//


template<unsigned SZ> inline void
l__remap_seed__(time_t, seed48v_t&)
{
    BOOST_STATIC_ASSERT(false && SZ);
}


// Specialization for 64-bit time_t
template<> inline void
l__remap_seed__<8>(time_t rawSeedVal, seed48v_t& elSeed)
{
    // 64-bit time_t:  Ignore the upper 2 bytes, which change far more slowly
    //                 than the rest.
    // Layout:
    //    ........ ........ ........ ........ ........ ........
    // 48       40       32       24       16        8        0
    //           5        4        3        2        1        0

    // Bytes 0 and 5
    elSeed[0] = ( ((rawSeedVal & 0xFF) << 8)
                  | ((rawSeedVal >> 40) & 0xFF) );
    // Bytes 2 and 3
    elSeed[1] = ( ((rawSeedVal >> 8) & 0xFF00)
                  | ((rawSeedVal >> 24) & 0xFF) );
    // Bytes 4 and 1
    elSeed[2] = ( ((rawSeedVal >> 24) & 0xFF00)
                  | ((rawSeedVal >> 8) & 0xFF) );
}


// Specialization for 32-bit time_t
template<> inline void
l__remap_seed__<4>(time_t rawSeedVal, seed48v_t& elSeed)
{
    // 32-bit time_t:  Split into two 6-bit segments, and four 5-bit
    //                 segments.
    //
    // Layout - The Input:
    //   ........ ........ ........ ........
    //  |     |      |     |    |     |    |
    // 32    26     20    15   10     5    0
    //          3        2        1        0
    //
    // Layout - The Output:
    //    ......00 0.....00 .....000 00.....0 000..... 00......
    // 48       40       32       24       16        8        0
    //           5        4        3        2        1        0

    // Bits 0-4 and 26-31
    // Map 26-31 to the LSB, and 0-4 to the bit-0 of the upper byte.
    elSeed[0] = ( ((rawSeedVal & 0x1F) << 8)
                  | ((rawSeedVal >> 26) & 0x3F) );
    // Bits 10-14 and 15-19
    // Map 15-19 to the bit-1 of the lower byte, and 10-14 to the bit-3 of
    // the upper byte.
    elSeed[1] = ( ((rawSeedVal << 1) & 0xF800)
                  | ((rawSeedVal >> 14) & 0x3E) );
    // Bits 20-25 and 5-9
    // Map 5-9 to the bit-2 of the lower byte, and 20-25 to the bit-2 of
    // the upper byte.
    elSeed[2] = ( ((rawSeedVal >> 10) & 0x7C00)
                  | ((rawSeedVal >> 3) & 0x7C) );
}


/////////////////////////

//
// namespace statistics Function Definitions
//


// REFACTOR:  Use template functions & inline-functors to define the PRNG
// functions.
// Hmmm... use classes.  Design so that we can refactor the implementation at
// a later stage (like, say, the very last!)
//
// When refactoring the actual generators [like gaussian()], do some more
// performance tests.  See how well they inline.


void jpw_math::statistics::parseSeed(std::string& seedStr,
                                     unsigned& seedLo, unsigned& seedHi)
{
    // FIXME:  This is now part of 'jpwTools::RandomDataSrc'.
    using namespace std;

    if(seedStr.empty()) {
        return;
    }

    // Make sure there's a leading "0x" prefix, or you'll get parsing errors.
    if( (seedStr[0] == 'x') || (seedStr[0] == 'X') ) {
        seedStr.insert(0, 1, '0');
    } else if( (seedStr.size() > 1) &&
               !( (seedStr[0] == '0') &&
                  ((seedStr[1] == 'x') || (seedStr[1] == 'X')) )
               ) {
        seedStr.insert(0, "0x");
    }

    unsigned long long seedVal(0);
    istringstream hex_iss(seedStr);
    hex_iss >> hex >> seedVal;
    if(!hex_iss.good() && !hex_iss.eof()) {
        string err("Not a valid format hex string:  \"");
        err += seedStr;
        err += '"';
        seedStr.clear();
        throw runtime_error(err);
    }

    seedHi = static_cast<unsigned>(seedVal >> 32);
    seedLo = static_cast<unsigned>(seedVal & 0xFFFFFFFF);
}


void jpw_math::statistics::init_rand(bool verbose)
{
    seed48v_t elseed;
    // get time/date to use as a seed
    l__remap_seed__<sizeof(time_t)>(time(static_cast<time_t*>(0)), elseed);
    if (verbose) {
        cout << std::hex
             << "\t\t[Random seed=0x"
             << elseed[2] << elseed[1] << elseed[0]
             << "]" << endl;
    }
    seed48(elseed);
    g__pRNG_Initialized = true;
}


void jpw_math::statistics::init_rand_firstTimeOnly(bool verbose)
{
    if(!g__pRNG_Initialized) {
        init_rand(verbose);
    }
}


void jpw_math::statistics::init_rand(unsigned s1, unsigned s2, bool verbose)
{

    seed48v_t elseed;
    elseed[2] = static_cast<unsigned short>(s1 & 0xFFFF);
    elseed[1] = static_cast<unsigned short>((s2 & 0xFFFF0000) >> 16);
    elseed[0] = static_cast<unsigned short>(s2 & 0xFFFF);
    if (verbose) {
        cout << std::hex << "\t\t[Specified seed=0x"
             << elseed[2] << elseed[1] << elseed[0]
             << "]" << endl;
    }
    seed48(elseed);
    g__pRNG_Initialized = true;
}


double jpw_math::statistics::gaussrand(void)
{
    static int iset=1;
    register double u,v,rsq;
    double fac;
    static double gset=0.0;


    if(iset)
    {
        // Pick two uniform random #'s on the unit circle.
        do
        {
            u = 2.0*drand48() - 1.0;
            v = 2.0*drand48() - 1.0;
            rsq = u*u + v*v + jpw_math::TINY;
        }
        while(rsq >= 1.0);

        // Now calculate the pair of normal random #'s.
        fac = sqrt(-2.0*log(rsq)/rsq);
        gset = v*fac;
        iset = 0;

        return(u*fac);
    }
    else
    {
        iset = 1;
        return(gset);
    }
}


double jpw_math::statistics::std_dev(const dvector_t& values)
{
    double mean(0);
    double sum_sq(0);
    double total(values.size());

    dvector_t::const_iterator values_end = values.end();
    for(dvector_t::const_iterator
            vi = values.begin(); vi != values_end; ++vi) {
        mean += *vi;
    }
    mean /= total;

    for(dvector_t::const_iterator
            vi = values.begin(); vi != values_end; ++vi) {
        sum_sq += jpw_math::SQR((*vi) - mean);
    }

    return(sqrt(sum_sq/(total-1)));
}


double jpw_math::statistics::signif_del_chisq_tg(double& siglevel)
{
    static double sigvals[]={.5,
                             .68268949,
                             .8,
                             .9,
                             .95,
                             .95449974,
                             .96,
                             .98,
                             .99,
                             .99730020,
                             .9999};
    static double del_chisq_vals[]={0.45493642,
                                    1.0,
                                    1.6423744,
                                    2.7055435,
                                    3.8414588,
                                    4.0,
                                    4.2178846,
                                    5.4118944,
                                    6.6348966,
                                    9.0,
                                    15.136705};
    static index_t nsigs=sizeof(sigvals)/sizeof(double);  // Should be 11.
    index_t i, sigidx;

    // Assumes errors obey Gaussian statistics and uses theory for finding the
    // significance of a value of DeltaChi^2 for 1 parameter.
    if(siglevel < sigvals[0])
    {
        sigidx = 0;
    }
    else if(siglevel >= sigvals[nsigs-1])
    {
        sigidx = nsigs-1;
    }
    else
    {
        i=1;
        sigidx=-1;
        while(i<nsigs)
        {
            if( (sigvals[i-1] <= siglevel) && (siglevel < sigvals[i]) )
            {
                sigidx = i-1;
            }
            ++i;
        }
    }

    siglevel = sigvals[sigidx];
    return(del_chisq_vals[sigidx]);
}


double jpw_math::statistics::confidence_tg(double del_chisq_sig,
                                           const dmatrix_t& inv_curv_m,
                                           index_t parmidx, bool verbose)
{
    double inval;


    // At the minimum, we should ALWAYS be positive definite.
    inval = inv_curv_m[parmidx][parmidx];
    if(jpw_math::NEAR_ZERO(inval, -jpw_math::DBL_PREC, jpw_math::DBL_PREC))
    {
        inval = 0.0;
    }

    if(inval <= 0.0)
    {
        if( (inval == 0.0) && verbose )
        {
            cerr << "confidence_tg():  null value at param #" << parmidx
                 << endl;
        }
        else if(inval < 0.0)
        {
            cerr << scientific << setprecision(2)
                 << "confidence_tg():  negative value, inv["
                 << parmidx << "][" << parmidx
                 << "]=" << inval
                 << "  Minimization Failure?"
                 << reset << endl;
        }
        return(jpw_math::HUGE);
    }

    return(sqrt(del_chisq_sig*inval));
}


long jpw_math::statistics::nearest_multiple(double val,
                                            double nearfac,
                                            bool use_larger)
{
    double yy_l, yy_u, nufac_l, nufac_u;
    double iy_l, iy_u;
    double aval, udiff, ldiff;

    // Make sure our input values are nonzero integers.
    val = rint(val);
    aval = jpw_math::ABS(val);
    nearfac = rint(nearfac);
    if(jpw_math::ABS(nearfac) >= aval)
    {
        // Note that this includes the case val==0.0.
        return(static_cast<long>(nearfac));
    }
    else if(nearfac == 0.0)
    {
        return(1L); // 1 is the closest multiple to 0 of any number.
    }

    yy_l = val/nearfac;
    iy_l = rint(yy_l);
    // Is nearfac actually a multiple of val?
    if(yy_l == iy_l)
    {
        return(static_cast<long>(nearfac));
    }

    yy_u = yy_l;
    iy_u = iy_l;
    nufac_l = nufac_u = nearfac;
    // Find multiple just above nearfac.
    while( (yy_u != iy_u) && (jpw_math::ABS(nufac_u) < aval) )
    {
        ++nufac_u;
        yy_u = val/nufac_u;
        iy_u = rint(yy_u);
    }
    // Find multiple just below nearfac.
    while( (yy_l != iy_l) && (jpw_math::ABS(nufac_l) > 1.0) )
    {
        --nufac_l;
        yy_l = val/nufac_l;
        iy_l = rint(yy_l);
    }
    udiff = jpw_math::ABS(nufac_u-nearfac);
    ldiff = jpw_math::ABS(nearfac-nufac_l);

    // Now choose the one which is nearest to 'nearfac'.
    if(udiff > ldiff)
    {
        return(static_cast<long>(nufac_l));
    }
    else if(udiff < ldiff)
    {
        return(static_cast<long>(nufac_u));
    }
    else
    {
        // When it's equally spaced, flip up or down depending on the value of
        // the flag "use_larger"
        if(use_larger) {
            return(static_cast<long>(nufac_u));
        }
        else
        {
            return(static_cast<long>(nufac_l));
        }
    }
}


/////////////////////////
//
// End
