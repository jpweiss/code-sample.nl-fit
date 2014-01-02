// -*- C++ -*-
// Header file for template class Vector_fwd
//
// Copyright (C) 2005-7 by John Weiss
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
// RCS $Id: Vector_fwd.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _Vector_fwd_H_
#define _Vector_fwd_H_

// Includes
//


// Forward Declarations
//
namespace std {

 // Required for the typedef, below.
 template<class C> class allocator;
 template<class C, class A> class vector;

}; //end namespace

namespace jpw_math {

 // Alas, we have to specify both template args in the fwd. declaration.
 typedef std::vector<double, std::allocator<double> > dvector_t;

}; //end namespace


#endif //_Vector_fwd_H_
/////////////////////////
//
// End
