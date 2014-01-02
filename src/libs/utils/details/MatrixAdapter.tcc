// -*- C++ -*-
// Implementation of class MatrixAdapter
//
// Copyright (C) 2005-2010 by John Weiss
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
//MatrixAdapter_cc__="RCS $Id: MatrixAdapter.tcc 1964 2010-09-05 02:24:07Z candide $";
#ifndef _MatrixAdapter_TCC_
#define _MatrixAdapter_TCC_


// Includes
//
#include <iostream>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/concept_check.hpp>
#include "nld_exceptions.h"
#include "details/SequenceStorage.h"


// Wrapper for parent namespace
namespace jpw_math {


// Convenience Macros for defining Template member functions.
//
#define TEMPL_M_MatrixAdapter \
    template<class S, \
             template<class> class STOR_POL>

#define CT_M_MatrixAdapter \
    MatrixAdapter<S, STOR_POL>


//
// Static variables
//


/////////////////////////

//
// General Function Definitions
//


template<class S, template <class> class P>
std::ostream& operator<<(std::ostream& ostr, const MatrixAdapter<S,P>& m)
{
    if (!m.nColumns()) {
        return ostr;
    } // else

    for(size_t i=0; i<m.nRows(); ++i) {
        ostr << m[i][0];
        for(size_t j=1; j<m.nColumns(); ++j) {
            ostr << ", " << m[i][j];
        }
        ostr << std::endl;
    }
    return ostr;
}


/////////////////////////

//
// MatrixAdapter Member Functions
//


// D'tor
TEMPL_M_MatrixAdapter
inline
CT_M_MatrixAdapter::~MatrixAdapter()
{
    storage_policy_t::destroy(m__data);
}


// Default C'tor
TEMPL_M_MatrixAdapter
inline
CT_M_MatrixAdapter::MatrixAdapter()
    : m__nrows(0)
    , m__ncols(0)
    , m__data()
{}


// Copy
TEMPL_M_MatrixAdapter
inline
CT_M_MatrixAdapter::MatrixAdapter(const MatrixAdapter& other)
    : m__nrows(other.m__nrows)
    , m__ncols(other.m__ncols)
    , m__data(storage_policy_t::new_copy_of(other.m__data, other.size()))
{}


// Standard
TEMPL_M_MatrixAdapter
inline
CT_M_MatrixAdapter::MatrixAdapter(size_t n_rows, size_t n_columns)
    : m__nrows(n_rows)
    , m__ncols(n_columns)
    , m__data(storage_policy_t::create(n_rows*n_columns))
{
    if(!m__nrows || !m__ncols) {
        throw jpw_nld::InvalidArgError("Cannot construct matrix with zero "
                                       "rows or columns");
    }
}


// Data-loading
TEMPL_M_MatrixAdapter
template<typename INPUT_ITERATOR>
CT_M_MatrixAdapter::MatrixAdapter(INPUT_ITERATOR begin, INPUT_ITERATOR end,
                                  size_t n_rows,
                                  size_t n_columns)
    : m__nrows(n_rows)
    , m__ncols(n_columns)
    , m__data(storage_policy_t::new_copy_of(begin, end))
{
    using namespace boost_concepts;
    using boost::function_requires;
    function_requires< ReadableIteratorConcept<INPUT_ITERATOR> >();

    if(!m__nrows || !m__ncols) {
        throw jpw_nld::InvalidArgError("Cannot construct matrix with zero "
                                       "rows or columns");
    }

    size_t data_size = end-begin;
    if(!data_size) {
        throw jpw_nld::InvalidArgError("Cannot construct matrix "
                                       "from empty vector");
    }

    if(!m__nrows && !m__ncols) {
        // Both n_rows and n_columns specified.
        // Check that there's enough data in v and that we don't slice
        if(data_size < m__ncols*m__nrows) {
            throw jpw_nld::InvalidArgError(
                "Matrix dimensions too large; "
                "insufficient data in the initialization vector");
        }
        // else
        if(data_size % m__ncols) {
            throw jpw_nld::InvalidArgError("Matrix dimensions do not match "
                                           "the size of the initialization "
                                           "vector");
        }
    }
}


TEMPL_M_MatrixAdapter
inline CT_M_MatrixAdapter&
CT_M_MatrixAdapter::operator=(const MatrixAdapter& other)
{
    m__nrows = other.m__nrows;
    m__ncols = other.m__ncols;
    storage_policy_t::assign(m__data, other.m__data, other.size());
    return *this;
}


//----------------------------------------


/// The cannonical swap member function.
TEMPL_M_MatrixAdapter
void CT_M_MatrixAdapter::swap(MatrixAdapter& other)
{
    if(&other == this) {
        return;
    }

    size_t tmp(other.m__nrows);
    other.m__nrows = m__nrows;
    m__nrows = tmp;
    tmp = other.m__ncols;
    other.m__ncols = m__ncols;
    m__ncols = tmp;

    storage_policy_t::swap(m__data, other.m__data);
}


/// Equivalence relation
TEMPL_M_MatrixAdapter
inline bool
CT_M_MatrixAdapter::operator==(const MatrixAdapter& other) const
{
    return ( (this == &other) ||
             ( (m__nrows == other.m__nrows) &&
               (m__ncols == other.m__ncols) &&
               (storage_policy_t::equivalent(m__data,
                                             other.m__data,
                                             other.size())) )
        );
}


// End namespace wrapper decls.
}; //end namespace jpw_math


#endif //_MatrixAdapter_TCC_
/////////////////////////
//
// End
