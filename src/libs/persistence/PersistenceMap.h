// -*- C++ -*-
// Header file for barrier data class.
//
// Copyright (C) 2009 by John Weiss
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
// RCS $Id: PersistenceMap.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _BarrierData_H_
#define _BarrierData_H_

// Includes
//
//..//#include <cmath>
//..//#include <vector>
#include <boost/utility.hpp>
#include "jpw_nld.h"
#include "Matrix.h"
#include "MathTools.h"


// Enclosing namespace
//
namespace jpw_nld {
 namespace measure {
  // Using decls.
  //
  using jpw_math::dvector_t;
  using jpw_math::dmatrix_t;


  // PersistenceMap
  /**
   * A container class for a persistence map and its related data.
   *
   * The main reason that it's a \c class and not a \c struct, with all member
   * fields \c public, is to keep the \c m__phases and \c m__lags matrices
   * properly-dimensioned relative to the \c m__map matrix.  Furthermore, any
   * time the \c m__map member is modified, \c m__phases and \c m__lags are
   * updated accordingly.
   *
   * Uses the compiler-generated copy-c'tor and assignment operator.
   *
   * Inherits measure-related constants from \c BarrierMeasure
   *
   * Note:  At present, the internal algorithms for computing the persistence
   * assume that you are generating a square map.  If you call the c'tor, \c
   * wipe() or \c clear() functions with a number of columns different from
   * the number of rows, you won't be able to call \c computeCSStdDev() or \c
   * computePersistence().  Ditto if you pass a non-square matrix to \c
   * swap_map().
   */
  class PersistenceMap
  {
  public:
      typedef dmatrix_t::vector_type::size_type size_type;
      typedef const dmatrix_t::vector_type const_vector_type;

      /// Main constructor.
      /**
       * Preallocates all of the member Matrix containers and constructs the
       * phase \& lag axes.  The latter are both scaled from 0.0 to 1.0.
       *
       * The argument \a n_columns is optional; if omitted or 0, it
       * will be set to \c n_rows.
       */
      explicit PersistenceMap(size_type n_Rows, size_type n_Columns=0)
          : m__map(n_Rows, (n_Columns ? n_Columns : n_Rows))
          , m__phases(n_Rows, (n_Columns ? n_Columns : n_Rows))
          , m__lags(n_Rows, (n_Columns ? n_Columns : n_Rows))
          , m__cs_avg(n_Rows, (n_Columns ? n_Columns : n_Rows))
          , m__cs_stddev(n_Rows, (n_Columns ? n_Columns : n_Rows))
          , m__computedCSStats(false)
          , m__computedPersistence(false)
      {
          fillAxes();
      }

      /// Convenience c'tor that preallocates the 3 member containers and
      /// copies over data from the specified double**.
      ///
      /// This function is deprecated.  It will be removed in a later
      /// version.
      explicit PersistenceMap(const dmatrix_t& otherMap)
          : m__map(otherMap)
          , m__phases(otherMap.nRows(), otherMap.nColumns())
          , m__lags(otherMap.nRows(), otherMap.nColumns())
          , m__cs_avg(otherMap.nRows(), otherMap.nColumns())
          , m__cs_stddev(otherMap.nRows(), otherMap.nColumns())
          , m__computedCSStats(false)
          , m__computedPersistence(false)
      {
          fillAxes();
      }

      /// Required by the \c FitLM_Adapter.
      size_type size() const { return m__map.size(); }

      /// The number of columns in the 3 member containers.
      /**
       * This is also the resolution of the persistence map's
       * lag-direction.
       */
      size_type nColumns() const { return m__map.nColumns(); }

      /// The number of rows in the 3 member containers.
      /**
       * This is also the resolution of the persistence map's
       * phase-direction.
       */
      size_type nRows() const { return m__map.nRows(); }

      /// Fills the \c m__phases and \c m__lags member containers.
      /**
       * Reallocates both if they don't have the same dimensions as the \c
       * m__map container.
       */
      void fillAxes();

      /// Call \c swap() on the member container, \c m__map.
      /**
       * Calls \c m__map.swap( \a other \c );
       *
       * Will also call \c fillAxes() if \a other and \c m__map have different
       * dimensions.
       *
       * After calling this function, both \c emptyCSStdDev() and \c empty()
       * will return \c false.  You'll need to rerun \c computePersistence()
       * [or \c computeCSStdDev()].  Therefore, use this function only to
       * "destructively" extract the persistence map into a \c Matrix.
       */
      void swap_map(dmatrix_t& other)
      {
          bool sizeChanged = ( (m__map.nRows() != other.nRows()) ||
                               (m__map.nColumns() != other.nColumns()) );
          m__map.swap(other);
          if(sizeChanged) {
              fillAxes();
          }
          m__computedCSStats = false;
          m__computedPersistence = false;
      }

      /// Compute the cyclostationary mean and standard deviation.
      /**
       * After calling this function, \c emptyCSStdDev() returns \c false.
       *
       * \param ts_data
       * The timeseries to compute the statistics from.
       *
       * \param reset
       * Boolean flag; default value is \c true.  Set this to \c false to
       * prevent the persistence map data from being rendered invalid.
       * \n \n [This should be the default behavior when calling \c
       * computeCSStdDev().  However, certain logic won't work correctly under
       * this default behavior.  Suppose, for example, that the code uses \c
       * empty() before calling any of the accessors.  If the same code needed
       * to get the old map data after calling \c computeCSStdDev() but before
       * the next call to \c computePersistence(), you'll need to pass \c
       * reset=false.]
       */
      void computeCSStdDev(const dmatrix_t& ts_data, bool reset=true);

      /// Compute the cyclostationary lag-autocorrelation.
      /**
       * If \c computeCSStdDev(ts_data) hasn't been called yet, it will be
       * invoked first.
       *
       * After calling this function, both \c emptyCSStdDev() and \c empty()
       * will return \c false.
       *
       * \param ts_data
       * The timeseries from which to compute the cyclostationary
       * lag-autocorrelation - a. k. a. the persistence.
       *
       * \param reset
       * Boolean flag; default value is \c false.  Set this to \c true to
       * invalidate the existing cyclostationary mean, standard deviation, and
       * persistence.  \c reset=true effectively forces this function to call
       * \c computeCSStdDev(ts_data), whether it needed to be or not.
       */
      void computePersistence(const dmatrix_t& ts_data, bool reset=false);

      /// Check if the internal \c Matrix objects are "empty."
      /**
       * The internal \c Matrix objects are never truly empty, as they have
       * predetermined dimensions and, therefore, preallocated elements.
       *
       * Instead, this function determines whether or not \c computeCSStdDev()
       * and \c computePersistence() have ever been called.
       *
       * When this function returns \c true, the functions \c as_1D(), \c
       * data(), \c csAverage(), and \c csStdDev() will return meaninless
       * data.
       *
       * \see computeCSStdDev()
       * \see computePersistence()
       */
      bool empty() const
      { return !(m__computedCSStats && m__computedPersistence); }

      /// Check if the cyclostationary mean and standard deviation are
      /// "empty."
      /**
       * This is similar to \c empty(), but only checks the state of the
       * internal \c Matrix objects for the cyclostationary mean and standard
       * deviation.
       *
       * When this function returns \c true, the functions \c csAverage(), and
       * \c csStdDev() will return meaninless data.
       *
       * \see computeCSStdDev()
       */
      bool emptyCSStdDev() const { return !m__computedCSStats; }

      /// Calls \c Matrix::wipe on the internal \c Matrix objects.
      /**
       * Does not wipe the two axes \c Matrix objects, however, unless the
       * specified dimensions change the size of the internal \c Matrix
       * objects.
       *
       * In this function, \a n_columns is optional.  If it's zero, it will be
       * set to \a n_rows.
       *
       * After calling this function, both \c emptyCSStdDev() and \c empty()
       * will return \c false.  You'll need to rerun \c computePersistence()
       * [or \c computeCSStdDev()].
       */
      void wipe(size_t n_rows=0, size_t n_columns=0)
      {
          if (!n_columns) {
              n_columns = n_rows;
          }

          bool sizeChanged = ( (m__map.nRows() != n_rows) ||
                               (m__map.nColumns() != n_columns) );
          m__map.wipe(n_rows, n_columns);
          m__cs_avg.wipe(n_rows, n_columns);
          m__cs_stddev.wipe(n_rows, n_columns);
          if(sizeChanged) {
              fillAxes();
          }
          m__computedCSStats = false;
          m__computedPersistence = false;
      }

      /// Calls \c Matrix::clear on the internal \c Matrix objects.
      /**
       * Unlike \c wipe(), this fn. "clears" the two axes \c Matrix objects
       * by invoking \c fillAxes().
       *
       * Unlike \c Matrix::clear, the dimension argument \a n_rows is
       * required.  The argument \a n_columns is optional; if omitted or 0, it
       * will be set to \c n_rows.
       *
       * After calling this function, both \c emptyCSStdDev() and \c empty()
       * will return \c false.  You'll need to rerun \c computePersistence()
       * [or \c computeCSStdDev()].
       *
       * Unless you are resizing the \c PersistenceMap object, this is not the
       * function you are looking for.  Use \c wipe() instead.
       */
      void clear(size_t n_rows, size_t n_columns=0)
      {
          m__map.clear(n_rows, n_columns);
          m__cs_avg.clear(n_rows, n_columns);
          m__cs_stddev.clear(n_rows, n_columns);
          m__computedCSStats = false;
          m__computedPersistence = false;
          fillAxes();
      }

      /// Accessor method.
      /**
       * \returns the persistence map data
       */
      const dmatrix_t& data() const
      { return m__map; }

      /// Accessor method.
      /**
       * \returns the cyclostationary average
       */
      const dmatrix_t& csAverage() const
      { return m__cs_avg; }

      /// Accessor method.
      /**
       * \returns the cyclostationary standard deviation
       */
      const dmatrix_t& csStdDev() const
      { return m__cs_stddev; }

      /// Returns the persistence map as a flat sequence.
      const_vector_type as_1D() const
      { return m__map.as_1D(); }

      /// Returns the phase-axis matrix as a flat sequence.
      const_vector_type phases_as_1D() const
      { return m__phases.as_1D(); }

      /// Returns the lag-axis matrix as a flat sequence.
      const_vector_type lags_as_1D() const
      { return m__lags.as_1D(); }

      /// Exception class for persistence computations.
      /**
       * Thrown if you ...
       * - ...constructed the PersistenceMap with non-square dimensions;
       * - ...passed a non-square matrix to \c swap_map();
       * - ...called \c clear() or \c wipe with non-square dimensions;
       * - ...call \c computePersistence() or \c computeCSStdDev() on a \c
       *   Matrix with a column size different from \c this-\>nColumns().
       */
      struct InconsistentDimensionsError;

  protected:
      /// The persistence map.
      dmatrix_t m__map;
      /// The "phase-axis", in a form usable by the \c BarrierModel.
      dmatrix_t m__phases;
      /// The "lag-axis", in a form usable by the \c BarrierModel.
      dmatrix_t m__lags;
      /// The cyclostationary average.
      dmatrix_t m__cs_avg;
      /// The cyclostationary standard deviation.
      dmatrix_t m__cs_stddev;
      /// Flag to track whether or not \c m__cs_avg and \c m__cs_stddev have
      /// been filled in.
      bool m__computedCSStats;
      /// Flag to track whether or not m__map has been filled in.
      bool m__computedPersistence;

  private:
      bool hasBadDimensions(const dmatrix_t& tsdata)
      {
          return( (tsdata.nColumns() != m__map.nColumns()) ||
                  (m__map.nColumns() != m__map.nRows()) );
      }
  };


 }; //end namespace
}; //end namespace


#endif //_BarrierData_H_
/////////////////////////
//
// End
