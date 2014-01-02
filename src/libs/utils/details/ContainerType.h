// -*- C++ -*-
// Implementation of ContainerType policy class templates
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
// RCS $Id: ContainerType.h 1864 2009-08-18 02:50:20Z candide $
//
#ifndef _ContainerType_H_
#define _ContainerType_H_


// Enclosing namespace
//
namespace jpw_math {
 /**
  * Type Traits classes
  *
  * Used with the MatrixFacade template class.
  */
 namespace traits {
  /**
   * Default type-traits class.  Assumes that the template parameter \a S is a
   * class with a public member typedef named \c "value_type".  The default
   * definition of this template merely forwards the \c value_type of \a S and
   * provides a few additional convenience typedefs.
   */
  template<class S>
  struct ContainerType
  {
      // N.B. - Need the implementation of this class here, in the header, for
      // the sake of the "Matrix.h" header.  The compiler barfs on it because
      // it can't find ContainerType<>.
      typedef typename S::value_type value_type;
      typedef typename boost::call_traits<value_type>::reference reference;
      typedef typename boost::call_traits<value_type>::const_reference
      const_reference;
  };

  /**
   * Specialization of the \c ContainerType traits template for pointer-based
   * containers.
   */
  template<class S>
  struct ContainerType<S*>
  {
      typedef S value_type;
      typedef typename boost::call_traits<value_type>::reference reference;
      typedef typename boost::call_traits<value_type>::const_reference
      const_reference;
  };
 };
}; //end namespace


#endif //_ContainerType_H_
/////////////////////////
//
// End
