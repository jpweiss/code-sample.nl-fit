// -*- C++ -*-
// Header file for template class MatrixRef_Writer
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
// RCS $Id: MatrixRef_Writer.h 2221 2011-07-09 01:56:38Z candide $
//
#ifndef _MatrixRef_Writer_H_
#define _MatrixRef_Writer_H_

// Includes
//
#include <boost/utility.hpp>
#include "nld_exceptions.h"
#include "Matrix.h"
#include "details/Matrix.tcc"


// Enclosing namespace
//
namespace jpw_math {
 namespace details {

  // Class MatrixRef_Writer
  /**
   * Provides a means for optimized write-access to the underlying block of
   * memory containing the elements of a \c Matrix\<C\>.  Use it for
   * cross-language or C-API support.
   *
   * An illustrative example will say it all:
   * \code
   * extern void
   * fortran_matrix_invert_(double* m, int rr, int cc);
   *
   * extern void
   * legacy_C_reader(someHandle_t h, double* m, size_t nn);
   *
   * void invert_matrix(MatrixRef_Writer<double> ref_mw)
   * {
   *     // Do some prepwork
   *     :
   *     :
   *     size_t nRowsNow(ref_mw->nRows())
   *     size_t nColsNow(ref_mw->nColumns())
   *     fortran_matrix_invert_(ref_mw.c_data(nRowsNow, nColsNow),
   *                            nRowsNow, nColsNow);
   * }
   *
   * //static unsigned read_nRows ... defined elsewhere.
   * //static unsigned read_nCols ... defined elsewhere.
   *
   * void readIt(MatrixRef_Writer<double> ref_mw)
   * {
   *     // Do some prepwork, including defining and setting
   *     // someHandle_t rh;
   *     :
   *     :
   *     ref_mw->clear(read_nRows, read_nCols);
   *     legacy_C_reader(rh, ref_mw.c_data(read_nRows, read_nCols),
   *                     ref_mw->size());
   *     :
   *     :
   * }
   *
   * void doExample()
   * {
   *     // Do some prepwork
   *     :
   *     :
   *     Matrix<double> data;
   *     readIt(data);
   *     invert_matrix(data);
   * }
   * \endcode
   *
   * As this example shows, \c MatrixRef_Writer\<C\> takes the place of a
   * non-const \c Matrix\<C\>\& arg.  The function \c
   * MatrixRef_Writer::c_data(), returns a pointer to the block of memory
   * where the \c Matrix\<C\> data starts.  Notice how \c
   * MatrixRef_Writer::c_data() forces you to specify the underlying \c Matrix
   * object's dimensions.  This is by design, since your legacy API will
   * almost certainly require you to pass the dimensions to it, too.
   *
   * You can also use this class with legacy APIs that read in matrix data as
   * a "flattened" 1-D sequence.  Just be sure to resize the underlying \c
   * Matrix object so that its number of rows matches the size of the
   * slowly-varying part of the "flattened" data.
   */
  template<class C>
  class MatrixRef_Writer : private boost::noncopyable
  {
      typedef jpw_math::Matrix<C> matrix_t;
      typedef C value_type;
      typedef typename matrix_t::vector_type vector_type;

  public:
      /// Default Constructor
      /**
       * This is also a conversion constructor.  It's designed to act as a
       * reference-type for \c Matrix function args.
       *
       * It stores a reference to the \c Matrix object, \a m, passed to it.
       * Thus, \a m \b cannot go out of scope before any instances of \c
       * MatrixRef_Writer do.
       */
      MatrixRef_Writer(matrix_t& m)
          : m__refMat(m)
      {}

      /// Destructor
      ~MatrixRef_Writer() {}

      /// Call members of the underlying \c Matrix object.
      matrix_t* operator->() { return &m__refMat; }

      /// \c const overload of op->().
      const matrix_t* operator->() const { return &m__refMat; }

      /// Returns a pointer to the underlying data stored in the \c Matrix
      /**
       * The parameters \a row_size and \a column_size specify the target
       * layout of the data you are storing in the memory address returned.
       *
       * \a row_size must be the "slower moving" index, i.e. \c
       * c_data(rsz,csz)+i*rsz+j is the address of \c m[i][j] .  You cannot
       * guarantee the integrity of the underlying \c Matrix object
       * otherwise.
       *
       * The underlying \c Matrix object will be resized to fit the specified
       * dimensions, if necessary.  Consider any data in the underlying \c
       * Matrix object as lost after calling this function.
       *
       * Use this function \em only with legacy C-APIs, as per Meyers'
       * "Effective STL", and even then, \em only in concert with
       * Matrix::size() on the underlying Matrix object.
       */
      value_type* c_data(size_t row_size, size_t column_size)
      {
          using jpw_nld::SizeMismatchError;

          if((row_size*column_size) > m__refMat.max_size()) {
              throw
                  SizeMismatchError("MatrixRef_Writer<>::c_data(...):  "
                                    "Specified row_size*column_size is too\n"
                                    "large.  Resize the MatrixRef_Writer "
                                    "first.");
          }
          if(row_size > m__refMat.nRows()) {
              throw SizeMismatchError("MatrixRef_Writer<>::c_data(...):  "
                                      "Specified row_size is too large.\n"
                                      "Resize the MatrixRef_Writer first.");
          }
          if(column_size > m__refMat.nColumns()) {
              throw SizeMismatchError("MatrixRef_Writer<>::c_data(...):  "
                                      "Specified column_size is too large.\n"
                                      "Resize the MatrixRef_Writer first.");
          }
          // Only resize when necessary.
          if( (row_size != m__refMat.nRows()) &&
              (column_size != m__refMat.nColumns()) )
          {
              m__refMat.reserve(row_size, column_size);
          }
          return &(m__refMat.m__data[0]);
      }

  private:
      matrix_t& m__refMat;
  };

 }; //end namespace details
}; //end namespace jpw_math


#endif //_MatrixRef_Writer_H_
/////////////////////////
//
// End
