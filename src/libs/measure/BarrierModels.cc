// -*- C++ -*-
// Implementation of barrier model classes.
//
// Copyright (C) 2008-2010 by John Weiss
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
BarrierModels_cc__="RCS $Id: BarrierModels.cc 1906 2010-03-03 20:32:43Z candide $";


// Includes
//
#include "BarrierModels.h"
#include "details/BarrierModels.tcc"


//
// Static variables
//


//
// Typedefs
//


// Wrappers for parent namespaces
namespace jpw_nld {
namespace measure {


/////////////////////////


//
// Explicit Template Instantiations
//


template class BarrierModel<policy::Full>;
template class BarrierModel<policy::MarkovOnly>;
template class BarrierModel<policy::BarrierOnly>;


// End namespace wrapper decls.
}; // end namespace measure
}; // end namespace jpw_nld


/////////////////////////
//
// End
