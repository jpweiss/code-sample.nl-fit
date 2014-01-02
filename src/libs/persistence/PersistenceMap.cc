// -*- C++ -*-
// Implementation of barrier model classes.
//
// Copyright (C) 2009 by John Weiss
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
PersistenceMap_cc__="RCS $Id: PersistenceMap.cc 1887 2009-10-13 20:05:57Z candide $";


// Includes
//
#include <iostream>
#include <string>
#include <stdexcept>
#include "PersistenceMap.h"

#include "details/Matrix.tcc"


using std::cout;
using std::cerr;
using std::flush;
using std::endl;
using std::string;

using jpw_math::Matrix;
using jpw_math::dmatrix_t;

using namespace jpw_nld;
using namespace jpw_nld::measure;

//
// Static variables
//


//
// Typedefs
//


/////////////////////////

//
// Class PersistenceMap::InconsistentDimensionsError
//



struct PersistenceMap::InconsistentDimensionsError
    : public std::length_error
{
    static string make_mesg(const char* const who,
                            const PersistenceMap& pmap,
                            const dmatrix_t& ts)
    {
        string errmsg("Fatal Error Invoking PersistenceMap::");
        errmsg += who;
        errmsg += "():  \n";
        if(pmap.nRows() != pmap.nColumns())
        {
            errmsg += "\tPersistenceMap instance is not square.\n";
        }
        if(ts.nColumns() != pmap.nColumns())
        {
            errmsg += "\tTimeseries data and PersistenceMap dimensions "
                "do not match.\n";
        }
        return errmsg;
    }

    explicit InconsistentDimensionsError(const char* const who,
                                         const PersistenceMap& pmap,
                                         const dmatrix_t& ts)
        : std::length_error(make_mesg(who, pmap, ts))
    {}
};


/////////////////////////

//
// PersistenceMap Member Functions
//


void PersistenceMap::fillAxes()
{
    // Phases are in the rows, lags are in the columns.
    // This may seem confusing due to how one normally plots a persistence
    // map, with the lags along the y-axis.  One normally associates the
    // y-axis with matrix columns.  In its unplotted form, however, the
    // persistence map is as described:  phases==rows, lags==columns.
    double dp = 1.0/static_cast<double>(m__map.nRows());
    double dl = 1.0/static_cast<double>(m__map.nColumns());

    // Let's make sure, while we're here, that 'm__phases' and 'm__lags' have
    // the correct dimensions.
    if( (m__map.nRows() != m__phases.nRows()) ||
         (m__map.nColumns() != m__phases.nColumns()) )
    {
        m__phases.clear(m__map.nRows(), m__map.nColumns());
    }
    if( (m__map.nRows() != m__lags.nRows()) ||
         (m__map.nColumns() != m__lags.nColumns()) )
    {
        m__lags.clear(m__map.nRows(), m__map.nColumns());
    }

    // Looking at the old SetupMapAxes() code, we find that:
    // 1. m__phases[i][j] is constant across 'j' and changes every 'i'
    // 2. m__lags[i][j] is constant across 'i' and changes every 'j'

    double p=0.0;
    for(tslen_t i=0; i<m__map.nRows(); ++i, p+=dp)
    {
        double l=0.0;
        for(tslen_t j=0; j<m__map.nColumns(); ++j, l+=dl)
        {
            m__phases[i][j] = p;
            m__lags[i][j] = l;
        }
    }
}


void PersistenceMap::computeCSStdDev(const dmatrix_t& ts_data, bool reset)
{
    if(hasBadDimensions(ts_data)) {
        throw InconsistentDimensionsError("computeCSStdDev", *this, ts_data);
    }

    if(reset) {
        m__computedCSStats = true;
        m__computedPersistence = true;
    }

    tslen_t nyrs(ts_data.nRows());
    tslen_t nphases(ts_data.nColumns());
    tslen_t nN;
    if(nyrs % 2) {
        nN = nyrs-1;
    } else {
        nN = nyrs-2;
    }

    m__cs_avg.wipe();
    m__cs_stddev.wipe();

    // perform the means
    tslen_t lag_offset;
    for(tslen_t n=1L; n <= nN; ++n)
    {
        for(tslen_t i=0; i<nphases; ++i)
        {
            for(tslen_t j=0; j<(i+1); ++j) {
                m__cs_avg[i][j] += ts_data[n][i-j];
            }

            lag_offset = (i + nphases);
            for(tslen_t j=(i+1); j<nphases; ++j) {
                m__cs_avg[i][j] += ts_data[n-1][lag_offset-j];
            }
        } // i
    } // n

    // calculate means from sums
    for(tslen_t i=0; i<nphases; ++i) {
        for(tslen_t j=0; j<nphases; ++j) {
            m__cs_avg[i][j] /= nN;
        }
    }

    // now perform the standard deviation
    for(tslen_t n=1L; n <= nN; ++n)
    {
        for(tslen_t i=0; i<nphases; ++i)
        {
            for(tslen_t j=0; j<(i+1); ++j) {
                m__cs_stddev[i][j] += jpw_math::SQR(ts_data[n][i-j]
                                                    - m__cs_avg[i][j]);
            }

            lag_offset = (i + nphases);
            for(tslen_t j=(i+1); j<nphases; ++j) {
                m__cs_stddev[i][j] += jpw_math::SQR(ts_data[n-1][lag_offset-j]
                                                    - m__cs_avg[i][j]);
            }
        } // i
    } // n

    // calculate means from sums
    for(tslen_t i=0; i<nphases; ++i) {
        for(tslen_t j=0; j<nphases; ++j) {
            m__cs_stddev[i][j] = sqrt( m__cs_stddev[i][j]/(nN-1) );
        }
    }

    m__computedCSStats = true;
}


void PersistenceMap::computePersistence(const dmatrix_t& ts_data, bool reset)
{
    if(hasBadDimensions(ts_data)) {
        throw InconsistentDimensionsError("computeCSStdDev", *this, ts_data);
    }

    if(reset) {
        m__computedCSStats = true;
        m__computedPersistence = true;
    }

    if(!m__computedCSStats) {
        computeCSStdDev(ts_data);
    }

    tslen_t nyrs(ts_data.nRows());
    tslen_t nphases(ts_data.nColumns());
    tslen_t nN;
    if(nyrs % 2) {
        nN = nyrs-1;
    } else {
        nN = nyrs-2;
    }

    m__map.wipe();

    // perform the correlations
    tslen_t lag_offset;
    double data_n_i_prime;
    for(tslen_t n=1L; n <= nN; ++n)
    {
        for(tslen_t i=0; i<nphases; ++i)
        {
            data_n_i_prime = (ts_data[n][i] - m__cs_avg[i][0]);
            for(tslen_t j=0; j<(i+1); ++j) {
                m__map[i][j] += (data_n_i_prime*(ts_data[n][i-j]
                                                 - m__cs_avg[i][j]));
            }

            lag_offset = (i + nphases);
            for(tslen_t j=(i+1); j<nphases; ++j) {
                m__map[i][j] += (data_n_i_prime*(ts_data[n-1][lag_offset-j]
                                                 - m__cs_avg[i][j]));
            }
        } // i
    } // n

    // now find the lags from the sums
    double denom;
    for(tslen_t i=0; i<nphases; ++i)
    {
        for(tslen_t j=0; j<nphases; ++j)
        {
            denom = m__cs_stddev[i][0]*m__cs_stddev[i][j];
            if(denom == 0.0) {
                m__map[i][j] = jpw_math::HUGE;
            } else {
                m__map[i][j] /= denom;
                m__map[i][j] /= (nN-1);
            }
        } // j
    } // i

    m__computedPersistence = true;
}


/////////////////////////
//
// End
