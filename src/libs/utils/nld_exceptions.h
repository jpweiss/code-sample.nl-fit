// -*- C++ -*-
// Header file nld_exceptions - Common exception classes, all bundled in the
//                              jpw_nld namespace.
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
// RCS $Id: nld_exceptions.h 1964 2010-09-05 02:24:07Z candide $
//
#ifndef _nld_exceptions_H_
#define _nld_exceptions_H_

// Includes
//
#include <string>
#include <stdexcept>


// Forward Declarations
//


// Enclosing namespace
/**
 * The common namespace for all of my research code.
 */
namespace jpw_nld {

 /// Exception class for generic file-open operations.
 /**
  * \todo
  * This class is redundant.  Replace it with the declaration of
  * \c jpw_nld::IO::FileNotFound.
  */
 struct FileNotFound : public std::runtime_error
 {
     explicit FileNotFound(const std::string& errmsg)
         : std::runtime_error(errmsg)
     {}
 };

 /// Exception class for function parameters with invalid values.
 struct InvalidArgError : public std::runtime_error
 {
     explicit InvalidArgError(const std::string& errmsg)
         : std::runtime_error(errmsg)
     {}
 };

 /// Exception class for invalid size parameters.
 struct SizeMismatchError : public InvalidArgError
 {
     explicit SizeMismatchError(const std::string& errmsg)
         : InvalidArgError(errmsg)
     {}
 };

#if 0 // Boilerplate for new runtime errors
 /// Exception class for
 /**
  * Thrown when ...
  */
 struct FooExcept : public std::runtime_error
 {
     explicit FooExcept(const std::string& what);
   private:
     std::string errMessage(const std::string& where,
                            const std::string& whatElse);
 };
#endif


}; //end namespace


#endif //_nld_exceptions_H_
/////////////////////////
//
// End
