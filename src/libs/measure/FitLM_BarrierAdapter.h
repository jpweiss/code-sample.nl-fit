// -*- C++ -*-
// Header file for class FitLM_BarrierAdapter
//
// Copyright (C) 2007-2010 by John Weiss
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
// RCS $Id: FitLM_BarrierAdapter.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _FitLM_BarrierAdapter_H_
#define _FitLM_BarrierAdapter_H_

// Includes
//
#include "FitLM_Adapter.h"
#include "PersistenceMap.h"
#include "BarrierModels.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace measure {


 // Class FitLM_BarrierAdapter
 /**
  * Child class of the \c FitLM_Adapter, using the \c BarrierModel and \c
  * PersistenceMap classes.  The \a F_POL_T class is the same policy type
  * taken by the \c BarrierModel class.
  *
  * Note:  Because this header uses "BarrierModels.h", translation units
  * \#including this header should also
  * <tt>\#include&nbsp;"details/BarrierModels.tcc"</tt>.
  */
  template<class F_POL_T=policy::Full>
  class FitLM_BarrierAdapter
      : public fortlib::FitLM_Adapter<BarrierModel<F_POL_T>, PersistenceMap>
  {
  public:
      typedef BarrierModel<F_POL_T> Model_t;
      typedef PersistenceMap Data_t;

  private:
      typedef fortlib::FitLM_Adapter<Model_t,PersistenceMap> Base_t;

  public:
      typedef typename Base_t::FitFunctor_t FitFunctor_t;
      typedef typename Base_t::FitStatus_t FitStatus_t;

      static const double ERROR_CONVERGESPEC;
      static const double PARAM_CONVERGESPEC;
      // NOTE:  MAX_ITERS==0 ==> default to a 100*(ndata+1)
      static const int MAX_ITERS=0;

      /// Default Constructor
      explicit FitLM_BarrierAdapter(tslen_t nData)
          : Base_t(nData)
          , m__theModel(nData)
      {}

      /// Destructor
      ~FitLM_BarrierAdapter() {}

      using Base_t::chiSquared;
      using Base_t::f_vec;
      using Base_t::f_jac;

      /// Perform a nonlinear least-squares fit.
      /**
       * Perform a nonlinear least-squares fit using the Levenberg-Marquardt
       * algorithm.
       *
       * The arguments are identical to those in \c
       * FitLM_Adapter::operator(), as is the return value.  The
       * exception is \a fittedParams, which is just the \c xv parameter of
       * FitLM_Adapter::operator() under a different name.
       *
       * Missing parameters use uniform constant values (specifically, the
       * static constants that are members of this class).
       */
      FitStatus_t operator()(dvector_t& fittedParams, const Data_t& theData,
                             double factor)
      {
          return Base_t::operator()(m__theModel, theData, fittedParams,
                                    ERROR_CONVERGESPEC, PARAM_CONVERGESPEC,
                                    MAX_ITERS, factor);
      }

      /// Returns the \c BarrierModel object used by this class.
      Model_t& theModel()
      { return m__theModel; }

      /// Overloaded version returning a const object.
      const Model_t& theModel() const
      { return m__theModel; }

  private:
      Model_t m__theModel;

      // Assignment Operator
      FitLM_BarrierAdapter& operator=(const FitLM_BarrierAdapter& other);
  };
  template<typename P>
  const double FitLM_BarrierAdapter<P>::ERROR_CONVERGESPEC=1.0e-12;
  template<typename P>
  const double FitLM_BarrierAdapter<P>::PARAM_CONVERGESPEC=1.0e-30;

  /// Perform a Levenberg-Marquardt fit using only the Markov component of the
  /// full barrier model.
  typedef FitLM_BarrierAdapter<policy::MarkovOnly> FitLM_MarkovOnly;

  /// Perform a Levenberg-Marquardt fit using the full barrier model.
  typedef FitLM_BarrierAdapter<policy::Full> FitLM_PBarrier;

 }; //end namespace
}; //end namespace


#endif //_FitLM_BarrierAdapter_H_
/////////////////////////
//
// End
