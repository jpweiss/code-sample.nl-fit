// -*- C++ -*-
// Header file for standalone functions in the jpw_math namespace.
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
// RCS $Id: MathTools.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _MathTools_H_
#define _MathTools_H_

// Includes
//
#include <cmath>      // For M_PI and M_PI_2
#include "jpw_nld.h"


// Enclosing namespace
//
namespace jpw_math {

 static const double DBL_PREC(1.0e-16);
 static const double FLT_PREC(1.0e-8);
 // NOTE:  If we replace the pRNGs in statistics.c with the boost ones, we can
 //        probably remove this.
#ifdef TINY
#undef TINY
#endif
 static const double TINY(1.0e-30);
#ifdef HUGE
#undef HUGE
#endif
 static const double HUGE(1.0e30);
 // FIXME:  Used several places.  Leave be.
#ifdef TWOPI
#undef TWOPI
#endif
 static const double TWOPI(2.0*M_PI);


 template<typename T>
 inline T
 SQR(T _a) { return (_a*_a); }

 // FIXME:  Unused, but kept for completion
 template<typename T>
 inline T
 CUBE(T _a) { return (_a*_a*_a); }

 template<typename T>
 inline T
 QUAD(T _a) { return (_a*_a*_a*_a); }

 template<typename T>
 inline T
 MAX(T _a, T _b) { return (_a > _b ? _a : _b); }

 // FIXME:  Unused, but keep for now.
 template<typename T>
 inline T
 MIN(T _a, T _b) { return (_a < _b ? _a : _b); }

#ifdef ABS
#undef ABS
#endif
 template<typename T>
 inline T
 ABS(T _x) { return (_x < 0.0 ? -_x : _x); }

 // A modulo-1 operator for floating point numbers.
 template<typename T>
 inline T
 MOD_1(T _a) { return ( _a - static_cast<long>(_a) ); }

 template<typename T>
 inline void
 CHK_ABSMAX(T _v, T& _max)
 {
     T _av = (_v < 0.0 ? -_v : _v);
     if(_av > _max) { _max = _v; }
 }

 template<typename T>
 inline bool
 NEAR_ZERO(T _x, T _a, T _b) { return (( _a  <= _x )&&( _x <= _b )); }

 template<typename T>
 inline T
 ORDEROF(T _x) { return (pow(10.0, rint(log10(_x)))); }

 template<typename T>
 inline bool
 INQUAD1(T a) { return ((0.0 <= a) && (a < M_PI_2)); }

 // FIXME:  Unused, but keep for completeness.
 template<typename T>
 inline bool
 INQUAD2(T a) { return ((M_PI_2 <= a) && (a < M_PI)); }

 // FIXME:  Unused, but keep for completeness.
 template<typename T>
 inline bool
 INQUAD3(T a) { return ((M_PI <= a) && (a < 3*M_PI_2)); }

 template<typename T>
 inline bool
 INQUAD4(T a) { return ((3*M_PI_2 <= a) && (a <= TWOPI)); }


 /// Compute \f$\chi^2\f$ from \a fvec
 /**
  * The template parameter, \c C, must have minimal STL-container semantics.
  *
  * NOTE:  We need the \c typename keyword to inform the compiler that
  * \c C::value_type is a member type of the template parameter, \c C.
  */
 template<typename C>
 inline typename C::value_type
 chiSquared(const C& fvec)
 {
     typename C::value_type out=0;
     for(jpw_nld::tslen_t i=0; i<fvec.size(); ++i) {
         out += jpw_math::SQR(fvec[i]);
     }
     return(out);
 }

}; //end namespace jpw_math


#endif //_MathTools_H_
/////////////////////////
//
// End
