// -*- C++ -*-
// Header file for template class FitGA
//
// Copyright (C) 2008-2009 by John Weiss
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
// RCS $Id: FitGA.h 2221 2011-07-09 01:56:38Z candide $
//
#ifndef _FitGA_H_
#define _FitGA_H_

/* Notes:
 * These must remain preprocessor flags.
#define DEBUG 1
#define GA_TESTING 1
*/


// Includes
//
#include <boost/utility.hpp>
#include "jpw_nld.h"
#include "Vector_fwd.h"
#include "statistics.h"  // for 'init_rand()'


// Forward Declarations
// Requires <vector> or "Vector_fwd.h"
//
namespace jpw_nld {
 namespace fortlib {
  typedef std::vector<int, std::allocator<int> > ivector_t;
 };
};


// Enclosing namespace
//
namespace jpw_nld {
 namespace optimize {
  // Using decls.
  //
  using std::vector;
  using jpw_math::dvector_t;


  // Class FitGA
  /**
   * Optimize a function via Genetic Algorithms.
   *
   * The template parameter \a FIT_FN must provide the following public
   * member constants and functions:
   * - <tt>static const index_t N_PARAMETERS<br/>
   * The number of tunable parameters in the fit function.  This is the vector
   * whose value you're trying to find by minimization.
   * - <tt>double chiSquared(const DATA_T\& data,
   * const dvector_t\& params)</tt><br/>
   * Compute and return the \chi^2 error between the data and your function,
   * evaluated at the parameters \a params.
   * - <tt>static void randomParams(dvector_t\& params)</tt><br/>
   * Fill the \c dvector_t with "constrained" random values, based on the
   * model.  The "constrained" random values should be uniformly
   * distributed over a range sensible for each parameter.  (E.g. if parameter
   * \#2 can't be smaller than -3, don't use an aribtrary random number.)
   * - <tt>static void limitParams(dvector_t\& params)</tt><br/>
   * Make sure that the parameters in the \c dvector_t aren't out of range.
   * Adjust appropriately if any are.<br/>
   * If the parameters in your function have no bounds, just define this with
   * an empty body.
   *
   * You'll notice that the \a FIT_FN class doesn't need to provide the
   * function itself.  This is by design, permitting \a FIT_FN to be
   * compatible with the FORTRAN-based Levenberg-Marquardt minimizer, \c
   * FitLM.
   *
   * This class makes use of the \c drand48() pRNG.  Call
   * jpw_math::statistics::init_rand() before using this class.
   */
  template<class FIT_FN,
           class DATA_T,
           unsigned POPULATION_SIZE=40,
           unsigned N_GENERATIONS=100,
           unsigned N_MUTATE=static_cast<unsigned>(POPULATION_SIZE*0.3),
           unsigned N_ELITE=3,
           unsigned WIDE_XOVER_WEIGHT_X_1000=1490>
  class FitGA : private boost::noncopyable
  {
      // Forward Decl.
      class PopVecFiller;

  public:
      typedef FIT_FN Model_t;
      typedef DATA_T FitData_t;
      typedef vector<dvector_t> popVec_t;

      static const index_t N_PARAMETERS=Model_t::N_PARAMETERS;
      /// The size of the initial population.
      /// It's larger than our working size to give us a good group from which
      /// to select some highly-fit initial members.
      static const
      unsigned INITIAL_POPULATION_SIZE=(5*POPULATION_SIZE);
      static const unsigned N_NONELITE=(POPULATION_SIZE-N_ELITE);
      /// This used to be tunable, but changing it didn't have much effect.
      /// So, it's now fixed at 50% of the population.
      static const unsigned N_BREEDING=(POPULATION_SIZE/2);
      /// Widened crossover weight
      /**
       * After about 10 generations, we start to get convergence \<==\>
       * inbreeeding.  To prevent this, we use a wider random weight in
       * crossover, a weight which should be in the interval [1.0,1.5].
       */
      static const double WIDE_XOVER_WEIGHT;

      /// Default Constructor
      FitGA()
          : m__blankParamVec(N_PARAMETERS, 0.0)
          , m__wrkPop0(INITIAL_POPULATION_SIZE, m__blankParamVec)
          , m__wrkChiPop0(INITIAL_POPULATION_SIZE)
          , m__wrkMutantDNA(N_PARAMETERS)
          , m__wrkSortPop0(INITIAL_POPULATION_SIZE)
          , m__wrkSortChiSq(POPULATION_SIZE)
          , m__LastParamPop(POPULATION_SIZE+2L, m__blankParamVec)
          , m__NextParamPop(POPULATION_SIZE+2L, m__blankParamVec)
          , m__ChiPop(POPULATION_SIZE)
          , m__ChiRating(POPULATION_SIZE)
          , m__zeroingFunctor()
      {}

      /// Destructor
      ~FitGA() {}

      /// Performs the minimization.
      /**
       * Returns the value of Chi^2 for the minimized parameters.
       */
      double operator()(dvector_t& param_min, const FitData_t& theData,
                        Model_t& theModel);

  protected:
      /// Create the initial population.
      /**
       * This function uses \a theData and \a theModel to sort \a ppop in
       * order of increasing fitness.
       *
       * \a theModel is non-<tt>const</tt> because it contains internal
       * bookkeeping members that change during the fit.
       */
      void initializePopulation(popVec_t& ppop,
                                const FitData_t& theData,
                                Model_t& theModel);

      /// Performs the reproduction, crossover, and mutation stages.
      /**
       * For this Genetic Algorithm, the "genes" are not bits but
       * floating-point numbers.  So the crossover stage, in this GA, is not
       * an exchange of bits, but a randomly-weighted sum:
       * \par
       * <em>A</em>*<em>x</em><sub>1</sub> +
       * (1-<em>A</em>)*<em>x</em><sub>2</sub>
       */
      void breedAndMutate(popVec_t& newpop,
                          const popVec_t& oldpop,
                          const dvector_t& ratingv);

  private:
      const dvector_t m__blankParamVec;
      popVec_t m__wrkPop0;
      dvector_t m__wrkChiPop0;
      dvector_t m__wrkMutantDNA;
      fortlib::ivector_t m__wrkSortPop0;
      fortlib::ivector_t m__wrkSortChiSq;
      popVec_t m__LastParamPop;
      popVec_t m__NextParamPop;
      dvector_t m__ChiPop;
      dvector_t m__ChiRating;
      PopVecFiller m__zeroingFunctor;

      struct PopVecFiller
      {
          void operator()(popVec_t& populationVector);
          void operator()(dvector_t& val);
      };
  };


  // Include large function bodies automatically.
  //#include "FitGA.tcc"

 }; //end namespace optimize
}; //end namespace jpw_nld


#endif //_FitGA_H_
/////////////////////////
//
// End
