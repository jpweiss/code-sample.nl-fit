// -*- C++ -*-
// Header file for types in namespace jpw_nld::fortlib.
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
// RCS $Id: FORTTypes.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _FORTTypes_H_
#define _FORTTypes_H_

// Includes
//
#include <string>
#include "jpw_nld.h"
#include "Vector_fwd.h"
#include "Matrix_fwd.h"


// Forward Declarations
//


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

  /// FORTRAN integer scalar type
  typedef int* fort_ivar_t;

  /// FORTRAN double-array type
  typedef double* fort_dvar_t;

  /// FORTRAN integer-array type
  typedef int* fort_ivec_t;

  /// FORTRAN double-array type
  typedef double* fort_dvec_t;

  /// FORTRAN double-matrix type
  typedef double* fort_dmat_t;

 }; //end namespace
}; //end namespace


#endif //_FORTTypes_H_
/////////////////////////
//
// End
