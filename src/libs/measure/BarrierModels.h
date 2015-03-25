// -*- C++ -*-
// Header file for barrier model classes.
//
// Copyright (C) 2008-2010, 2015 by John Weiss
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
// For the FitLM::FitFnAction_t enum.
#include "FitLM.h"
#include "MathTools.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace measure {
  // Using decls.
  //
  using jpw_math::dvector_t;
  using jpw_nld::fortlib::FitLM;

  // Forward Declarations
  //
  class PersistenceMap;


  /// Policy classes for use with the \c BarrierModel template class.
  /**
   * Each class in this namespace must define one member:  the integer
   * constant, \c N_PARAMETERS.  This is the only requirement imposed by \c
   * BarrierModel.
   */
  namespace policy
  {
   // Class MarkovOnly
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement a
    * modified barrier model using only the Markov term.  (I. e. force
    * <tt>A==0</tt>.)
    *
    * \see BarrierModel
    * \see \ref BaMoMath
    */
   struct MarkovOnly {
       static const index_t N_PARAMETERS=1;
   };

   // Class BarrierOnly
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement a
    * modified barrier model that omits the Markov term.    (I. e. force
    * <tt>A==1</tt>.)
    *
    * \see BarrierModel
    * \see \ref BaMoMath
    */
   struct BarrierOnly {
       static const index_t N_PARAMETERS=2;
   };


   // Class Full
   /**
    * Policy class:  Instructs \c BarrierModel\<POL\> to implement the
    * full barrier model.
    *
    * \see BarrierModel
    * \see \ref BaMoMath
    */
   struct Full {
       static const index_t N_PARAMETERS=4;
   };
  };


  // Class BarrierModel
  /**
   * Core class for all of the variants of the barrier model.
   *
   * Several of the member functions won't make much sense unless you read
   * the section \ref BaMoMath.
   *
   * \tparam MODEL_POLICY
   * \em MUST be one of the 3 classes defined in the \c measure::policy
   * namespace.  If you try to use any other class, you will get linkage
   * failures from (one or more of) the functions \c randomParams(), \c
   * limitParams(), or \c calculate().
   *
   * \section BaMoTD Technical Details
   *
   * Most of the member functions are explicitly instantiated within the
   * library, so using \c -lmeasure when linking is sufficient.  Three
   * functions, however, do not have static linkage.  If you are using one of
   * the member functions \c randomParams(), \c limitParams(), and/or \c
   * calculate(), you will need to add
   * <tt>\#include&nbsp;"details/BarrierModels.tcc"</tt> to the
   * translation-unit calling them.
   *
   * The functions \c randomParams(), \c limitParams(), and \c calculate()
   * have different implementations, created through class-level template
   * specialization.  They have only been defined for the 3 different \c
   * measure::policy classes.  That's the reason why using an invalid class as
   * the \c MODEL_POLICY will cause link-errors.
   *
   * Notice that \c calculate() is \em also a template-<em>function</em>.
   * Doing so allows \c FitLM_Adapter to use this class without resorting to
   * nasty, nonportable tricks mucking about with pointers.  The documentation
   * for \c calculate() explains the reason why.
   *
   * \section BaMoMath The Barrier Model:  Theory and Implementation
   *
   * The function \c calculate(), as its name implies, calculates the equation
   * for the actual model of the persistence barrier.  The theory behind this
   * model can be found in my dissertation; it's outside of the scope of this
   * class' documentation.  Instead, we'll look at \em how \c calculate()
   * computes the barrier-model.
   *
   * \c calculate() has to use "rescaled parameters", or the
   * Levenberg-Marquardt algorithm (accessed through FitLM_BarrierAdapter),
   * won't work.  A set of member functions, named <tt>scale_*()</tt> and
   * <tt>descaled_*()</tt>, convert these "rescaled parameters" to "the
   * barrier-model's 'true' parameters".
   *
   * But first, let's look at the definition of the (full) persistence-barrier
   * model:
   * \f[
   * M(p,l\,;\, A, p_{b},\Delta p, \tau) \equiv
   * A\; B(p,l\,;\, p_{b},\Delta p) +
   * (1\,-\,A)\; e^{-\frac{l}{\left| \tau \right|}}
   * \f]
   * where \f$ 0 \le A \le 1 \,, \quad 0 \le p_{b} \le 1 \,, \quad
   * \left| \Delta p \right| \le 1 \,, \f$ \n
   * and:
   * \f[
   * B(p,l\,;\, p_{b},\Delta p) \equiv \sum ^{\infty }_{n=-\infty } \left[
   * \tanh\left(\frac{p-p_{b}-l + n}{\left| \Delta p \right| } \right) -
   * \tanh\left(\frac{p-p_{b} + n}{\left| \Delta p \right| } \right)
   * \right]
   * \f]
   * The values, \f$ A \f$, \f$ p_{b} \f$, \f$ \Delta p \f$,
   * and \f$ \tau \f$ are the model's four parameters.
   *
   * This is the model's purely-theoretical form.  It's also completely
   * unusable in a nonlinear least-squared fit.
   *
   * Firstly, and most obviously, there is the infinite sum.  That, however,
   * is easy to deal with.  (If you truncate the sum to \f$ \pm
   * \frac{20}{\Delta p} \f$, the resulting approximation error is smaller
   * than machine-precision.)
   *
   * Secondly, two of the parameters, (\f$\Delta p\f$ and \f$\tau\f$)
   * cannot be zero.
   *
   * Lastly, and more importantly, the derivative of this function is
   * discontinuous, and therefore utterly useless for nonlinear least-squares
   * fitting.  Removing the absolute-values makes the derivative usable, but
   * requires enforcing limits on the ranges of all four of the parameters.
   * The Levenberg-Marquardt algorithm cannot enforce these range-limits.
   * Worse, making the fit-function enforce these limits "gives the
   * Levenberg-Marquardt indigestion":  it fails to converge and/or will send
   * \f$ \tau \f$ (which has no upper limit) off to \f$ \infty \f$ (which
   * isn't a valid solution).
   *
   * To deal with these complications, we need to "rescale" the parameters,
   * changing the model to this form:
   * \f[
   * M^{\prime} \left( p,l\,;\, \alpha, p_{b},\nu, \rho \right) \equiv
   * \frac{1}{2}\left(1 + \cos \alpha \right)\:
   * B^{\prime}\left( p,l\,;\: p_{b}, \nu \right) +
   * \frac{1}{2}\left(1 - \cos \alpha \right)\: e^{-l\, \rho^{2}}
   * \f]
   * where:
   * \f{eqnarray*}{
   * B^{\prime}\left( p,l\,;\: p_{b}, \nu \right) & \equiv &
   * \sum ^{\kappa }_{n=-\kappa }\left[
   * \tanh\left(\nu^{2} \left[ p-p_{b}-l + n \right] \right) -
   * \tanh\left(\nu^{2} \left[ p-p_{b} + n \right] \right)
   * \right]
   * \\
   * & \vphantom{B} &
   * \\
   * A & = & \frac{1}{2}\left(1 + \cos \alpha \right)
   * \\
   * \Delta p & = & \nu^{2}
   * \\
   * \tau & = & \rho^{2}
   * \\
   * \kappa & \equiv & \textrm{int} \left( \frac{20}{\Delta p} \right)
   * \f}
   *
   * So now, we perform a nonlinear least-squares fit to \em this new model.
   * We then take the optimal parameters, \f$ \alpha \f$, \f$ p_{b}
   * \f$, \f$ \nu \f$, and \f$ \rho \f$, and convert them back to the original
   * parameters,  \f$ A \f$, \f$ p_{b} \f$, \f$ \Delta p \f$,
   * and \f$ \tau \f$, using the functions:
   * - descale_ampl(double)
   *   \n
   *   (from \f$ \alpha \f$ to \f$ A \f$)
   * - descale_width(double)
   *   \n
   *   (from \f$ \nu \f$ to \f$ \Delta p \f$)
   * - descale_tau(double)
   *   \n
   *   (from \f$ \rho \f$ to \f$ \tau \f$)
   *
   * The rescaled-model, \f$ M^{\prime} \left( p,l\,;\, \alpha, p_{b},\nu,
   * \rho \right) \f$, not only gets rid of the two absolute-values, it
   * eliminates any possibility of a divide-by-zero.  Furthermore, it
   * naturally imposes the limit \f$ 0 \le A \le 1 \f$ through the use of a
   * smooth periodic function ... just what the Levenberg-Marquardt algorithm
   * wants.
   *
   * The model has two special cases, \f$ A = 0 \f$ and \f$ A = 1 \f$.
   * They're used to analyze the results of the full model.  Rather than incur
   * the performance hit from using the general, full model, we use
   * class-template-specialization of \c calculate() to implement the two
   * special-case models:
   * \f[
   * M^{\prime} \left( p,l\,;\, \rho \right)_{A=1} \equiv e^{-l\, \rho^{2}}
   * \f]
   * and:
   * \f[
   * M^{\prime} \left( p,l\,;\, p_{b},\nu \right)_{A=0} \equiv
   * \sum ^{\kappa }_{n=-\kappa }\left[
   * \tanh\left(\nu^{2} \left[ p-p_{b}-l + n \right] \right) -
   * \tanh\left(\nu^{2} \left[ p-p_{b} + n \right] \right)
   * \f]
   * (Note that these are in rescaled form.) \n
   * The \c policy::MarkovOnly policy-class selects the first special-case,
   * while \c policy::BarrierOnly selects the second one.
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
          calculate<dvector_t>(theMap, fitParams, deltas, m__wrkJac,
                               FitLM::ComputeFunction);
      }

      /// Compute \f$ \chi^2 \f$ using \a theMap and evaluating the model at
      /// \a fitParams.
      /**
       * Equivalent to calling running \c jpw_math::chiSquared() on the \a
       * deltas you passed to a call to
       * \c operator()(const PersistenceMap&, dvector_t&, dvector_t&).
       */
      double chiSquared(const PersistenceMap& theMap, dvector_t& fitParams)
      {
          calculate<dvector_t>(theMap, fitParams, m__wrkErrs, m__wrkJac,
                               FitLM::ComputeFunction);
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
       * It's not used by any of the member functions in this class [which
       * handles out-of-range values in a different way], but has been
       * provided for use by other code.
       *
       * To use this function, you must
       * <tt>\#include&nbsp;"BarrierModels.tcc"</tt>.  This is done to promote
       * inlining it.
       */
      static void limitParams(dvector_t& params);

      /// Given the "normal" barrier amplitude parameter, returns its rescaled
      /// equivalent.
      /**
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double scale_ampl(double us_ampl) {
          return(acos(2*us_ampl - 1));
      }

      /// Inverse operation to \c scale_ampl().
      /**
       * Used to convert from the value found by FitLM_BarrierAdapter back to
       * the actual barrier measure, \f$ A \f$ (the barrier amplitude).
       *
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double descale_ampl(double s_ampl) {
          return(0.5*(cos(s_ampl) + 1.0));
      }

      /// Given the "normal" barrier width parameter, returns its rescaled
      /// equivalent.
      /**
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double scale_width(double us_width) {
          return(1.0/sqrt(us_width));
      }

      /// Inverse operation to \c scale_width().
      /**
       * Used to convert from the value found by FitLM_BarrierAdapter back to
       * the actual barrier measure, \f$ \Delta p \f$ (the barrier width).
       *
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double descale_width(double s_width) {
          return(jpw_math::SQR(1.0/s_width));
      }

      /// Given the "normal" decay time parameter, returns its rescaled
      /// equivalent.
      /**
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double scale_tau(double us_tau) {
          return(1.0/sqrt(us_tau));
      }

      /// Inverse operation to \c scale_tau().
      /**
       * Used to convert from the value found by FitLM_BarrierAdapter back to
       * the actual barrier measure, \f$ \tau \f$ (the rate of stochastic
       * decorrelation).
       *
       * \see BarrierModel, specifically \ref BaMoMath "this" section
       */
      static double descale_tau(double s_tau) {
          return(jpw_math::SQR(1.0/s_tau));
      }

  protected:
      tslen_t m__callcount;
      dvector_t m__wrkJac;
      dvector_t m__wrkErrs;

      /// The function that actually implements the model.
      /**
       * It increments m__callcount every time it's invoked.
       *
       * \tparam VT&nbsp;&nbsp;
       * Will be either \c fortlib::fort_dvec_t or \c dvector_t [which is just
       * a \c typedef to <tt>std::vector&lt;double&gt;</tt>].  Don't use any
       * other container type.
       *
       * <hr/>
       *
       * <b>Design Notes</b>:
       *
       * At present, this functor is used in two ways:  (1) by the
       * FitLM_BarrierAdapter; and (2) in a genetic algorithm.  The latter is
       * implemented in C++, so there is absolutely \em no \em reason to force
       * it to use the pointer-based FORTRAN vector, \c fort_dvec_t.  Why add
       * unnecessary memory-management overhead just to satisfy an external
       * library from an different programming language?
       *
       * However, it makes just as little sense to create two copies of the
       * same function.  Since we have 3 different variants of the barrier
       * model, we would have 6 times the maintenance load.  But why double
       * our work just for a change from \c fort_dvec_t to \c
       * jpw_math::dvector_t [a.k.a. \c double* respectively] and \c
       * std::vector<double> respectively]?
       *
       * Obviously, this is a job for templates!  The question, however, is
       * \em where to templatize.
       *
       * The entire class need not be templatized on a vector-type.  Only one
       * member function requires this distinction, the \c operator().
       *
       * However, we can't specialize \c operator(), because \c
       * FitLM_BarrierAdapter requires it to have external linkage.  It also
       * requires \c operator() to have a specific signature, which the C++
       * user of this functor doesn't need.
       *
       * Hence this member function template and the two \c operator()
       * overloads.  The latter become simple dispatch-calls to this function,
       * which the compiler can inline.
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
