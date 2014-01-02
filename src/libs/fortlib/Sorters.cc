// -*- C++ -*-
// Implementation of: jpw_nld::fortlib::sort
//
// Copyright (C) 2007-2008 by John Weiss
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
Sorters_cc__="RCS $Id: Sorters.cc 1864 2009-08-18 02:50:20Z candide $";


// Includes
//
#include <string>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "Matrix.h"

#include "details/Matrix.tcc"

#include "FORTLib.h"


//
// Using Decls.
//
using std::string;
using std::runtime_error;
using std::length_error;
using boost::lexical_cast;

using namespace jpw_nld::fortlib;


//
// Static variables
//


//
// Typedefs
//


/////////////////////////


//
// External FORTRAN Functions
//


#ifdef __cplusplus
extern "C" {
#endif
    extern void dpsort_(double *, int *, int *, int *, int *);
#ifdef __cplusplus
}; /* end extern "C" */
#endif


/////////////////////////

//
// Exception Class Member Functions
//

inline string
sort::SortError::errMessage(const string& where,
                            const string& fortFn, int errCode)
{
    string mesg("While executing ");
    mesg += where;
    mesg += "():\n\tFatal error in FORTRAN subroutine ";
    mesg += fortFn;
    mesg += "() - Error #";
    mesg += lexical_cast<string>(errCode);
    return mesg;
}


sort::SortError::SortError(const string& where,
                           const string& fortFn, int errCode)
    : runtime_error(errMessage(where, fortFn, errCode))
{}


/////////////////////////

//
// General Function Definitions
//


/* Returns the list of sorted indices in the provided array without
   altering the original list of values. */
void sort::byIndex(const dvector_t& dlst, ivector_t& idxarr, int dir)
{
    int kflag(1);
    if(dir < 0) {
        kflag = -1;
    }

    int err(0);
    int nn(dlst.size());
    std::fill(idxarr.begin(), idxarr.end(), 0);
    dpsort_(const_cast<double*>(&dlst[0]), &nn, &idxarr[0], &kflag, &err);

    if(err) {
        throw SortError("jpw_nld::fortlib::sort::byIndex", "dpsort", err);
    }
}


/////////////////////////
//
// End
