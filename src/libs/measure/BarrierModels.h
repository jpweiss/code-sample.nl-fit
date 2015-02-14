// -*- C++ -*-
// Header file for barrier model classes.
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
// RCS $Id: BarrierModels.h 2221 2011-07-09 01:56:38Z candide $
//
#ifndef _BarrierModels_H_
#define _BarrierModels_H_

// Includes
//
#include <cmath>
#include <vector>
#include <boost/utility.hpp>
#include "FORTTypes.h"
#include "MathTools.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace measure {
  // Using decls.
  //
  using jpw_math::dvector_t;

  // Forward Declarations
  //
  class PersistenceMap;


  /// Policy classes for use with the \c BarrierModel template class.
  namespace policy
  {
   // Class MarkovOnly
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement a
    * modified barrier model using only the Markov term.  (I. e. force
    * <tt>A==0</tt>.)
    */
   struct MarkovOnly {
       static const index_t N_PARAMETERS=1;
   };

   // Class BarrierOnly
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement a
    * modified barrier model that omits the Markov term.    (I. e. force
    * <tt>A==1</tt>.)
    */
   struct BarrierOnly {
       static const index_t N_PARAMETERS=2;
   };

   // Class Full
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement the
    * full barrier model.
    */
   struct Full {
       static const index_t N_PARAMETERS=4;
   };
  };


  // Class BarrierModel
  /**
   * Core class for all of the variants of the barrier model.
   *
   * Each of the 3 valid variants is "activated" using the template parameter,
   * \c MODEL_POLICY, which \em MUST be one of the 3 classes defined in the \c
   * measure::policy namespace.
   *
   * From a C++ standpoint, the member function BarrierModel::calculate() has
   * 3 different implementations, one for each of the valid policy classes.
   * Furthermore, to allow \c FitLM to use this class without resorting to
   * nasty, nonportable tricks mucking about with pointers,
   * BarrierModel::calculate() is, itself, a template function.  See its
   * documentation for more details.
   *
   * Most of the member functions are explicitly instantiated within the
   * library, so using \c -lmeasure when linking is sufficient.  The member
   * functions \c randomParams() and \c limitParams(), however, do not have
   * static linkage.  There is also a private template function, \c
   * calculate(), used by many of the other member functions.  Because
   * of this, if you intend to call \em any functions in this class, you must
   * add <tt>\#include&nbsp;"details/BarrierModels.tcc"</tt> to your source
   * code.
   */
  template<typename MODEL_POLICY=policy::Full>
  class BarrierModel : private boost::noncopyable
  {
  public:
      typedef MODEL_POLICY Policy_t;
      static const unsigned KAPPA1=20;
      static const index_t N_PARAMETERS=Policy_t::N_PARAMETERS;

      /// Default Constructor
      explicit BarrierModel(tslen_t nData)
          : m__callcount(0)
          , m__wrkJac(nData)
          , m__wrkErrs(nData)
      {}

      /// Accessor fn. for the # of times the model was called.
      tslen_t callcount() const {
          return m__callcount;
      }

      /// Resets to zero the counter that tracks the # of times the model was
      /// called.
      void clearCallcount() {
          m__callcount = 0;
      }

      /// The model, C++-style.
      void operator()(const PersistenceMap& theMap,
                      dvector_t& fitParams, dvector_t& deltas)
      {
          calculate<dvector_t>(theMap, fitParams, deltas, m__wrkJac, 1);
      }

      /// Compute \c Chi^2 using \a theMap and evaluating the model at \a
      /// fitParams.
      /**
       * Equivalent to running \c jpw_math::chiSquared() on the \c
       * deltas you passed to a call to \c operator().
       */
      double chiSquared(const PersistenceMap& theMap, dvector_t& fitParams)
      {
          calculate<dvector_t>(theMap, fitParams, m__wrkErrs, m__wrkJac, 1);
          return jpw_math::chiSquared<dvector_t>(m__wrkErrs);
      }

      /// The model, in the form required by \c FitLM_Adapter.
      /**
       * Not all of the args are used.
       */
      void operator()(int /*neq*/, const PersistenceMap& theMap,
                      int /*nvar*/, fortlib::fort_dvec_t fitParams,
                      fortlib::fort_dvec_t deltas,
                      fortlib::fort_dmat_t fnJacob,
                      int /*ld_fnJac*/, int actionCode)
      {
          calculate<fortlib::fort_dvec_t>(theMap, fitParams,
                                          deltas, fnJacob, actionCode);
      }

      /// Fill \a params with "constrained" random values.
      /**
       * The "constrained" random values are uniformly-distributed pRNGs, but
       * over a range that makes sense for this model.
       *
       * To use this function, you must
       * <tt>\#include&nbsp;"BarrierModels.tcc"</tt>.  This is done to promote
       * inlining it.
       */
      static void randomParams(dvector_t& params);

      /// Restrict \a params according to their natural ranges.
      /**
       * The model's parameters won't necessarily have infinite range.  This
       * function takes any parameter that is out of range and readjusts it so
       * that it remains in-bounds.
       *
       * To use this function, you must
       * <tt>\#include&nbsp;"BarrierModels.tcc"</tt>.  This is done to promote
       * inlining it.
       */
      static void limitParams(dvector_t& params);

      /// The shape.
      static double shape_h(double x) {
          return(tanh(x));
      }

      /// The first derivative of shape_h()
      static double deriv_h(double x) {
          double sech_x;
          sech_x = (1.0/cosh(x));
          return(jpw_math::SQR(sech_x));
      }

      /// Given the unscaled barrier amplitude, returns its scaled equivalent.
      static double scale_ampl(double us_ampl) {
          return(acos(2*us_ampl - 1));
      }

      /// Inverse operation to \c scale_ampl().
      static double descale_ampl(double s_ampl) {
          return(0.5*(cos(s_ampl) + 1.0));
      }

      /// Given the unscaled barrier width, returns its scaled equivalent.
      static double scale_width(double us_width) {
          return(1.0/sqrt(us_width));
      }

      /// Inverse operation to \c scale_width().
      static double descale_width(double s_width) {
          return(jpw_math::SQR(1.0/s_width));
      }

      /// Given the unscaled decay time, returns its scaled equivalent.
      static double scale_tau(double us_tau) {
          return(1.0/sqrt(us_tau));
      }

      /// Inverse operation to \c scale_tau().
      static double descale_tau(double s_tau) {
          return(jpw_math::SQR(1.0/s_tau));
      }

  protected:
      tslen_t m__callcount;
      dvector_t m__wrkJac;
      dvector_t m__wrkErrs;

      /// The function that actually implements the model.
      /**
       * NOTE:  This is a template so that we can use the same implementation
       * with both dvector_t and double*, the latter being required by \c
       * FitLM (for the sake of efficiency).
       *
       * It increments m__callcount every time it's invoked.
       */
      template<typename VT>
      void calculate(const PersistenceMap& theMap,
                     VT& fitParams, VT& deltas, VT& fnJacob, int actionCode);
  };


  //
  // Member Typedefs
  //


  /// The Full Barrier Model
  typedef BarrierModel<> FullBarrierModel_t;

  /// Modified Barrier Model using only the Markov term.
  typedef BarrierModel<policy::MarkovOnly> MarkovOnlyBarrierModel_t;

  /// Modified Barrier Model omitting the Markov term.
  typedef BarrierModel<policy::BarrierOnly> BarrierOnlyBarrierModel_t;


 }; //end namespace
}; //end namespace


#endif //_BarrierModels_H_
/////////////////////////
//
// End
