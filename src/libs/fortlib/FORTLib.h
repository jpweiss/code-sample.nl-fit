// -*- C++ -*-
// Header file for independent functions in namespace jpw_nld::fortlib.
//
// Copyright (C) 2007-2009 by John Weiss
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
// RCS $Id: FORTLib.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _FORTLib_H_
#define _FORTLib_H_

// Includes
//
#include <stdexcept>
#include "FORTTypes.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace fortlib {
  // Using decls.
  //
  using std::string;
  using jpw_math::dvector_t;
  using jpw_math::dmatrix_t;
  typedef std::vector<int, std::allocator<int> > ivector_t;

  /// Functions for inverting square matrices.
  namespace square_matrix_inversion
  {
   /// Invert a square matrix using Gauss-Jordan Elimination.
   /**
    * \param dm
    * The square matrix to invert.
    *
    * \param dm_inv
    * The resulting inverse matrix.
   int sqmInvert_GJE(const dmatrix_t& dm, dmatrix_t& dm_inv);
    */

   /// Invert a square matrix using Singular-Value Decomposition.
   /**
    * \param dm
    * The matrix to invert.
    *
    * \param dm_inv
    * The resulting inverse matrix.
   int sqmInvert_SVD(const dmatrix_t& dm, dmatrix_t& dm_inv);
    */
  };

  /// Numeric sorting functions.
  namespace sort
  {
   /**
    * Exception thrown due to errors in the internal FORTRAN sort algorithm.
    */
   struct SortError : public std::runtime_error
   {
       SortError(const string& where, const string& fortFn, int errCode);
   private:
       string errMessage(const string& where,
                         const string& fortFn, int errCode);
   };

   /// Sort a list non-destructively, returning a list of sorted indices.
   /**
    * \param dlst
    * The vector to sort.
    *
    * \param idxarr
    * The list to return the results in.  Iterating over \c
    * dlst[idxarr[i]] will return the values of \c dlst in sorted order.
    *
    * \param dir
    * The sort direction.  A negative number sorts in descending order.  A
    * non-negative value performs an ascending sort.
    */
   void byIndex(const dvector_t& dlst, ivector_t& idxarr, int dir);
  };

 }; //end namespace
}; //end namespace


#endif //_FORTLib_H_
/////////////////////////
//
// End
