// -*- C++ -*-
// Header file for template class Matrix
//
// Copyright (C) 2005-2010 by John Weiss
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
// RCS $Id: Matrix.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _Matrix_H_
#define _Matrix_H_

// Includes
//
#include <vector>
#include "MatrixAdapter.h"


// Enclosing namespace
//
namespace jpw_math {
 // Forward Declarations
 //
 namespace details {
  template<class C> class MatrixRef_Writer;
 };

 // Using decls.
 //
 using std::size_t;


 /// Class Matrix
 /**
  * To use this class, you must also
  * <tt>\#include&nbsp;"details/Matrix.tcc"</tt> in the translation unit
  * instantiating the \c Matrix\<C\> template.
  *
  *
  * N.B. - If you \em really need reverse iterator types for this class,
  * typedef them yourself off of the member vector_type typedef.
  */
 template<class C>
 class Matrix : public MatrixAdapter<std::vector<C>,
                                     policy::SequenceStorage>
 {
     friend class jpw_math::details::MatrixRef_Writer<C>;

 public:
     typedef std::vector<C> vector_type;
     typedef C value_type;
     typedef typename vector_type::iterator iterator;
     typedef typename vector_type::const_iterator const_iterator;

 private:
     typedef MatrixAdapter<vector_type,
                           policy::SequenceStorage
                           > Base_t;
     typedef Matrix<value_type> Self_t;

 protected:
     // N.B.:  This is necessary, for some reason.  Even though Matrix
     // inherits these from MatrixAdapter, they're not visible in the member
     // functions unless fully-qualified.
     using Base_t::m__nrows;
     using Base_t::m__ncols;
     using Base_t::m__data;

 public:
     typedef typename Base_t::reference reference;
     typedef typename Base_t::const_reference const_reference;

     // Destructor
     /* == doxycomment disabled ==
      * \fn ~Matrix()
      *
      * Implicitly declared and defined.  No need to do anything beyond what
      * the default implementation would.
      */

     // Copy Constructor
     /* == doxycomment disabled ==
      * \fn Matrix(const Matrix\& other)
      *
      * Implicitly declared and defined.  No need to do anything beyond what
      * the default implementation would.
      */

     /// Standard Constructor
     /**
      * Equivalent to calling the same constructor in the \c MatrixAdapter
      * class.
      */
     Matrix(size_t n_rows, size_t n_columns);

     /// Assignment Constructor
     /**
      * Equivalent to calling the \c Matrix(n_rows,n_columns), then assigning
      * \a value to each element.
      */
     Matrix(size_t n_rows, size_t n_columns, const value_type& value);

     /// Conversion/data-loading constructor.
     /**
      * Equivalent to calling the same constructor in the \c MatrixAdapter
      * class.
      */
     explicit Matrix(const vector_type& v,
                     size_t n_rows=0,
                     size_t n_columns=0);

     /// Assignment Operator
     /**
      * Equivalent to calling MatrixAdapter::operator=().
      */
     Matrix& operator=(const Matrix& other);

     /// Returns the total number of elements the Matrix could
     /// possibly hold.
     size_t max_size() const { return m__data.max_size(); }

     /// Access an element, checking indices.
     /**
      * Overloaded version for use in LHS expressions.  See the documentation
      * of the other version for behavior details.
      *
      * The default implementation of this function is only valid for STL
      * Sequences.  For \c std::valarray, you would need to specialize this
      * member function.
      */
     reference at(size_t row_idx, size_t column_idx);

     /// Access an element, checking indices.
     /**
      * Equivalent to \c as_1D().data()[row_idx*m.nColumns()+column_idx].
      * Note that the indices are all 0-based.
      *
      * (The implementation of this function is only valid for STL Sequences.)
      */
     const_reference at(size_t row_idx, size_t column_idx) const;

     /// An auxillary swap member function.
     /**
      * Swap the underlying vector with \a other.  This should be way more
      * efficient than copying in the individual elements.
      *
      * \a other must be \c nRows()*nColumns() long.  If not, throws a \c
      * std::invalid_argument exception.
      */
     void swap(vector_type& other);

     using Base_t::swap;

     /// Clear and reshape the Matrix.
     /**
      * Clear the underlying \c vector, then reshape the Matrix, if
      * requested.  This is equivalent to:
      * \code
      * oldmatrix = Matrix(n_rows, n_columns);
      * \endcode
      * ...but without any risk of temporary objects being
      * constructed/destroyed.  \b Note, however, that the internal memory
      * backing the underlying \c vector \b is deallocated.
      *
      * If you just want to "erase" the Matrix, consider calling \c fill() or
      * \c wipe().
      *
      * If either \a n_rows or \a n_columns is zero, no reshaping occurs.
      */
     void clear(size_t n_rows=0, size_t n_columns=0);

     /// Fill the Matrix with the specified value, and maybe resize.
     /**
      * Fill every element in the Matrix with \a v then reshape it to \a
      * n_rows and \a n_columns if needed.  If either \a n_rows or \a
      * n_columns is zero, no reshaping occurs.  Similarly, if the specified \a
      * n_rows and \a n_columns are the same as the current dimensions, there
      * is no resizing.
      *
      * Calling this function is far more efficient than calling \c clear(),
      * since the latter ultimately deallocates and reallocates the
      * underlying memory.
      *
      * To "erase" the contents of the Matrix by filling it with 0, call \c
      * wipe().
      */
     void fill(value_type v, size_t n_rows=0, size_t n_columns=0);

     /// "Erase" and possibly reshape the Matrix.
     /**
      * Equivalent to calling:
      * \code
      * fill(value_type(), n_rows, n_columns);
      * \endcode
      */
     void wipe(size_t n_rows=0, size_t n_columns=0)
     {
         fill(value_type(), n_rows, n_columns);
     }

     /// Check if two Matrix objects are equivalent to within some precision.
     /**
      * \param other
      * The Matrix object to compare this one to.
      *
      * \param epsilon
      * A small number.  Each element of the two Matrix objects should differ
      * from each other by no more than this amount.
      */
     bool equivalent(const Matrix& other, double epsilon=1.0e-16) const;

 protected:
     /// For internal use only.
     void reserve(size_t n_rows, size_t n_columns);
 };


// Omit:  #include "Matrix.tcc"
// To speed compilation, please explicitly instantiate instances of this
// template class in a single translation unit.  That T.U. should include the
// *.tcc file.
// Then, write a separate "fwd" header to declare your specializations
// (templates and their specializations all have external linking by default,
// so the "extern" keyword, while informative, isn't strictly required).


 //---------------------------------------------------------------------------


 //
 //  Typedefs
 //


 typedef std::vector<double> dvector_t;
 typedef Matrix<double> dmatrix_t;


} //end namespace


#endif //_Matrix_H_
/////////////////////////
//
// End
