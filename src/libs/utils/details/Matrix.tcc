// -*- C++ -*-
// Implementation of class Matrix
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
//Matrix_cc__="RCS $Id: Matrix.tcc 1905 2010-02-19 18:59:22Z candide $";
#ifndef _Matrix_TCC_
#define _Matrix_TCC_


// Includes
//
#include "nld_exceptions.h"

// Anything including this impl-file will also need the implementation of
// MatrixAdapter, so pull it in, too.
#include "details/MatrixAdapter.tcc"


// Wrapper for parent namespace
namespace jpw_math {


// Convenience Macros for defining Template member functions.
//
#define TEMPL_M_Matrix template<typename C>

#define CT_M_Matrix Matrix<C>


//
// Static variables
//


/////////////////////////

//
// General Function Definitions
//


/////////////////////////

//
// Matrix Member Functions
//


TEMPL_M_Matrix
inline
CT_M_Matrix::Matrix(size_t n_rows, size_t n_columns)
    : Base_t(n_rows, n_columns)
{}


TEMPL_M_Matrix
inline
CT_M_Matrix::Matrix(size_t n_rows, size_t n_columns, const value_type& value)
    : Base_t(n_rows, n_columns)
{
    iterator end = m__data.end();
    for(iterator iter=m__data.begin(); iter != end; ++iter) {
        *iter = value;
    }
}


TEMPL_M_Matrix
inline
CT_M_Matrix::Matrix(const vector_type& v,
                    size_t n_rows,
                    size_t n_columns)
    : Base_t(v.begin(), v.end(), n_rows, n_columns)
{}


TEMPL_M_Matrix
inline CT_M_Matrix&
CT_M_Matrix::operator=(const Matrix& other)
{
    Base_t::operator=(other);
    return *this;
}


TEMPL_M_Matrix
inline typename CT_M_Matrix::reference
CT_M_Matrix::at(size_t row_idx, size_t column_idx)
{
    return m__data.at(row_idx*m__ncols+column_idx);
}


TEMPL_M_Matrix
inline typename CT_M_Matrix::const_reference
CT_M_Matrix::at(size_t row_idx, size_t column_idx) const
{
    return m__data.at(row_idx*m__ncols+column_idx);
}


TEMPL_M_Matrix
inline void
CT_M_Matrix::swap(vector_type& other)
{
    if (other.size() != m__data.size()) {
        throw jpw_nld::InvalidArgError("Error:  Matrix<>::swap(): "
                                       "vector<> arg must have the same "
                                       "number of elements as\n"
                                       "the target Matrix.");
    }
    m__data.swap(other);
}


TEMPL_M_Matrix
inline void
CT_M_Matrix::reserve(size_t n_rows, size_t n_columns)
{
    size_t n_elements_new(n_rows*n_columns);
    if(n_elements_new) {
        m__nrows = n_rows;
        m__ncols = n_columns;
        if(m__data.size() < n_elements_new) {
            // The _size_ of the underlying vector must always be
            // nRows*nColumns, even if we reserve extra space.
            m__data.reserve(n_elements_new+1);
            m__data.resize(n_elements_new);
        }
    }
}


TEMPL_M_Matrix
inline void
CT_M_Matrix::clear(size_t n_rows, size_t n_columns)
{
    m__data.clear();
    reserve(n_rows, n_columns);
}


TEMPL_M_Matrix
inline void
CT_M_Matrix::fill(value_type v, size_t n_rows, size_t n_columns)
{
    if( (n_rows != m__nrows) || (n_columns != m__ncols)) {
        reserve(n_rows, n_columns);
    }
    m__data.assign(m__nrows*m__ncols, v);
}


TEMPL_M_Matrix
inline bool
CT_M_Matrix::equivalent(const Matrix& other, double epsilon) const
{
    if((m__nrows != other.m__nrows) || (m__ncols != other.m__ncols)) {
        return false;
    }
    if (epsilon < 0) {
        epsilon = -epsilon;
    }

    size_t commonLength(m__data.size());
    for(size_t i=0; i<commonLength; ++i) {
        double delta(other.m__data[i]-m__data[i]);
        if((delta < -epsilon) || (epsilon < delta)) {
            return false;
        }
    }

    return true;
}


// End namespace wrapper decls.
}; //end namespace jpw_math


#endif //_Matrix_TCC_
/////////////////////////
//
// End
