// -*- C++ -*-
// Header file for class FitLM_Adapter
//
// Copyright (C) 2007-2009 by John Weiss
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
  * The fit-functor, \c F, must have the following members
  * - <tt>public static const</tt> \em INT_TYPE <tt>N_PARAMETERS</tt><br/>
  * Will be \c static_cast to an unsigned integer type.
  * - <tt>void operator()(int neq, const D\& fitData,
  *   <br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *                       int nvar, fort_dvec_t xvec, fort_dvec_t fvec,
  *   <br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  *                       fort_dmat_t fjac, int ldfjac, int iflag)</tt>
  * <br/>
  * The parameters for the functor are as described in the documentation for
  * \c FitLM::fit_function_ptr_t, with \a neq corresponding to \a mm and \a
  * xvec corresponding to \a x.  The typedefs \c fort_dvec_t and \c
  * fort_dmat_t are defined in "FitLM.h".  The \a fitData arg. has no
  * equivalent in \c FitLM::fit_function_ptr_t.
  *
  * The data container, \c D, must contain the following members:
  * - \c public \em INT_TYPE <tt>size() const</tt><br/>
  * Should return the size of the current dataset.  Will be passed as the
  * first arg to FitLM::operator();
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
       * \param xv
       * \param errtol
       * \param ptol
       * \param factor
       * \param maxiter
       */
      FitStatus_t operator()(FitFunctor_t& theModel,
                             const Data_t& theData, dvector_t& xv,
                             double errtol, double ptol,
                             int maxiter, double factor)
      {
          m__fitter = &theModel;
          m__fitData = &theData;
          m__activeThis = this;
          FitStatus_t retval
              = this->FitLM::operator()(theData.size(), xv, errtol,
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
