// -*- C++ -*-
// Header file TestException.h
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
// RCS $Id: TestException.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _TestException_H_
#define _TestException_H_

// Includes
//
#include <string>
#include <stdexcept>
#include <boost/test/minimal.hpp>


//
// Macros
//


/// Test that a function call throws an exception.
/// \a \c TargExceptT is the expected exception.
/// \a code is any function call or block of code to run.
/// If \a code doesn't throw \a TargExceptT, \c BOOST_ERROR is called with an
/// appropriate error message.
#define JPW_TEST_EXCEPTION(TargExceptT,code)                            \
    try {                                                               \
        code;                                                           \
        BOOST_ERROR(#TargExceptT" not thrown");                         \
    } catch(TargExceptT&) {                                             \
    } catch(std::exception& ex) {                                       \
        std::string errmesg("Exception other than " #TargExceptT        \
                            " thrown.  Reason:\n");                     \
        errmesg += ex.what();                                           \
        BOOST_ERROR(errmesg.c_str());                                   \
    } catch(...) {                                                      \
        BOOST_ERROR("Exception other than " #TargExceptT " thrown");    \
    }


#endif //_TestException_H_
/////////////////////////
//
// End
