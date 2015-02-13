// -*- C++ -*-
// Implementation of SequenceStorage policy class templates
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
// RCS $Id: SequenceStorage.h 2184 2011-06-03 03:34:13Z candide $
//
#ifndef _SequenceStorage_H_
#define _SequenceStorage_H_


// Includes
//
#include <boost/call_traits.hpp>
#include "details/ContainerType.h"


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

  //
  // Default Implementation.  (Documented in MatrixAdapter.h)
  //
  template<class S>
  struct SequenceStorage
  {
      typedef S vec_t;
      typedef typename boost::call_traits<S>::reference ref_vec_t;
      typedef typename boost::call_traits<S>::const_reference
      constref_vec_t;

      /// Returns \a size
      /**
       * Use with \a S that have a native constructor which takes a \c
       * size_t.
       */
      static size_t create(size_t size) { return size; }

      /// NoOp.
      /**
       * Use with \a S that have a native destructor.
       */
      static void destroy(ref_vec_t) { }

      /// Performs <tt>targ = src</tt>.
      /**
       * Assumes the \c operator=() of \a S performs a deep-copy.
       */
      static void assign(ref_vec_t targ, constref_vec_t src, size_t)
      { targ = src; }

      /// Returns \a other
      /**
       * Assumes the copy-c'tor of \a S performs deep-copy.
       */
      static vec_t new_copy_of(constref_vec_t other, size_t)
      { return other; }

      /// Returns copy of sequence defined by the iterators.
      /**
       * Assumes the corresponding c'tor of \a S performs deep-copy.
       */
      template<typename ITER>
      static vec_t new_copy_of(ITER begin, ITER end)
      { return vec_t(begin, end); }

      /// Element-by-element equality comparison.
      /**
       * Merely calls \c (a == b).  This will not produce the expected results
       * unless an operator==() exists for the template type parameter, \c S.
       */
      static bool equivalent(constref_vec_t a, constref_vec_t b, size_t)
      { return (a == b); }

      /// Swaps \a a and \a b
      /**
       * Merely calls \c a.swap(b).
       */
      static void swap(ref_vec_t a, ref_vec_t b) { a.swap(b); }
  };


  //
  // Specialization for POD-Pointer Sequences.
  //
  template<class S>
  struct SequenceStorage<S*>
  {
      typedef typename traits::ContainerType<S*>::value_type val_t;
      typedef val_t* vec_t;  // Pointer OK
      typedef typename boost::call_traits<vec_t>::reference ref_vec_t;
      typedef typename boost::call_traits<vec_t>::const_reference
      constref_vec_t;

      /// Returns a pointer to a new \c S* C-style array.
      /**
       * Assumes that the caller will take complete control of the pointer
       * returned.
       *
       * \note The caller must free the returned pointer using
       * <tt>delete[]</tt>, not the ordinary version of \c delete.
       */
      static vec_t create(size_t size) { return new val_t[size]; }

      /// Deletes \a vec.
      /**
       * Used with pointers to POD-arrays.
       */
      static void destroy(ref_vec_t vec) { delete[] vec; }

      /// Create a deep-copy of \a other.
      /**
       * The caller becomes responsible for freeing the returned array (which
       * is returned as a pointer-to-"val_t", since we can't return a
       * val_t[]).
       *
       * \note The caller must free the returned pointer using
       * <tt>delete[]</tt>, not the ordinary version of \c delete.
       */
      static vec_t new_copy_of(const vec_t other, size_t other_size)
      {
          vec_t newVec(create(other_size));
          for(unsigned i=0; i<other_size; ++i) {
              newVec[i] = other[i];
          }
          return newVec;
      }

      /// Returns copy of sequence defined by the iterators.
      /**
       * Assumes the corresponding c'tor of \a S performs deep-copy.
       */
      template<typename ITER>
      static vec_t new_copy_of(ITER begin, ITER end)
      { return new_copy_of(begin, (end<begin ? 0 : end-begin)); }

      /// Performs copy-assignment.
      /**
       * Uses the version of \c new_copy_of() found in this specialization to
       * perform the deep-copy of \a src.  Handles any needed deallocations,
       * thereby preventing memory leaks.
       */
      static void assign(ref_vec_t targ, constref_vec_t src, size_t src_size)
      {
          if(targ == src) {
              // Ignore self-assignment attempts.
              return;
          }
          vec_t copy_of_src(new_copy_of(src, src_size));
          vec_t oldtarg(targ);
          targ = copy_of_src;
          destroy(oldtarg);
      }

      /// Element-by-element equality comparison.
      /**
       * You must check that \a a and \a b have at least \a size_both elements.
       * this implementation only compares the elementst of \a a and \a b to
       * each other, not their sizes.
       */
      static bool equivalent(constref_vec_t a, constref_vec_t b,
                             size_t size_both)
      {
          if(a == b) {
              // Trivial case:  self-comparison.
              return true;
          }

          for(size_t i=0; i<size_both; ++i) {
              if (a[i] != b[i]) {
                  return false;
              }
          }
          return true;
      }

      /// Swaps \a a and \a b.
      /**
       * Performs a manual swap using \c S::operator=() (which must exist for
       * this policy to work correctly.
       */
      static void swap(ref_vec_t a, ref_vec_t b) {
          if(a == b) {
              // Ignore self-swap attempts.
              return;
          }
          vec_t tmp(a);
          a = b;
          b = tmp;
      }
  };

 };
}; //end namespace jpw_math::policy


#endif //_SequenceStorage_H_
/////////////////////////
//
// End
