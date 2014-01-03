// -*- C++ -*-
// Header file jpw_nld - Common Namespace for the research-nld code.
//                       Contains commonly used typedefs, enums, and
//                       constants.
//
// Copyright (C) 2007 by John Weiss
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
// RCS $Id: jpw_nld.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _jpw_nld_H_
#define _jpw_nld_H_

// Includes
//
#include <stdint.h>


// Enclosing namespace
/**
 * The common namespace for all of my research code.
 */
namespace jpw_nld {

 /// Integer type for time values & indices.  Can be # phases or years.
 typedef unsigned long tslen_t;

 /// Integer type for numbers of steps in a monte-carlo run, integration,
 /// mapper run, etc.
 typedef unsigned long step_t;

 /// Integer type for the number of dimensions in a tensor, space, Sushi-file
 /// record, etc.
 typedef uint8_t dim_t;

 /// Integer type for array indices, lengths of dimensions in a Sushi-file,
 /// and similar.
 typedef uint32_t index_t;

}; //end namespace


#endif //_jpw_nld_H_
/////////////////////////
//
// End
