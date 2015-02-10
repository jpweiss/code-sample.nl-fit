// -*- C++ -*-
// Header file for class FitLM_Adapter
//
// Copyright (C) 2007-2009, 2015 by John Weiss
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
// RCS $Id: FitLM_Adapter.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _FitLM_Adapter_H_
#define _FitLM_Adapter_H_

// Includes
//
#include "FitLM.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace fortlib {


 // Class FitLM_Adapter
 /**
  * Perform a nonlinear least-squares fit by passing a functor to the
  * Levenberg-Marquardt algorithm.
  *
  * Unlike its parent, FitLM, this class separates the model from the data
  * being fit to it, using template parameters to specify both model and data
  * in a typesafe fashion.
  *
  * \tparam D
  * \tparam F
  *
  * \par D
  * A sequence data-container. (i.e. a 1-dimensional data structure).
  * \n
  * It must contain the following members:
  * - \c public \em INT_TYPE <tt>size() const</tt>
  *   \n
  *   Should return the size of the current dataset.  Will be passed as the
  *   first arg to FitLM::operator().
  *   \n
  *
  * \par F
  * The fit-functor.  Computes the function that you are fitting the data to.
  * \n
  * Must have the following members:
  * - <tt>public static const</tt> \em INT_TYPE <tt>N_PARAMETERS</tt>
  *   \n
  *   Will be \c static_cast to an unsigned integer type.
  *   \n
  *   \n
  * - <tt>void operator()(int nData, const D\& fitData,
  *   <br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *                       int nParams, fort_dvec_t curParams,
  *   <br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *                       fort_dvec_t deltas, fort_dmat_t fnJacob,
  *   <br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *                       int ld_fnJac, int actionCode)</tt>
  *   \n
  *   This is the fit-functor itself, which computes the model.
  *   \n
  *   The parameters are as as follows:
  *   \n
  *     - \a <tt>nData</tt>
  *       \n
  *       The number of data points to fit to.  Will always be identical to \c
  *       fitData.size().
  *     - \a <tt>fitData</tt>
  *       \n
  *       This is the instance of \c D that was passed to \c
  *       FitLM_Adapter::operator().  The contents will remain \b unaltered.
  *     - \a <tt>nParams</tt>
  *       \n
  *       The number of tunable parameters in the model.
  *     - \a <tt>curParams</tt>
  *       \n
  *       An \a nParams element long \c fort_dvec_t, containing the current
  *       set of tunable parameters that the Levenberg-Marquardt algorithm is
  *       testing.
  *       \n
  *       You do not need to perform any memory-management with this \c
  *       fort_dvec_t.  That is all handled externally.
  *       \n
  *       <b><em>DO NOT MODIFY</em></b> its contents!
  *     - \a <tt>deltas</tt>
  *       \n
  *       A \a nData element long \c fort_dvec_t, pre-allocated externally for
  *       you.  (Therefore, you do not need to perform any memory-management
  *       operations on it.)
  *       \n
  *       Must be filled with difference between the \c fitData and the
  *       model.  See below for details of how you should compute it.
  *     \n
  *     - \a <tt>fnJacob</tt>
  *       \n
  *       An \a nParams by \a ldfjac matrix, flattened into a \c fort_dmat_t.
  *       You do not need to perform any memory-management on it, as it has
  *       been pre-allocated externally for
  *       \n
  *       Must be filled with the Jacobian matrix of the model being fit to,
  *       computed using \a curParams.  See below for details of how to do
  *       this.
  *     - \a <tt>ld_fnJac</tt>
  *       \n
  *       The "long-dimension" of \a fnJacob.  Should always be identical to
  *       \a nData.
  *     - \a <tt>actionCode</tt>
  *       \n
  *       An integer code for what the functor should compute.
  *       - When equal to \c 1, \a deltas must be computed and filled in,
  *         while \a fnJacob will remain unaltered.
  *       - When equal to \c 2, \a deltas must be left alone and \a fnJacob
  *         must be calculated and filled in.
  *
  * The typedefs \c fort_dvec_t and \c fort_dmat_t are defined in
  * "FORTTypes.h".
  *
  * \n
  *
  * \section FitFunctor Writing a Fit-Functor
  *
  * Implementing the fit-functor requires some knowledge of how the
  * Levenberg-Marquardt fits the data to the model.  The \ref LMNLLSqF
  * "documentation of FitLM" goes into the full details.
  *
  * Although they're passed to your fit-functor, 3 of the arguments are
  * redundant:
  * - \a <tt>nParams</tt>
  *   \n
  *   Since the model is likely a mathematical function that you wrote on
  *   paper, you already know the number of parameters.
  *   \n
  *   Furthermore, this argument should always have the same value as the
  *   inherited member \c FitLM::m__nparams.
  * - \a <tt>nData</tt>
  *   \n
  *   As noted above, this argument will always be identical to \c
  *   fitData.size().  You could use it instead of calling \c
  *   fitData.size(), thereby optimizing away the function call.  Or, if
  *   you prefer, you could safely ignore it and hope that the compiler
  *   optimizes your \c fitData.size() calls.
  * - \a <tt>ld_fnJac</tt>
  *   \n
  *   As stated earlier, the value of this argument will be identical to \a
  *   nData.  It will also be identical to the inherited member \c
  *   FitLM::m__ndata.
  *   \n
  *   So, if you wish, you could ignore both this and \a nData and just use \c
  *   FitLM::m__ndata instead.
  *
  * <h4>Computing &ldquo;<em><tt>deltas</tt></em>&rdquo;</h4>
  *
  * Your model will likely be a univariate function with one or more
  * constants, the model's "tunable parameters."  Mathematically, you're
  * trying to find the set of parameters, \f$ \left{p\right} \f$ that make
  * this equation true: \f[
  * f_{\left\{ p\right\} }\left(x_{j}\right) =
  * D\left(x_{j}\right)\qquad\forall\; j\in\left[0,\, nData\right)
  * \f]
  * Each data point, \f$ D\left(x_{j}\right) \f$, corresponds to a specific
  * value of \f$ x \f$ (labelled \f$ x_{j} \f$).  The fit-functor must compute
  * the model at each \f$ x_{j} \f$, so you should incorporate them into your
  * functor object, or into the data's container-type, \a D.  How you do so
  * is an implementation detail.
  *
  * Let's rewrite the previous equation in a form that illustrates what the
  * fit-functor will be computing: \f[
  * f\left(x_{j};\; p_{0},\,\cdots,\, p_{\left(nParams-1\right)}\right)
  * - D\left(x_{j}\right) =
  * \Delta_{j}\qquad\forall\; j\in\left[0,\, nData\right)
  * \f]
  *
  * Your fit-functor, therefore, should perform the following set of steps for
  * each element of \c fitData:
  * - Find the independent variable(s), '<tt>x[j]</tt>', corresponding to
  *   '<tt>fitData[j]</tt>';
  * - Using the parameter values from \a curParams, compute '<tt>f_j</tt>',
  *   the value of the fit-function evaluated at '<tt>x_j</tt>';
  * - Set:\code
  *     deltas[j] = fitData[j] - f_j
  * \endcode
  * - Repeat for all '<tt>j</tt>'.
  *
  * Note that, if your model is a multivariate function, you'll need to use a
  * list of pairs or tuples as the '<tt>x[j]</tt>' instead of using a list of
  * scalars.
  * \n
  *
  * <em>Remember</em>:  You only compute \a <tt>deltas</tt> when the argument
  * <tt><em>actionCode</em>==1</tt>.  Otherwise, do not touch \a \c deltas at
  * all.
  * \n
  *
  * <h4>Computing &ldquo;<em><tt>fnJacob</tt></em>&rdquo;</h4>
  *
  * The algorithm also requires a matrix of the model's partial-derivatives.
  * In the context of minimization algorithms, this matrix is known as a
  * "Jacobian matrix."  (However, for fitting data to a function, it's not
  * really a matrix, since it's 1-dimensional.)  To find the Jacobian for your
  * model, you will need to do the following partial-derivatives on paper:
  * \f[
  * J_{i} = \frac{\partial f}{\partial p_{i}}
  * \qquad\forall\; i\in\left[0,\, nParams\right)
  * \f]
  * Notice that you are taking the partial-derviatives <em>with respect to the
  * <b>parameters</b></em>, because in the context of fitting to a model, it's
  * the <em>parameters</em> that change (not the independent variable).
  *
  * You now have \a <tt>nParams</tt> equations.  You will need to compute them
  * at the same \f$ x_{j} \f$ that you used to compute the model itself:
  * \f[
  * J_{i}\left(x_{j}\right) =
  * \frac{\partial f}{\partial p_{i}}\left(x_{j}\right)
  * \qquad\forall\; i\in\left[0,\, nParams\right)
  * \quad j\in\left[0,\, nData\right)
  * \f]
  * This gives you a <em><tt>nParams</tt></em>-by-<em><tt>nData</tt></em>
  * matrix.
  *
  * The fit-functor will also be computing the Jacobian.  Here are the steps
  * that it should perform:
  * - Use the same independent variable(s), '<tt>x[j]</tt>', corresponding to
  *   '<tt>fitData[j]</tt>', that you used to compute the
  *   '<tt>deltas[j]</tt>'.
  *   - If your '<tt>x[j]</tt>' is some fixed, pre-build array, you're done.
  *   - If you have to construct '<tt>x[j]'</tt>, construct it in the \em
  *     exact \em same \em order that you did when you computed
  *     '<tt>deltas[j]</tt>'.
  *   - And, just in case it's not clear,
  *     \n
  *     The order of the '<tt>x[j]</tt>' is important!
  *     \n
  * - You will have \em nParams derivatives of your model.
  *   - Let's label the <tt>i<sup>th</sup></tt> derivative:\code
  *     df_dpi(double y, double* paramVec)
  *     \endcode
  *     Remember:  This is the partial-derivative of your model <em>with
  *     respect to</em> the <tt>i<sup>th</sup></tt> parameter.
  *   - You don't need to write a separate function for each of the
  *     derivatives.  In fact, you probably shouldn't for performance issues.
  *     We're only writing it as a function to make this explanation clearer.
  *     \n
  * - Repeat the following for each of your tunable parameters:
  *   - Set the integer variable, '<tt>i_param</tt>', to the index of the
  *     parameter whose derivative you're about to evaluate.
  *   - Compute the value
  *     '<tt>df_dpi_x_j&nbsp;=&nbsp;df_dpi(x[j],&nbsp;curParams)</tt>'
  *     - Remember:  you need to pick the correct '<tt>df_dpi(&hellip;)</tt>',
  *       the one that is the partial-derivative with respect to the
  *       <tt>i_param<sup>th</sup></tt> parameter.
  *     - You will compute the <tt>i_param<sup>th</sup></tt>
  *       partial-derivative using '<tt>x[j]</tt>' and the entire array,
  *       '<tt>curParams</tt>'.
  *       - Again:  you're not using only '<tt>curParams[i_param]</tt>',
  *         you'll likely need all of the elements of '<tt>curParams</tt>'.
  *       - In fact, you may have differentiated away the
  *         <tt>i_param<sup>th</sup></tt> parameter.
  *   - Set:\code
  *       fnJacob[j + nData*i_param] = df_dpi_x_j
  *     \endcode
  *     Note the indexing used.  Remember that \c fnJacob has been
  *     \em flattened from a 2-D matrix into a 1-D sequence.
  *     \n
  * - Repeat for all '<tt>j</tt>'.
  *
  * The same caveat for multivariate functions applies here:  you'll need to
  * use a list of pairs or tuples as the '<tt>x[j]</tt>' instead of a scalar
  * array.
  * \n
  *
  * <em>Remember</em>:  You only compute \a <tt>deltas</tt> when the argument
  * <tt><em>actionCode</em>==1</tt>.  Otherwise, do not touch \a \c deltas at
  * all.
  * \n
  */
  template<class F, class D>
  class FitLM_Adapter : protected FitLM
  {
  protected:
      typedef F FitFunctor_t;
      typedef D Data_t;
      typedef FitLM_Adapter<F,D> Self_t;

      static void fit_function_adapter(fort_ivar_t neq, fort_ivar_t nvar,
                                       fort_dvec_t xvec, fort_dvec_t fvec,
                                       fort_dmat_t fjac,
                                       fort_ivar_t ldfjac, fort_ivar_t iflag)
      {
          // FIXME:  Look into removing '*neq'.  Let the functor do any
          // optimizing away of calls to m__fitData.size()

          // FIXME: What about "wrapping" xvec, fvec, (and possibly fjac) into
          // a lightweight array-like class?  'std::array' won't work, since
          // its size is compile-time.  And std::vector copies its elements
          // in&out, so it's not suitable.
          // Hmmm...
          // ...perhaps a custom, STL-like adapter-class?
          (*(m__activeThis->m__fitter))(*neq, *(m__activeThis->m__fitData),
                                        *nvar, xvec, fvec, fjac,
                                        *ldfjac, *iflag);
      }

  public:
      /// Default Constructor
      explicit FitLM_Adapter(index_t ndata)
          : FitLM(Self_t::fit_function_adapter,
                  ndata,
                  static_cast<index_t>(FitFunctor_t::N_PARAMETERS))
          , m__fitter(0)
          , m__fitData(0)
      {}

      /// Destructor
      ~FitLM_Adapter() {}

      using FitLM::chiSquared;
      using FitLM::f_vec;
      using FitLM::f_jac;

      /// Perform a nonlinear least-squares fit.
      /**
       * Perform a nonlinear least-squares fit using the Levenberg-Marquardt
       * algorithm.
       *
       * Most of the arguments are identical to those in \c FitLM::operator(),
       * as is the return value.
       *
       * \param theModel
       * Functor object of type \c F.  A pointer to it is held for the
       * duration of this function call.
       *
       * \param theData
       * Data container object of type \c D.  A pointer to it is held for the
       * duration of this function call.  Will be passed to \a theModel
       * by the LM algorithm.
       *
       * \param params
       * The vector of tunable parameters.  Element order is never changed.
       * I.e. - \a theModel will receive it in the same order that you pass it
       * here.
       *
       * \param errtol
       *
       * \param ptol
       *
       * \param factor
       *
       * \param maxiter
       * The maximum number of times that the algorithm calls \a theModel to
       * recompute the error-vector
       */
      FitStatus_t operator()(FitFunctor_t& theModel,
                             const Data_t& theData, dvector_t& params,
                             double errtol, double ptol,
                             int maxiter, double factor)
      {
          m__fitter = &theModel;
          m__fitData = &theData;
          m__activeThis = this;
          FitStatus_t retval
              = this->FitLM::operator()(theData.size(), params, errtol,
                                        ptol, maxiter, factor);
          m__activeThis = 0;
          m__fitData = 0;
          m__fitter = 0;
          return retval;
      }

  private:
      static const Self_t* m__activeThis;
      FitFunctor_t* m__fitter;
      const Data_t* m__fitData;

      // Assignment Operator
      FitLM_Adapter& operator=(const FitLM_Adapter& other);
  };
  template<typename F, typename D>
  const FitLM_Adapter<F,D>*
  FitLM_Adapter<F,D>::m__activeThis=0;

 }; //end namespace
}; //end namespace


#endif //_FitLM_Adapter_H_
/////////////////////////
//
// End
