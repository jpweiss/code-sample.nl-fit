// -*- C++ -*-
// Header file for class RandomDataSrc
//
// Copyright (C) 2003-2011 by John Weiss
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
// $Id: RandomDataSrc.h 2215 2011-07-02 17:23:17Z candide $
//
#ifndef _RandomDataSrc_H_
#define _RandomDataSrc_H_

// Includes
//
#include <string>
#include <limits>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>


// Forward Declarations
//
#include <iosfwd>


// Enclosing namespace
//
namespace jpwTools
{
 /**
  * A namespace containing random number generator tools.
  *
  * At the moment, it contains the "rand_cast" function-templates.  I may move
  * RandomDataSrc into here at a later date, however.
  */
 namespace random
 {
  using std::string;
  using std::istream;
  using std::ostream;

  // Class RandomDataSrc
  /**
   * {Document it here}
   */
  class RandomDataSrc
  {
  public:
      // Forward Declarations:
      //
      struct PrintSeed;

      //------------------------------------------------------------

      /// Default Constructor
      RandomDataSrc();

      /// Destructor
      ~RandomDataSrc();

      /// Copy Constructor
      RandomDataSrc(const RandomDataSrc& other);

      /// Assignment Operator
      RandomDataSrc& operator=(const RandomDataSrc& other);

      /// Seed the internal RNG.
      void seed(unsigned long myseed);

      /// Retrieve the last seed used by the internal RNG
      unsigned long seed() const;

      /// Manipulator for printing out \c seed().
      /**
       * Prints \c seed in hex with a "0x" prefix.  It's used as follows:
       * \code
       * RandomDataSrc xi;
       * cout << "RNG Seed:  " << xi.print_seed() << endl;
       * \endcode
       */
      PrintSeed print_seed() { return PrintSeed(m__spImpl); }

      /// Parse and set the seed.
      /**
       * \param s
       * The string containing the "<tt>unsigned long</tt>" seed ... <b>and
       * only the seed</b>.  Parsing will fail if the string contains any
       * other text.  (Parsing also fails if \s wouldn't fit in an
       * "<tt>unsigned long</tt>".)
       * \param supportSlash
       * Set this to \c true if you want to alow the leading string, "\\x", to
       * indicate a hexadecimal number.  Otherwise, only "0x" indicates a
       * hexadecimal number.
       * \param hexOnly
       * When set to \c true, \a s will always be parsed as a hexadecimal
       * number, whether or not it contains a leading "0x" (or a leading "\\x"
       * if \a supportSlash is \c true))).<p>Otherwise, \s can contain a
       * hexadecimal, decimal, or even an octal value.  Hex values require a
       * leading "0x" (or "\\x"), while octal values always start with "\\0"
       * <em>regardless what \a supportSlash is set to</em>.</p>
       *
       * \return \c false if parsing fails, in which case the seed won't be
       * set.
       */
      bool parse_seed(const string& s,
                      bool supportSlash=false,
                      bool hexOnly=true) {
          ParseSeed parser(m__spImpl);
          return parser(s, supportSlash, hexOnly);
      }

      /// Overloaded version that can function as a manipulator.
      /**
       * Equivalent to the following code fragment:
       * \code
       * string seed;
       * iss >> seed;
       * if(!parse_seed(seed, true, true)) {
       *     // Set the failbit on 'iss'.
       * }
       * \endcode
       */
      istream& parse_seed(istream& iss) {
          ParseSeed parser(m__spImpl);
          return parser(iss);
      }

      /// Number generator
      unsigned long operator()();

      /// STL-like Uniform Random Number Generator
      /**
       * Models the STL's concept of a RNG.  Returns an integer in the range
       * [0, \a n).
       */
      long operator()(long n);

      /// Uniform Random Unit Interval
      double unit();

      /// Uniform Random Bit
      bool bit();

      /// Random Word
      string word(unsigned char length=5);

      /// Random Phrase/Sentence
      string text(unsigned length=80);

      /// Set the state of the internal RNG.
      void setState(istream& ins);

      /// Get the state of the internal RNG.
      string getState() const;

      //
      // Member classes
      //

      struct RNG_Impl;


      struct StateSaver
      {
          /// Save the former state of \a xi.
          /**
           * When the object is destroyed, the state of \a xi will be
           * restored.
           */
          StateSaver(RandomDataSrc& rnSrc);
          /// Restore the \c RandomDataSrc you passed to the c'tor to its
          /// former state.
          ~StateSaver();
      private:
          RandomDataSrc& m__randomDataSrc;
          string m__oldState;
          StateSaver& operator=(const StateSaver&);
      };


      /**
       * Utility Class.  Not for general use.
       */
      struct PrintSeed
      {
          /// Used by the \c print_seed() manipulator.  Performs the actual
          /// printing.
          PrintSeed(boost::shared_ptr<RNG_Impl>& rng_spImpl)
              : m__spImpl(rng_spImpl)
          {}
          /// Performs the actual parsing of an RNG seed.  See \c
          /// RandomDataSrc::parse_seed.
          ostream& operator()(ostream& ost) const;
      private:
          boost::shared_ptr<RNG_Impl> m__spImpl;
      };


      /**
       * Utility Class.  Not for general use.
       */
      struct ParseSeed
      {
          /// Used by \c parse_seed().
          ParseSeed(boost::shared_ptr<RNG_Impl>& rng_spImpl)
              : m__spImpl(rng_spImpl)
          {}
          /// Performs the actual printing of an RNG seed in hex.  See \c
          /// RandomDataSrc::print_seed.
          istream& operator()(istream& ist,
                              bool supportSlash=true,
                              bool hexOnly=true);
          /// Overloaded version.
          bool operator()(const string& s,
                          bool supportSlash=true,
                          bool hexOnly=true);
      private:
          /**
           * Inline function that performs the actual parsing and sets the
           * seed.
           *
           * The input string \a seedVal should contain \b only the seed, and
           * nothing else.
           */
          bool parseAndSet(const string& seedVal,
                           bool supportSlash,
                           bool hexOnly);
          boost::shared_ptr<RNG_Impl> m__spImpl;
      };


  private:
      boost::shared_ptr<RNG_Impl> m__spImpl;
  };


  /// Used by the Manipulator \c RandomDataSrc::print_seed()
  inline ostream& operator<<(ostream& outs,
                             const RandomDataSrc::PrintSeed& seedFunctor)
  {
      return seedFunctor(outs);
  }


  namespace details
  {
   // Helper functions & classes for the rand_cast function templates defined
   // below.

   template<typename T> inline T
   rand_cast_floating_point_helper(RandomDataSrc& xi)
   {
       // Note:  numeric_limits<T>::min() is the smallest positive nonzero
       // number for floating point data types.
       typedef typename std::numeric_limits<T> limits_t;

       // Beware of overflow when performing this computation.
       return (2.*xi.unit() - 1.)*limits_t::max();
   }

   // For integral types smaller than 'long', generate a random number twice
   // the size of 'long', then subtract the size of 'long'.
   template<typename T, unsigned SZ_T, unsigned SZ_LONG>
   struct RandCastIntegralHelper
   {
       inline static T convert(RandomDataSrc& xi)
       {
           BOOST_STATIC_ASSERT(SZ_T < SZ_LONG);
           return static_cast<T>( xi(1L << (8*SZ_T)) - (2L << (8*SZ_T)) );
       }
   };

   // Specialization:  (unsigned) long long with 4-byte long:
   //
   // Use two 32-bit random numbers to construct the 64-bit
   // unsigned random number.
   template<typename T>
   struct RandCastIntegralHelper<T, 8, 4>
   {
       inline static T convert(RandomDataSrc& xi)
       {
           return ( (static_cast<T>(xi()) << 32) | static_cast<T>(xi()) );
       }
   };

   // Specialization:  T has the same size as long:
   //
   // Just invoke \c xi(), statically cast to the appropriate type.
   template<typename T>
   struct RandCastIntegralHelper<T, sizeof(long), sizeof(long)>
   {
       inline static T convert(RandomDataSrc& xi)
       {
           return static_cast<T>(xi());
       }
   };

  }; // end namespace details


  /// "Cast" a \c RandomDataSrc to a random number of type \c T.
  /**
   * This function template "casts" an instance of \c RandomDataSrc to a
   * random number by calling one of the member functions in \c
   * RandomDataSrc.
   *
   * Mathematically, random variables don't have a
   * specific value, by definition.  They only have a probability
   * distribution.  A sequence of "random" values from a random variable are
   * actually "samples" of the random variable, \em not its value.
   *
   * So, conceptually, you can think of a \c RandomDataSrc object as a
   * (pseudo)random variable, and of this "cast" operator as "sampling" the
   * random variable.
   *
   * This generic function template is intended for use with integral types.
   * For most of the integral types, it merely calls \c
   * RandomDataSrc::operator()().  It won't work, however, for integral types
   * the same size as or larger than \c long.
   *
   * On platforms where \c long is 64-bits, this function will return \c xi()
   * for the \c long \c long types.  For platforms with a 32-bit \c long, this
   * function will return \c xi() for \c int and \c unsigned.  For the \c long
   * \c long types, it uses 2 random integers, or-ed togeter, for upper and
   * lower 32-bits.  (If this bit-manipulation provides insufficient
   * randomness for you, consider directly using \c boost::uniform_int, which
   * will convert any integral pRNG to any sized integer without
   * side-effects).
   *
   * Don't bother using this with \c T \c = \c unsigned \c long.  It's
   * superfluous, since \c RandomDataSrc::operator()() already returns an \c
   * unsigned \c long.
   */
  template<typename T> inline T rand_cast(RandomDataSrc& xi)
  {
      typedef typename
          details::RandCastIntegralHelper<T, sizeof(T), sizeof(long)>
          int_rc_helper_t;

      return int_rc_helper_t::convert(xi);
  }


  /// "Cast" a \c RandomDataSrc to a random \c bool.
  /**
   * Full-specialization of the function template \c rand_cast for type \c
   * bool.
   *
   * Simple wrapper around \c RandomDataSrc::bit().
   */
  template<> inline bool rand_cast<bool>(RandomDataSrc& xi)
  {
      return xi.bit();
  }


  /// "Cast" a \c RandomDataSrc to a random \c long.
  /**
   * Full-specialization of the function template \c rand_cast for type \c
   * long.
   *
   * Returns \c RandomDataSrc::op()() statically cast to \c long.
   */
  template<> inline long rand_cast<long>(RandomDataSrc& xi)
  {
      return static_cast<long>(xi());
  }


  /// "Cast" a \c RandomDataSrc to a random \c float.
  /**
   * Full-specialization of the function template \c rand_cast for type \c
   * float.
   *
   * Multiplies <tt>(2*xi.unit()-1)</tt> by the maximum possible value of \c
   * float.
   */
  template<> inline float rand_cast<float>(RandomDataSrc& xi)
  {
      using details::rand_cast_floating_point_helper;

      return details::rand_cast_floating_point_helper<float>(xi);
  }


  /// "Cast" a \c RandomDataSrc to a random \c double.
  /**
   * Full-specialization of the function template \c rand_cast for type \c
   * double.
   *
   * Multiplies <tt>(2*xi.unit()-1)</tt> by the maximum possible value of \c
   * double.
   */
  template<> inline double rand_cast<double>(RandomDataSrc& xi)
  {
      using details::rand_cast_floating_point_helper;

      return details::rand_cast_floating_point_helper<double>(xi);
  }


  /// "Cast" a \c RandomDataSrc to a random \c long \c double.
  /**
   * Full-specialization of the function template \c rand_cast for type \c
   * long \c double.
   *
   * Multiplies <tt>(2*xi.unit()-1)</tt> by the maximum possible value of \c
   * long \c double.
   */
  template<> inline long double rand_cast<long double>(RandomDataSrc& xi)
  {
      using details::rand_cast_floating_point_helper;

      return details::rand_cast_floating_point_helper<long double>(xi);
  }


 }; //end namespace random
}; //end namespace jpwTools


#endif //_RandomDataSrc_H_
/////////////////////////
//
// End
