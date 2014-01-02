// -*- C++ -*-
// Header file for namespace inManips and outManips
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
// RCS $Id: Manips.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _MANIPS_H_
#define _MANIPS_H_

// Includes
//
#include "details/CustomManips.h"
// Included as a convenience.  Not really needed.
#include <iomanip>


// Forward Declarations
//


/// Namespace inManip
/**
 * Custom input manipulators.
 *
 * You must use the directive \c using \c namespace \c CustomManips; before
 * you can use these manipulators.  Otherwise, the compiler will give you
 * errors.
 */
namespace inManips {
 using namespace CustomManips;

 // Actual function
 istream& newln(istream& ist, bool skipSpace);
 /// Eat all newline characters.
 /**
  * Consumes all consecutive \c '\n' and \c '\r' characters in a stream.
  * If \a skipSpace \c == \c true, applies the \c ws manipulator to the
  * stream before stripping the \c '\n' and \c '\r' characters.
  */
 inline iosManip<bool, istream>
 newln(bool skipSpace=false) {
     return iosManip<bool, istream>(inManips::newln, skipSpace);
 }

};


/// Namespace outManips
/**
 * Custom output manipulators.
 *
 * You must use the directive \c using \c namespace \c CustomManips; before
 * you can use these manipulators.  Otherwise, the compiler will give you
 * errors.
 */
namespace outManips {
 using namespace CustomManips;

 /**
  *  Predefined time format strings for use in the \ref nowM "now(const
  * char*)" manipulator.
  */
 namespace timefmt {
  /// Format akin to timestamps in Unix syslogs.
  const char* const log="%h %d, %Y;  %T:    ";
  /// Format for use as a timestamp, such as in filenames.
  const char* const timestamp="%Y%m%d%H%M%S";
 };

 // Actual function
 ostream& now(ostream& ost, const char* fmt);

 /// Output current date \& time to stream.
 /** \anchor nowM
  * The string \a fmt specifies how to format the date \& time prior to
  * output.  See \c strftime(3) for details on what one can place in \a
  * fmt.
  */
 inline iosManip<const char*, ostream>
 now(const char* fmt=timefmt::log) {
     return iosManip<const char*, ostream>(outManips::now, fmt);
 }

 /// Reset floating point output format.
 /**
  * This manipulator reverses the effects of a \c scientific or \c fixed
  * manipulator.
  */
  inline ostream& general(ostream& ost)
  {
      ost.setf(ios_base::fmtflags(0), ios_base::floatfield);
      return ost;
  }

 /// Reset an output stream to its default format.
 /**
  * Resets \a ost to the defaults specified in the C++ Standard, section
  * 27.4.4.1.
  */
  inline ostream& reset(ostream& ost)
  {
      // Implementation Notes:
      // - Could be used for any child class of ios_base
      // - Don't bother implementing a "reseteol" function.  Saving those
      //   extra 4 characters doesn't help readability and just bloats the
      //   code.

      ost.flags(ios_base::dec | ios_base::skipws);
      ost.precision(6);
      ost.fill(ost.widen(' '));
      // Not really needed, but included for completeness.
      ost.width(0);
      return ost;
  }

};


#endif //_MANIPS_H_
/////////////////////////
//
// End
