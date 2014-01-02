// -*- C++ -*-
// Implementation of "manips.h" functions
//
// Copyright (C) 2001-2004 by John Weiss
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
Id__="RCS $Id: Manips.cc 1864 2009-08-18 02:50:20Z candide $";


// Includes
//
#include <iterator>
#include <ctime>
#include <iostream>

#include "Manips.h"


using namespace std;
using namespace CustomManips;


// Static variables
//


/////////////////////////

//
// Namespace "inManips" Function Definitions
//


istream&
inManips::newln(istream& ist, bool skipSpace) {
    if(skipSpace) {
        ist >> ws;
    }
    // Strips all isolated \r and \n, removing not only line terminators, but
    // blank lines, as well.
    char c=ist.peek();
    while((c == '\n') || (c == '\r')) {
        // Don't use "<<"; it'll skip blank lines.
        ist.get();
        c=ist.peek();
    }
    return ist;
}


/////////////////////////

//
// Namespace "outManips" Function Definitions
//


ostream&
outManips::now(ostream& ost, const char* fmt) {
    char out[1024];
    time_t currentTime = time(NULL);
    strftime(out, sizeof(out)-1, fmt, localtime(&currentTime));
    ost << out;
    return ost;
}



/////////////////////////
//
// End
