// -*- C++ -*-
// Header file for class FitLM
//
// Copyright (C) 2007-2008 by John Weiss
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
// RCS $Id: FitLM.h 2221 2011-07-09 01:56:38Z candide $
//
#ifndef _FitLM_H_
#define _FitLM_H_

// Includes
//
#include <boost/utility.hpp>
#include "jpw_nld.h"
#include "FORTTypes.h"
#include "MathTools.h"
// Alas, we need to include this directly.  The members "m__f_vec" and
// "m__f_jac" require a complete type.
#include "Matrix.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace fortlib {
  // Class FitLM
  /**
   * Wrapper around the FORTRAN function, \c lmder_.  Performs a nonlinear
   * least-squares fit using the Levenberg-Marquardt algorithm.
   */
  class FitLM : private boost::noncopyable
  {
  public:
      /// The return status of the Levenberg-Marquardt algorithm.
      enum FitStatus_t {
          /// The fit function aborted while computing the Jacobians.
          ModelAbortedIn_fjac=-2,
          /// The fit function aborted while computing itself.
          ModelAbortedIn_fvec=-1,
          /// Improper input parameters passed.
          InputError=0,
          /// The LM algorithm stopped due to quadrature.
          /**
           * The estimated relative error in the "sum of squares" is below the
           * error tolerance passed to the LM fitter.
           */
          Success_SumSq=1,
          /// The LM algorithm stopped due to fit parameter convergence.
          /**
           * The estimated relative error in the \Delta parameter vector is
           * below the error tolerance passed to the LM fitter.  Either that,
           * or the \Delta parameter vector itself has changed by less that
           * the error tolerance.  "\Delta parameter" refers here to the
           * change in the vector of tunable parameters in the model being
           * fit (or independent variables in the equations being minimized).
           */
          Success_dParam=2,
          /// Both \c Success_SumSq and \c Success_dParam are true.
          Success_Both=3,
          /// The LM algorithm has hit machine precision.
          /**
           * Specifically, the vector of values from the minimized function
           * (or the vector of errors in a fit to data) is orthogonal, to
           * within machine precision, to any column of the Jacobian.
           *
           * \note The underlying FORTRAN routine returns this code when the
           * vector of values (or fit errors) is orthogonal to any column of
           * the Jacobian to within a specified tolerance.  FitLM sets that
           * tolerance to "0", however.  So, this status code becomes
           * equivalent to \c MachinePrec_LM.
           */
          MachinePrec=4,
          /// The LM algorithm aborted due to too many iterations.
          /**
           * Specifically, the LM algorithm has computed the fit function
           * (not the Jacobians) the maximum number of times.  The "maximum
           * number of times" is one of the control parameters passed by the
           * caller.
           */
          IterationOverflow=5,
          /// Can't minimize the sum of the squares any further.
          /**
           * Specifically, the LM Algorithm has minimized the sum of the
           * squares as far as it can, yet it still isn't below the error
           * tolerance passed by the caller.  Indicates either a poor fit, too
           * small of a desired least-squares error, or a combination of the
           * two.
           *
           * This underflow error corresponds to the success status, \c
           * Success_SumSq.
           */
          UnderflowError_SumSq=6,
          /// Can't reduce the relative parameter error any further.
          /**
           * Specifically, the LM Algorithm has reduced the relative error in
           * the "\Delta parameter" vector as far as it can, yet it still
           * isn't below the error tolerance passed by the caller.  Indicates
           * either a poor fit, too small of a desired parameter error, or a
           * combination of the two.
           *
           * This underflow error corresponds to success status, \c
           * Success_dParam.
           */
          UnderflowError_dParam=7,
          /// The LM algorithm has hit machine precision.
          /**
           * Specifically, the vector of values from the minimized function
           * (or the vector of errors in a fit to data) is orthogonal, to
           * within machine precision, to any column of the Jacobian.
           *
           * \note Because \c FitLM uses an orgthogonality tolerance of "0",
           * this status code and \c MachinePrec mean the same thing.  So we
           * convert this code to the former on return.
           */
          MachinePrec_LMAlg=8,
      };

      /// Function pointer type for the fit function.
      /**
       * This is the function to minimize.  It should produce a vector of \mm
       * values, the magnitude of which will be minimized.
       *
       * Since the Levenberg-Marquardt algorithm minimizes this function, if
       * you wish to instead fit a model to data, you must somehow build the
       * data into the fit function.  Unfortunate, but we're stuck with the
       * design of the underlying FORTRAN code.
       *
       * The parameters are, in order:
       *
       * \param mm
       * integer.  The number of equations, i.e. the number of data points to
       * fit to.
       *
       * \param nParams
       * integer.  The number of parameters in the model being fit.
       *
       * \param x
       * double.  An \a nParams element vector of the most recent parameter
       * values.
       *
       * \param f_vec
       * double.  An \a mm element vector.  Should be filled with the value of
       * the \a mm equations, computed at \a x.
       *
       * \param f_jac
       * double.  An \a nParams by \a ldfjac matrix.  Should be filled with
       * the jacobian computed at \a x.
       *
       * \param ldfjac
       * integer.  The long-dimension of \a f_jac.  Usually identical to \a
       * mm, but the FORTRAN algorithm provides it for special cases needing a
       * larger jacobian matrix.
       *
       * \param iflag
       * integer.  Control flag.  When set to \c 1, the function should
       * compute \a f_vec and leave \a f_jac unaltered.  When set to \c 2, the
       * function should leave \a f_vec alone and compute \a f_jac.  The
       * function can set this to a negative number to terminate minimization,
       * but shouldn't modify it otherwise.
       */
      typedef void (*fit_function_ptr_t)(fort_ivar_t, fort_ivar_t,
                                         fort_dvec_t, fort_dvec_t,
                                         fort_dmat_t,
                                         fort_ivar_t, fort_ivar_t);

      /// Default Constructor
      /**
       * Technically, this object doesn't \em really perform a fit, but a
       * minimization.  Specifically, it minimizes \a mm "equations" in \a
       * nparm variables (where \a nparm is the argument passed to the c'tor).
       * The fit function can be nonlinear in one or more of the variables.
       *
       * As a result, the fit function, \a ffp, must somehow "contain" the
       * data as well as the model.
       *
       * \param ffp
       * A function pointer to the fit function.  The fit function computes
       * the model and its jacobians.  See \c fit_function_ptr_t for details.
       *
       * \param ndata
       * The maximum number of data points you intend to fit to using this
       * object.
       *
       * \param nparm
       * The number of parameters in \a ffp that you're fitting to.  This
       * number also defines the minimum number of data points you intend to
       * fit to.
       */
      explicit FitLM(fit_function_ptr_t ffp, index_t ndata, index_t nparm);

      /// Destructor
      ~FitLM();

      /// Perform a nonlinear least-squares fit.
      /**
       * Perform a nonlinear least-squares fit using the Levenberg-Marquardt
       * algorithm.  See the c'tor for some related info.
       *
       * \param mm
       * The number of data points used in the fit.  It must be in the range
       * <tt>[</tt>\a nparm<tt>, </tt>\a ndata<tt>]</tt>, inclusive, where \a
       * ndata and \a nparm are two of the parameters you passed to the
       * c'tor.
       *
       * \param xv
       * A vector of initial parameter values.  On return, it
       * will contain the best-fit values of the parameters.
       *
       * \param errtol
       * The tolerance for \Chi^2.  If the change in \Chi^2 from one iteration
       * to the next is less than errtol, the algorithm stops.
       *
       * \param ptol
       * The tolerance for the set of parameters.  If the magnitude of the
       * parameter vector changes by less than ptol, the algorithm stops.
       *
       * \param factor
       * The initial maximum step size.  A typical value is in the range
       * [0.1,100.0].  If factor==0, a value of factor=100.0 is used instead.
       *
       * \param maxiter
       * The maximum number of times the routine recalculates fv[] before
       * stopping.  Not really the number of iteration, but close enough.  If
       * the initial value of maxiter==0, a value of maxiter=100*(nn+1) is
       * used.
       *
       * \returns The error codes, which are the same that the FORTRAN
       * routine, lmder(), returns in its \c info parameter.
       */
      FitStatus_t operator()(int mm, dvector_t& xv,
                             double errtol, double ptol,
                             int maxiter, double factor);

      /// Compute \Chi^2 for the last run of \c operator().
      /**
       * Equivalent to running \c jpw_math::chiSquared() on \c
       * f_vec().
       */
      double chiSquared() const {
          return jpw_math::chiSquared<dvector_t>(m__f_vec);
      }

      /// Accessor function for the f_vec.
      const dvector_t& f_vec() const
      { return m__f_vec; }

      /// Accessor function for the f_jac.
      /**
       * Note that it doesn't return the actual Jacobian (which is fed to the
       * FORTRAN routine as a flat vector), but calls \c Matrix::swap() on an
       * internal member.  So, it's a tad slower than a simple "return a
       * member" accessor.
       */
      const dmatrix_t& f_jac() {
          m__f_jac_out.swap(m__f_jac_flat);
          return m__f_jac_out;
      }

  private:
      fit_function_ptr_t m__ffp;
  protected:
      int m__ndata;
      int m__nparams;
  private:
      int m__wrksz;
      fort_ivec_t m__ipvt;
      fort_dvec_t m__workbuf;
      fort_dvec_t m__diag;
      fort_dvec_t m__qtf;
      fort_dvec_t m__wa1;
      fort_dvec_t m__wa2;
      fort_dvec_t m__wa3;
      fort_dvec_t m__wa4;
  protected:
      dvector_t m__f_jac_flat;
      dvector_t m__f_vec;
      dmatrix_t m__f_jac_out;
  };


  /// Convenience typedef for \c FitLM::FitStatus_t.
  typedef FitLM::FitStatus_t FitLMStatus_t;

 }; //end namespace
}; //end namespace


#endif //_FitLM_H_
/////////////////////////
//
// End
