// -*- C++ -*-
// Header file for namespace CustomManips
//
// Copyright (C) 2001-2007 by John Weiss
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
// RCS $Id: CustomManips.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _CUSTOM_MANIPS_H_
#define _CUSTOM_MANIPS_H_

// Includes
//
#include <iosfwd>


// Forward Declarations
//


// Special classes for custom manipulators.
//
namespace CustomManips {
 using namespace std;

 // Forward declarations
 template<class TArg, class TSt> class iosManip;
 template<class TArg>
 inline istream&
 operator>>(istream& i, const iosManip<TArg, istream>& m);
 template<class TArg>
 inline ostream&
 operator<<(ostream& o, const iosManip<TArg, ostream>& m);

 template <class TArg, class TSt> class iosManip {
     TSt& (*m__f)(TSt&, TArg);
     TArg m__a;
 public:
     iosManip(TSt& (*f)(TSt&, TArg), TArg a) : m__f(f), m__a(a) { }
     //
     friend
     istream& operator>> <>(istream& i, const iosManip<TArg, istream>& m);
     friend
     ostream& operator<< <>(ostream& o, const iosManip<TArg, ostream>& m);
 };

 template<class TArg>
 inline istream&
 operator>>(istream& i, const iosManip<TArg, istream>& m)
 { (*m.m__f)(i, m.m__a); return i; }

 template<class TArg>
 inline ostream&
 operator<<(ostream& o, const iosManip<TArg, ostream>& m)
 { (*m.m__f)(o, m.m__a); return o;}

 // Forward declarations
 template<class TArg, class TSt> class iosManipObj;
 template<class TObj>
 inline istream&
 operator>>(istream& i, const iosManipObj<TObj, istream>& m);
 template<class TObj>
 inline ostream&
 operator<<(ostream& o, const iosManipObj<TObj, ostream>& m);

 template <class TObj, class TSt> class iosManipObj {
     typedef TSt& (*TObj_FuncPtr_t)(TSt&, const TObj&);
     TObj_FuncPtr_t m__f;
     TObj m__Obj;
 public:
     iosManipObj(TObj_FuncPtr_t f, const TObj& obj) : m__f(f), m__Obj(obj) {}
     //
     friend
     istream& operator>> <>(istream& i,
                            const iosManipObj<TObj, istream>& m);
     friend
     ostream& operator<< <>(ostream& o,
                            const iosManipObj<TObj, ostream>& m);
 };

 template<class TObj>
 inline istream&
 operator>>(istream& i, const iosManipObj<TObj, istream>& m)
 { (*m.m__f)(i, m.m__Obj); return i; }

 template<class TObj>
 inline ostream&
 operator<<(ostream& o, const iosManipObj<TObj, ostream>& m)
 { (*m.m__f)(o, m.m__Obj); return o;}
};


#endif //_CUSTOM_MANIPS_H_
/////////////////////////
//
// End
