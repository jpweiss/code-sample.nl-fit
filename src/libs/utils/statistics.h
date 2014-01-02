// -*- C++ -*-
// Header file for functions in namespace "statistics"
//
// Copyright (C) 2009-2010
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
// RCS $Id: statistics.h 1905 2010-02-19 18:59:22Z candide $
//
#ifndef _statistics_H_
#define _statistics_H_

// Includes
//
#include "jpw_nld.h"
#include "Matrix.h"


// Forward Declarations
//
double drand48();
namespace std {
 template<typename _Alloc>
 class allocator;
 template<class _CharT>
 struct char_traits;
 template<typename _CharT, typename _Traits, typename _Alloc>
 class basic_string;
 template<> struct char_traits<char>;
 typedef basic_string<char>    string;
};


// Enclosing namespace
//
namespace jpw_math {
 namespace statistics {
  // Using decls.
  //
  using jpw_nld::index_t;


  /// Parses a seed for the *rand48() pRNG.
  void parseSeed(std::string& seedStr, unsigned& seedLo, unsigned& seedHi);

  /// Seeds the *rand48() pRNG functions with the current time.
  void init_rand(bool verbose=true);

  /// Seeds the *rand48() pRNG functions with the current time, but only if it
  /// hasn't been done already.
  void init_rand_firstTimeOnly(bool verbose=true);

  /// Overloaded version that takes a seed as 2 unsigned integers.
  /**
   * The seed should be, at minimum, a 48-bit number.
   *
   * \param s1
   * The upper 16 bits of the seed, stored in the first 2 bytes of \a s1.  In
   * other words, treat this parameter as if it's an \c unsigned \c short.
   *
   * \param s2
   * The lower 32 bits of the seed.
   */
  void init_rand(unsigned s1, unsigned s2, bool verbose=true);

  /// Returns a random index from 0 to nn-1
  inline index_t randidx(index_t nn)
  {
      return( static_cast<index_t>(drand48()*nn) );
  }

  /// Returns a uniform random number in the range [-del,del]
  inline double range_rand(double del)
  {
      return(del*(2.0*drand48() - 1.0));
  }


  /// Returns a Gaussian random number with a std. deviation of 1
  double gaussrand();

  /// Computes the standard deviation of the vector.
  double std_dev(const dvector_t& values);

  /// Computes the standard deviation of the cells in a Matrix.
  inline double std_dev(const dmatrix_t& values)
  {
      return(std_dev(values.as_1D()));
  }

  /// Returns the change in a Chi^2 distribution corresponding to the
  /// specified percent significance level.
  double signif_del_chisq_tg(double& siglevel);

  /// Finds the confidence intervals for a least squares fit.
  /**
   * It's only correct assuming Gaussian errors or for a linear model.
   * Otherwise, consider it only a rough estimate.
   */
  double confidence_tg(double del_chisq_sig,
                       const dmatrix_t& inv_curv_m,
                       index_t parmidx, bool verbose);

  /// Finds the nearest multiple of a number.
  /**
   * \a use_larger indicates which multiple to return when \a nearfac falls
   * exactly halfway between two multiples.
   */
  long nearest_multiple(double val, double nearfac, bool use_larger);

 }; //end namespace statistics
}; //end namespace jpw_math


#endif //_statistics_H_
/////////////////////////
//
// End
