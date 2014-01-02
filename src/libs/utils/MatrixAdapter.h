// -*- C++ -*-
// Header file for template class MatrixAdapter
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
// RCS $Id: MatrixAdapter.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _MatrixAdapter_H_
#define _MatrixAdapter_H_

// Includes
//
#include <boost/call_traits.hpp>
#include "details/ContainerType.h"


// Forward Declarations
//
#include <iosfwd>


// Enclosing namespace
//
namespace jpw_math {
 // Using decls.
 //
 using std::size_t;


 //
 // Forward Declarations
 //

 /**
  * Policy classes
  *
  * Used with the MatrixAdapter template class.
  */
 namespace policy {
  /**
   * Policy Class for how the native sequence is stored.
   *
   * The SequenceStorage policy class templates provide a wrapper around the
   * quirks of different sequential storage types.  It takes a single template
   * argument, \a S, which should be an STL sequence (like
   * <tt>std::vector\<...\></tt>), the \c boost::array<...> sequence or a
   * POD-pointer array type.
   *
   * All specializations of the SequenceStorage policy class template must
   * define the following static member functions:
   *
   * - c create(size_t) provides an Adapter to different forms of \a S
   * constructors.  Specializations may have different return types.
   *
   * - \c destroy(ref_vec_t) will be a no-op for class types.  It's only needed
   * by POD-pointer array types to manage memory.
   *
   * - \c assign() will usually just call \c S::operator=().
   * For classes that do not perform a deep copy, specialize this function.
   * Specializations for POD-pointer array types must manage memory as needed.
   *
   * - <tt>new_copy_of(constref_vec_t, size_t)</tt> should perform a deep-copy
   * of the vector passed to it.  The caller is expected to manage any memory
   * returned to it (if it returns a pointer type, that is).  It will usually
   * just return via the copy-c'tor.
   *
   * - <tt>template\<typename ITER\> new_copy_of(ITER begin, ITER end)</tt>
   * is an overloaded template function that should take two iterators
   * defining the sequence to copy.
   *
   * - <tt>swap(ref_vec_t, ref_vec_t)</tt> function should call the native \c
   * swap() member.  If there is no native \c swap() member function in \c
   * ref_vec_t, this function should be specialized to manually swap the two
   * arguments.
   */
  template<class S> struct SequenceStorage;

  /**
   * Partial Specialization for \a S of pointer type.
   */
  template<class S> struct SequenceStorage<S*>;
 };

 /// Class MatrixAdapter
 /**
  * A Adapter class for accessing a 1-D sequence as if it were a 2-D matrix.
  *
  * It is designed for efficient access, not super-flexibility.  The indices
  * are 0-based, and cannot be altered.  The leading index selects the row,
  * i.e. indexing is C-style.  You cannot change it to FORTRAN style.  If you
  * need features like that, see the boost library for more powerful classes.
  * (Those classes are, however, also more slow.)
  *
  * To use this class, you must also
  * <tt>\#include&nbsp;"details/MatrixAdapter.tcc"</tt> in the translation
  * unit instantiating the \c MatrixAdapter\<...\> template.
  *
  * The template argument, \a S, can be any form of sequence class or pointer
  * to POD type.  \a S must, at minimum, define an \c operator[].
  *
  * To minimize assumptions on \a S, there are other template arguments, for
  * specifying policy classes.
  *
  * The remaining template arguments are all policy classes.
  *
  * \a STOR_POL describes what kind of behaviors the storage type, \a S,
  * guarantees.  Usually, you'll use the default, \c policy::SequenceStorage.
  * Specialize this class or use it as a model if you need to "roll your own."
  *
  * This class internally uses the \c traits::ContainerType class template to
  * determine the data type stored in \a S.  If you have custom requirements,
  * specialize \c traits::ContainerType for your needs.
  */
 template<class S,
          template <class> class STOR_POL=policy::SequenceStorage>
 class MatrixAdapter
 {
 private:
     struct RowProxy;
     friend class RowProxy;

     typedef STOR_POL<S> storage_policy_t;

 public:
     typedef S vector_type;
     typedef typename traits::ContainerType<S>::value_type value_type;
     typedef typename traits::ContainerType<S>::reference reference;
     typedef typename traits::ContainerType<S>::const_reference
     const_reference;
     typedef typename boost::call_traits<vector_type>::const_reference
     const_vector_type_reference;

     /// Default Constructor
     /**
      * Creates a 0x0 matrix.
      *
      * Clearly such an object is useless.  Don't even think about trying to
      * use \c operator[] or \c at() with it.
      *
      * All other constructors are guaranteed to create matrix objects with
      * nonzero dimensions.  Thus, you can uniquely identify any
      * default-constructed instances of this class.
      *
      * To "insert" data into a default-constructed instance of a \c
      * MatrixAdapter, you can:
      * -# Use \c operator=() to copy an existing \c MatrixAdapter into it.
      *    You don't want to do this, however:
      *    \code
      *    typedef MatrixAdapter<somesequence_t> matrix_t;
      *    matrix_t mt;
      *    :
      *    :
      *    mt = matrix_t(42, 24);
      *    \endcode
      *    It won't perform as well as this:
      * -# Use \c swap() with a temporary \c MatrixAdapter instance.  This is
      *    most useful for "copy-constructing into" the empty matrix, like so:
      *    \code
      *    typedef MatrixAdapter<somesequence_t> matrix_t;
      *    matrix_t mt;
      *    :
      *    :
      *    matrix_t nuDim_tmp(42, 24);
      *    mt.swap(nuDim_tmp);          // <-- Fast!
      *    \endcode
      */
     MatrixAdapter();

     /// Destructor
     ~MatrixAdapter();

     /// Copy Constructor
     MatrixAdapter(const MatrixAdapter& other);

     /// The Standard Constructor
     /**
      * Creates a matrix with the specified dimensions.  The elements of the
      * matrix will all be default-constructed.
      *
      * This is the constructor you'll use most of the time.
      */
     MatrixAdapter(size_t n_rows, size_t n_columns);

     /// Conversion/Data-Loading constructor.
     /**
      * Creates a new matrix using the data in the sequence defined by \a
      * begin and \a end by "reshaping" it to the specified dimensions.
      *
      * The new reshaped matrix satisfies
      * '<code>m[i][j]&nbsp;=&nbsp;v_m[i*m.nColumns()+j]</code>', where \c v_m
      * is the sequence bound by \a begin and \a end.
      *
      * Note that this c'tor should not be used when \a S is a POD-pointer
      * array type.  It will fail to compile if you try to call it.
      */
     template<typename INPUT_ITERATOR>
     MatrixAdapter(INPUT_ITERATOR begin, INPUT_ITERATOR end,
                  size_t n_rows, size_t n_columns);

     /// Assignment Operator
     MatrixAdapter& operator=(const MatrixAdapter& other);

     /// Returns the number of rows in this MatrixAdapter.
     size_t nRows() const { return m__nrows; }

     /// Returns the number of columns in this MatrixAdapter.
     size_t nColumns() const { return m__ncols; }

     /// The cannonical swap member function.
     /**
      * For STL Sequences and classes based on them, this should be an effcient
      * operation.  Other forms of sequence (e.g. C-style arrays and \c
      * std::valarray) may not be.
      */
     void swap(MatrixAdapter& other);

     /// Returns the underlying 1-D data.
     /**
      * This MatrixAdapter class organizes its elements in "typical C
      * fashion".  That is, the indices are 0-based, and the last index (the
      * columns) vary the fastest.  Element access of the form \tt m[i][j] is,
      * therefore, equivalent to indexing \tt v_m[i*m.nColumns()+j] on the
      * underlying 1-D container, \tt v_m .
      *
      * There are several instances where you will wish to or even need to
      * access the underlying 1-D container object.  You can do so here
      * ... but only for non-modifying operations.
      */
     const_vector_type_reference as_1D() const { return m__data; }

     /// Equivalence relation
     bool operator==(const MatrixAdapter& other) const;

     /// Returns !op==(other).
     bool operator!=(const MatrixAdapter& other) const
     { return !operator==(other); }

     /// Returns the total number of elements in the MatrixAdapter.
     size_t size() const { return m__nrows*m__ncols; }

     /// Returns size() == 0.
     /**
      * Because matrices have an intrinsic size, this function really tells
      * you if the \c MatrixAdapter was default-constructed or not.
      */
     bool empty() const {return size()==0; }

     /// Indexes a row of the MatrixAdapter.
     /**
      * This operator allows you to write expressions like:
      * \code
      * MatrixAdapter<vector<double> > m = some_MatrixAdapter_operation();
      * m[2][7] = 3.141;
      * \endcode
      * Note that the indices are all 0-based.
      *
      * It returns an intermediate type that is not meant to be used
      * directly.  In fact, it's not meant to be seen \em at \em all, but
      * (hopefully) will be optimized away by the compiler.
      */
     RowProxy operator[](size_t row_idx) {
         return RowProxy(*this, row_idx*m__ncols);
     }

     /// Indexes a row of the MatrixAdapter.
     /**
      * Overloaded version for accessing MatrixAdapter elements in a
      * RHS-expression.
      *
      * Please read the documentation for the other version of this function.
      */
     const RowProxy operator[](size_t row_idx) const {
         // Only the temporary RowProxy must be const.
         return RowProxy(const_cast<MatrixAdapter&>(*this),
                         row_idx*m__ncols);
     }

 protected:
     size_t m__nrows;
     size_t m__ncols;
     vector_type m__data;

 private:
     /**
      * This is the "classic" proxy class for accessing columns of a row of a
      * MatrixAdapter.  Most compilers will (or should) optimize this away,
      * converting \tt m[i][j] into \tt m.as_1D()[i*m.nColumns()+j] .
      *
      * Trying to cache a RowProxy in order to speed up element access
      * hasn't worked.  (Tests showed no real performance improvement from
      * doing this.)  Furthemore, attempting to store a RowProxy could have
      * all manner of side-effects.
      *
      * Best to leave it alone and let the compiler optimize it away.
      */
     struct RowProxy
     {
         explicit RowProxy(MatrixAdapter& m, size_t row_offset)
             : m__theMatrix(m)
             , m__row_offset(row_offset)
         {}

         reference operator[](size_t column_idx) {
             return m__theMatrix.m__data[m__row_offset+column_idx];
         }

         const_reference operator[](size_t column_idx) const {
             return m__theMatrix.m__data[m__row_offset+column_idx];
         }

     private:
         MatrixAdapter& m__theMatrix;
         size_t m__row_offset;
     };
 };


 template<class S, template <class> class P>
 std::ostream& operator<<(std::ostream& ostr, const MatrixAdapter<S,P>& m);


// Omit:  #include "MatrixAdapter.tcc"
// To speed compilation, please explicitly instantiate instances of this
// template class in a single translation unit.  That T.U. should include the
// *.tcc file.
// Then, write a separate "fwd" header to declare your specializations
// (templates and their specializations all have external linking by default,
// so the "extern" keyword, while informative, isn't strictly required).

}; //end namespace


#endif //_MatrixAdapter_H_
/////////////////////////
//
// End
