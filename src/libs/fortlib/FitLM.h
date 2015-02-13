// -*- C++ -*-
// Header file for class FitLM
//
// Copyright (C) 2007-2008, 2015 by John Weiss
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
// RCS $Id: FitLM.h 2850 2013-10-17 00:52:45Z candide $
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
   * Wrapper around the FORTRAN function, \c lmder_().  Performs a nonlinear
   * least-squares minimization using the Levenberg-Marquardt algorithm.
   *
   * \section LMNLLSqF Nonlinear Least-Squares Minimization
   *
   * There are two uses of the Levenberg-Marquardt algorithm:
   * -# Solving \a <tt>M</tt> nonlinear functions in \a <tt>N</tt>
   *    variables.
   *    \n
   *    Specifically, it solves for the \em zeros of the functions.
   * -# Nonlinear fitting of a function with \a <tt>M</tt> parameters to a
   *    dataset with \a <tt>N</tt> elements.
   *    - The fitting-function has one or more independent variables, in
   *      addition to the \a <tt>M</tt> parameters.
   *    - Each element in the dataset corresponds to a specific value of the
   *      independent variable(s).
   *    - The fitting-function is nonlinear <em>in the parameters</em>.  It
   *      may or may not be a nonlinear function of its independent
   *      variables().
   *
   * The FORTRAN library is actually trying to solve the following:
   * \f[
   * f_{i}\left(x_{1},\, x_{2},\,\cdots,\, x_{N}\right) =
   * \epsilon_{i}\qquad\forall\; i\in\left[1,\, M\right]
   * \f]
   * by finding the values of \f$ x_{j} \f$ (where \f$ n\in\left[1,\,N\right]
   * \f$ ) that minimize the \a <tt>M</tt> different functions \f$ f_{i} \f$
   * to zero.
   * \n
   * Now, if your \a <tt>M</tt> equations look like this:
   * \f[
   * f_{i}\left(x_{1},\, \cdots,\, x_{N}\right) =
   * c_{i}\qquad\forall\; i\in\left[1,\, M\right]
   * \f]
   * then you simply recast the equation:
   * \f[
   * f_{i}\left(x_{1},\, \cdots,\, x_{N}\right) - c_{i} = \varepsilon_{i}
   * \f]
   * so that the algorithm finds the \f$ x_{j} \f$ that minimize all of the
   * \f$ \varepsilon_{i} \f$ to zero.
   *
   * You will also need to compute the Jacobian matrix:
   * \f[
   * J_{i,j}\left(x_{1},\,\cdots,\, x_{N}\right) =
   * \left.\frac{\partial f_{i}}{\partial x_{j}}
   * \right|_{\left(x_{1},\cdots,x_{N}\right)}
   * \;\forall\; i\in\left[1,\, M\right]\quad j\in\left[1,\, N\right]
   * \f]
   * You'll start by computing the partial derivatives of the functions on
   * paper.  You'll then, inside of your fitting-function, compute the value
   * of each \f$ J_{i,j} \f$ at the \f$ x_{j} \f$ passed to you by the
   * minimization algorithm.
   *
   * \n\n
   *
   * Nonlinear least-squares fitting is a variation of the equation-solving
   * problem.  This time, however, you have a function, \f$ H\left(u\right)
   * \f$, that you want to fit to a \a <tt>N</tt> element dataset:
   * \f[
   * H\left(u_{j}\right) = d_{u_{j}} \qquad\forall\; j\in\left[1,\, N\right]
   * \f]
   * Each \f$ d_{u_{j}} \f$ is the element in the dataset that corresponds to
   * the specific value of \f$ u \f$ which we've called \f$ u_{j} \f$.
   * Alternatively,\f$ u_{j} \f$ is the value of \f$ u \f$ that will make the
   * model equal the data point \f$ d_{u_{j}} \f$ ... once the model fits
   * the data, that is.  To fit the model to the data, \f$ H \f$ must have
   * tunable parameters:  \f$p_{i}\f$.  We want to find the values of these \a
   * <tt>M</tt> tunable parameters that make the preceding equation valid for
   * all of the \f$d_{u_{j}}\f$.
   *
   * You can re-express the problem as follows:
   * \f[
   * H\left(p_{1},\,\cdots,\, p_{M}\,;\; u_{j}\right)-d_{u_{j}} =
   * \varepsilon_{j}\qquad\forall\; j\in\left[1,\, N\right]
   * \f]
   * This is now in the form that the FORTRAN function, \c lmder_(), is
   * designed to solve.
   *
   * The Levenberg-Marquardt algorithm requires a Jacobian matrix.  However,
   * we only have one equation, the model \f$ H\left(u\right) \f$, and we're
   * not solving the problem by changing \f$ u \f$, we're tuning the
   * parameters, \f$ p_i \f$.  So the "Jacobian matrix" changes into:
   * \f[
   * J_{i}\left(u_{j}\right) =
   * \left.\frac{\partial H}{\partial p_{i}}\right|_{u_{j}}
   * \qquad\forall\; i\in\left[1,\, M\right]\quad j\in\left[1,\, N\right]
   * \f]
   * Now, instead of a matrix that is a function of \a <tt>N</tt> values, we
   * have \a <tt>M</tt> functions, \f$ J\left(u\right) \f$, that we are
   * evaluating at \a <tt>N</tt> specific values of \f$ u \f$.  Each of those
   * specific values, \f$ u_{j} \f$ corresponds to one of the \a <tt>N</tt>
   * data points.
   *
   * Notice, too, that we take the partial derivative of the model <em>with
   * respect to each parameter</em>, \f$ p_{i} \f$.  Again, we are computing
   * \f$ H\left(u\right) \f$ at the <em>fixed values</em> \f$ u_{j} \f$.  For
   * the purposes of fitting the model to the data, the model's independent
   * variable, \f$ u \f$, is effectively constant.
   *
   * One final point about \f$ J_{i}\left(u_{j}\right) \f$:  it doesn't depend
   * on the data <em>directly</em>.  Instead, it depends on the data
   * indirectly through the fixed values, \f$ u_{j} \f$.
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
           * The estimated relative error in the \f$\Delta\f$-parameter vector
           * is below the error tolerance passed to the LM fitter.  Either
           * that, or the \f$\Delta\f$-parameter vector itself has changed by
           * less that the error tolerance.
           * &quot;\f$\Delta\f$-parameter&quot; refers here to the change in
           * the vector of tunable parameters in the model being fit (or
           * independent variables in the equations being minimized).
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
           * the &quot;\f$\Delta\f$-parameter&quot; vector as far as it can,
           * yet it still isn't below the error tolerance passed by the
           * caller.  Indicates either a poor fit, too small of a desired
           * parameter error, or a combination of the two.
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
       * This is the function to minimize.  It should produce a vector of
       * \p mm floating-point values.
       *
       * Since the Levenberg-Marquardt algorithm minimizes this function, if
       * you wish to instead fit a model to data, you must somehow build the
       * data into the fit function.  Unfortunate, but we're stuck with the
       * design of the underlying FORTRAN code.
       *
       * The parameters are, in order:
       *
       * \param mm
       * \c integer
       * \n
       * The number of equations (or the number of data points to fit to).
       * Will be the same as the value passed to the first argument of
       * operator()(int, dvector_t&, double, double, int, double).
       *
       * \param nn
       * \c integer
       * \n
       * The number of variables in the equations (or parameters in the model
       * being fit).
       *
       * \param x
       * \c double
       * \n
       * An \a nn element vector of the variables (or for fitting to a model,
       * the values of the parameters), to use to compute \a f_vec or \a
       * f_jac.
       *
       * \param f_vec
       * \c double
       * \n
       * An \a mm element vector.  Should be filled with the value of the \a
       * mm equations (or the model minus the data), computed using \a x.
       * \n
       * (Each \a f_vec[j] must obviously be computed with \a x[j].)
       *
       * \param f_jac
       * \c double
       * \n
       * An \a nn by \a ldfjac matrix, flattened into an array.  Should be
       * filled with the Jacobian of the \a mm equations, computed using \a
       * x.
       * \n
       * The matrix has been flattened into <tt>f_jac[j + mm*i]</tt>, where
       * <tt>i</tt> is the index representing a specific equation and
       * <tt>j</tt> represents the index used with the array, \a x.  Be sure
       * to compute each of the \a mm elements of \a f_jac with the correct
       * corresponding element of \a x !
       *
       * \param ldfjac
       * \c integer
       * \n
       * The long-dimension of \a f_jac.  Will be identical to the value
       * passed to the constructor argument, \a ndata_max.  Should therefore
       * be identical to \a mm (unless you're doing something weird).
       *
       * \param iflag
       * \c integer
       * \n
       * Control flag.  When set to \c 1, the function should compute \a f_vec
       * and leave \a f_jac unaltered.  When set to \c 2, the function should
       * leave \a f_vec alone and compute \a f_jac.  The function can set this
       * to a negative number to terminate minimization, but shouldn't modify
       * it otherwise.
       */
      typedef void (*fit_function_ptr_t)(fort_ivar_t, fort_ivar_t,
                                         fort_dvec_t, fort_dvec_t,
                                         fort_dmat_t,
                                         fort_ivar_t, fort_ivar_t);

      /// Main Constructor
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
       * the model and its Jacobians.
       * [See \ref FitLM::fit_function_ptr_t "fit_function_ptr_t" for
       * details.]
       *
       * \param ndata_max
       * The maximum number of data points you intend to fit to using this
       * object.
       *
       * \param nparm
       * The number of parameters in \a ffp that you're fitting to.  This
       * number also defines the minimum number of data points you intend to
       * fit to.
       */
      FitLM(fit_function_ptr_t ffp, index_t ndata_max, index_t nparm);

      /// Destructor
      ~FitLM();

      /// Perform a nonlinear least-squares fit.
      /**
       * Perform a nonlinear least-squares fit using the Levenberg-Marquardt
       * algorithm.  See the c'tor for some related info.
       *
       * \param mm
       * The number of data points used in the fit.  It must be in the range
       * <tt>[</tt>\a nparm<tt>, </tt>\a ndata_max<tt>]</tt>, inclusive, where
       * \a ndata_max and \a nparm are two of the parameters you passed to the
       * c'tor.
       * \n
       * You will almost always use the same value here as you did for \a
       * ndata_max in the c'tor.
       *
       * \param xv
       * A vector of initial parameter values.  On return, it
       * will contain the best-fit values of the parameters.
       *
       * \param errtol
       * The tolerance for \f$\chi^2\f$.  If the change in \f$\chi^2\f$
       * from one iteration to the next is less than errtol, the algorithm
       * stops.
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
       * The maximum number of times that the algorithm recalculates
       * \c f_vec [see \c fit_function_ptr_t] before stopping.  Not really
       * the number of iteration, but close enough.  If the initial value of
       * \c maxiter==0, a value of <tt>maxiter=100*(<em>ndata_max</em>+1)</tt>
       * is used.
       *
       * \returns The error codes, which are the same that the FORTRAN
       * routine, \c lmder_(), returns in its \c info parameter.
       */
      FitStatus_t operator()(int mm, dvector_t& xv,
                             double errtol, double ptol,
                             int maxiter, double factor);

      /// Compute \f$\chi^2\f$ for the last run of \c operator().
      /**
       * Equivalent to running \c jpw_math::chiSquared() on \c
       * f_vec().
       */
      double chiSquared() const {
          return jpw_math::chiSquared<dvector_t>(m__f_vec);
      }

      /// Accessor function for the final value of f_vec.
      /**
       * \see fit_function_ptr_t
       */
      const dvector_t& f_vec() const
      { return m__f_vec; }  // FIXME::Rename this fn.

      /// Accessor function for the f_jac.
      /**
       * Note that it doesn't return the actual Jacobian (which is fed to the
       * FORTRAN routine as a flat vector), but calls \c Matrix::swap() on an
       * internal member.  So, it's a tad slower than a simple "return a
       * member" accessor.
       *
       * \see fit_function_ptr_t
       */
      const dmatrix_t& f_jac() {  // FIXME::Rename this fn.
          // Only call 'swap()' if we've run the LM-algorithm since the last
          // call to this fn.
          if(m__jac_requiresUpdate) {
              m__f_jac_out.swap(m__f_jac_flat);
          }
          return m__f_jac_out;
      }

  private:
      fit_function_ptr_t m__ffp;
  protected:
      int m__ndataMax;
      int m__nparams;
  private:
      bool m__jac_requiresUpdate;
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
