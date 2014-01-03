// -*- C++ -*-
// Unit Tests for the Matrix facade classes.
//
// Copyright (C) 2006-2010 by John Weiss
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
static const char* const
t_matrix_cc__="RCS $Id: t_matrix.cc 2844 2013-10-15 13:10:32Z candide $";


// Includes
//
#include <iostream>
#include <string>
#include <typeinfo>

#include <boost/test/minimal.hpp>

#include "Matrix.h"
#include "details/Matrix.tcc"


#include <boost/mpl/vector.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/transform.hpp>

#include "RandomDataSrc.h"


//
// Using Decls.
//
using namespace jpw_math;
using std::string;
using std::vector;
using std::exception;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::hex;
using std::dec;
using std::showbase;
using std::noshowbase;
using namespace jpwTools::random;


//
// Static variables
//


//
// Typedefs
//


// For testing purposes:  Instantiate a few Matrix classes:
typedef Matrix<long double> ldMatrix_t;
typedef Matrix<short> sMatrix_t;
typedef Matrix<unsigned long long> ullMatrix_t;

// Next, try instantiating a POD-pointer array version of the MatrixAdapter:
typedef MatrixAdapter<float*> fPODMatrix_t;
typedef MatrixAdapter<long*> lPODMatrix_t;
typedef MatrixAdapter<unsigned*> uPODMatrix_t;


// MPL Typedefs
//
typedef boost::mpl::vector< double,
                            long long,
                            float,
                            int,
                            long double,
                            long,
                            short,
                            string,
                            unsigned char,
                            unsigned long long,
                            unsigned short,
                            unsigned> testing_typelist_t;


/////////////////////////

//
// Quick-n-dirty MatrixAdapter subclass for POD-pointer arrays
//


 template<class C>
 class PODMatrix : public MatrixAdapter<C*>
 {
 public:
     typedef C* vector_type;
     typedef C value_type;
     typedef C* iterator;
     typedef const iterator const_iterator;

 private:
     typedef MatrixAdapter<vector_type> Base_t;
     typedef PODMatrix<vector_type> Self_t;

 protected:
     using Base_t::m__nrows;
     using Base_t::m__ncols;
     using Base_t::m__data;

 public:
     typedef typename Base_t::reference reference;
     typedef typename Base_t::const_reference const_reference;

     PODMatrix(size_t n_rows, size_t n_columns)
         : Base_t(n_rows, n_columns)
     {}
     PODMatrix(const vector_type& v,
               size_t n_rows,
               size_t n_columns)
         : Base_t(v, v+(n_rows*n_columns), n_rows, n_columns)
     {}
     PODMatrix& operator=(const PODMatrix& other)
     {
         this->Base_t::operator=(other);
         return *this;
     }
     size_t max_size() const { return Base_t::size(); }
     reference at(size_t row_idx, size_t column_idx)
     { return Base_t::at(row_idx, column_idx); }
     const_reference at(size_t row_idx, size_t column_idx) const
     { return Base_t::at(row_idx, column_idx); }
 };


/////////////////////////

//
// Meta-algorithm template type for iterating over our testing class.
//


// Fwd. Decl.
template<typename TesterTypeVector, bool done> struct iterate_over_types;


// TesterTypeVector is an mpl::vector of template testing classes which, by
// using mpl::transform, have been instantiated for an mpl::vector of types.
// The template testing class must take 1 arg:  RandomDataSrc&.
template<typename TesterTypeVector,
         bool done = false>
struct iterate_over_types
{

    typedef typename boost::mpl::pop_front<TesterTypeVector>::type
    tester_typevec_remaining_t;

    typedef typename boost::mpl::empty<tester_typevec_remaining_t>::type
    tester_typevec_is_empty_t;

    typename boost::mpl::front<TesterTypeVector>::type m__currentTest;

    iterate_over_types<tester_typevec_remaining_t,
                       tester_typevec_is_empty_t::value>
    m__remainingTests;

    // C'tor and test runner.
    iterate_over_types(RandomDataSrc& xi)
        : m__currentTest(xi)
        , m__remainingTests(xi)
    {}
};


// The partial specialization that terminates iteration.
template<typename TesterTypeVector>
struct iterate_over_types<TesterTypeVector, true>
{
    // NOOP --- the test-typevector is empty at this point.  There's nothing
    // more to do.
    iterate_over_types(RandomDataSrc&)
    {}
};


/////////////////////////

//
// Random Cast Specialization for strings.
//

namespace jpwTools {
 namespace random {
  template<>
  string rand_cast<string>(RandomDataSrc& xi) {
      return xi.text(xi(240)+16);
  }
 };
};


/////////////////////////

//
// General Function Definitions
//


template<class T>
T** createRawData(size_t nRows, size_t nColumns, RandomDataSrc& xi)
{

    T** m = new T*[nRows + 1];
    m[0] = new T[nRows*nColumns + 1];
    for(unsigned ui=1; ui<nRows; ++ui) {
        m[ui] = m[ui-1] + nColumns;
    }

    for(unsigned ui=0; ui<nRows; ++ui) {
        for(unsigned uj=0; uj<nColumns; ++uj) {
            m[ui][uj] = rand_cast<T>(xi);
        }
    }

    return m;
}


template<typename VT>
inline void createRawData(VT& data_vec,
                          size_t nRows, size_t nColumns,
                          RandomDataSrc& l_xi)
{
    typedef typename VT::value_type value_type;
    data_vec.clear();
    for(size_t ui=0; ui<nRows*nColumns; ++ui) {
        data_vec.push_back(rand_cast<value_type>(l_xi));
    }
}


template<class T>
void delete_matrix(T** m)
{
    ::delete [] m[0];
    ::delete [] m;
}


/////////////////////////

//
// Testing Classes
//
// Base Class:
//


template<typename VT, typename MT>
struct testSuite_base
{
    typedef VT vector_t;
    typedef MT matrix_t;
    typedef typename matrix_t::value_type value_type;
//    typedef value_type const* const* const const_raw_matrix_t;
    typedef value_type** raw_matrix_t;

    explicit testSuite_base(RandomDataSrc& l_xi) : xi(l_xi) {}

    bool testBasicOps(size_t nRows, size_t nColumns,
                      const vector_t& data_vec1, const vector_t& data_vec2);
    bool testAccess(size_t nRows, size_t nColumns,
                    const raw_matrix_t raw_data, const matrix_t& test_data);

protected:
    RandomDataSrc& xi;
};


template<typename VT, typename MT>
bool
testSuite_base<VT, MT>::testBasicOps(size_t nRows,
                                     size_t nColumns,
                                     const vector_t& data_vec1,
                                     const vector_t& data_vec2)
{
    cout << "============================================================="
         << endl
         << ":                     Basic Operations                      "
         << endl
         << ":\tType: " << string(typeid(value_type).name())
         << endl
         << ":\tDimensions:  " << nRows << " x " << nColumns
         << endl;

    // Data-Loading C'tor
    matrix_t test_data(data_vec1, nRows, nColumns);
    BOOST_REQUIRE( (data_vec1 == test_data.as_1D()) );

    // Copy C'tor
    matrix_t test_data_dup(test_data);
    BOOST_REQUIRE( (data_vec1 == test_data_dup.as_1D()) );

    // Size Asserts
    BOOST_CHECK( (nRows == test_data.nRows()) );
    BOOST_CHECK( (nColumns == test_data.nColumns()) );
    BOOST_CHECK( (nRows*nColumns == test_data.size()) );

    // op==
    matrix_t m2(data_vec1, nRows, nColumns);
    BOOST_REQUIRE( (m2 == test_data) );
    BOOST_REQUIRE( (test_data_dup == test_data) );

    // The swap() operator.
    // As a bank-shot, sanity-tests op!=()
    matrix_t m3(data_vec2, nRows, nColumns);
    m2.swap(m3);
    // Post Swap, m2 == data_vec2
    BOOST_CHECK( (data_vec2 != test_data.as_1D()) );
    BOOST_CHECK( (data_vec2 == m2.as_1D()) );
    BOOST_CHECK( (m2 != test_data) );
    BOOST_CHECK( (m3 == test_data) );
    // Very rarely, you'll need the following two assertions.  It is highly
    // unlikely that xi() would return the same random data twice in such a
    // short cycle.
    //BOOST_REQUIRE( (data_vec1 != data_vec2) );
    //BOOST_REQUIRE( (m2 != m3) );

    return true;
}


template<typename VT, typename MT>
bool
testSuite_base<VT, MT>::testAccess(size_t nRows, size_t nColumns,
                                   const raw_matrix_t raw_data,
                                   const matrix_t& test_data)
{
    cout << "============================================================"
         << endl
         << ":                      Element Access                       "
         << endl
         << ":\tType: " << string(typeid(value_type).name())
         << endl
         << ":\tDimensions:  " << nRows << " x " << nColumns
         << endl;

    unsigned errs(0);

    for(unsigned ui=0; ui<nRows; ++ui) {
        for(unsigned uj=0; uj<nColumns; ++uj) {
            if(raw_data[ui][uj] != test_data[ui][uj]) {
                ++errs;
                cout << "// ERR: m[" << ui << "][" << uj << "]" << endl;
            }
        }
    }

    if (errs) { BOOST_FAIL("Matrix access"); }

    return true;
}


//
// Testing Class:  vector-based Matrix
//


template<typename T>
struct testSuite_V : public testSuite_base< typename Matrix<T>::vector_type,
                                            Matrix<T> >
{
    typedef T value_type;
    typedef Matrix<T> matrix_t;
    typedef typename Matrix<T>::vector_type vector_t;
    typedef testSuite_base<vector_t, matrix_t> Base_t;

    explicit testSuite_V(RandomDataSrc& l_xi)
        : Base_t(l_xi)
    {
        testBasicOps(Base_t::xi(8)+8, Base_t::xi(8)+8);
        testAccess(Base_t::xi(8)+8, Base_t::xi(8)+8);
    }

    bool testBasicOps(size_t nRows, size_t nColumns)
    {
        vector_t data_vec;
        createRawData<vector_t>(data_vec, nRows, nColumns, Base_t::xi);

        vector_t data_vec2;
        createRawData<vector_t>(data_vec2, nRows, nColumns, Base_t::xi);

        return Base_t::testBasicOps(nRows, nColumns, data_vec, data_vec2);
    }

    bool testAccess(size_t nRows, size_t nColumns)
    {
        value_type** raw_data
            = createRawData<value_type>(nRows, nColumns, Base_t::xi);
        vector_t data_vec(raw_data[0], raw_data[0]+ nRows*nColumns);
        matrix_t test_data(data_vec, nRows, nColumns);
        bool retStat(Base_t::testAccess(nRows, nColumns,
                                        raw_data, test_data));
        delete_matrix(raw_data);
        return retStat;
    }
};


//
// Testing Class:  POD-pointer-based Matrix
//


template<typename T>
struct testSuite_POD : public testSuite_base< typename Matrix<T>::vector_type,
                                              Matrix<T> >
{
    typedef T value_type;
    typedef Matrix<T> matrix_t;
    typedef typename Matrix<T>::vector_type vector_t;
    typedef testSuite_base<vector_t, matrix_t> Base_t;

    explicit testSuite_POD(RandomDataSrc& l_xi)
        : Base_t(l_xi)
    {
        testBasicOps(Base_t::xi(8)+8, Base_t::xi(8)+8);
        testAccess(Base_t::xi(8)+8, Base_t::xi(8)+8);
    }

    bool testBasicOps(size_t nRows, size_t nColumns)
    {
        vector_t data_vec;
        for(unsigned ui=0; ui<nRows*nColumns; ++ui) {
            data_vec.push_back(rand_cast<value_type>(Base_t::xi));
        }
        vector_t data_vec2;
        for(unsigned ui=0; ui<nRows*nColumns; ++ui) {
            data_vec2.push_back(rand_cast<value_type>(Base_t::xi));
        }
        return Base_t::testBasicOps(nRows, nColumns, data_vec, data_vec2);
    }

    bool testAccess(size_t nRows, size_t nColumns)
    {
        value_type** raw_data
            = createRawData<value_type>(nRows, nColumns, Base_t::xi);
        vector_t data_vec(raw_data[0], raw_data[0]+ nRows*nColumns);
        matrix_t test_data(data_vec, nRows, nColumns);
        bool retStat(Base_t::testAccess(nRows, nColumns,
                                        raw_data, test_data));
        delete_matrix(raw_data);
        return retStat;
    }
};


//
// Standalone Testing Functions
//


void test_dMatrix(RandomDataSrc& xi)
{
    typedef Matrix<double> dMatrix;
    typedef dMatrix::vector_type vector_t;

    size_t nRows(xi(8)+8);
    size_t nColumns(xi(8)+8);

    vector_t data_vec;
    // Can't use createRawData<vector_t>(...) here.  We need data whose
    // magnitude is O(1).
    for(size_t ui=0; ui<nRows*nColumns; ++ui) {
        data_vec.push_back(2.*xi.unit() - 1.);
    }
    dMatrix data(data_vec, nRows, nColumns);

    dMatrix data_same(data);

    dMatrix data_e9(data);
    for(size_t i=0; i<nRows; ++i) {
        for(size_t j=0; j<nColumns; ++j) {
            data_e9[i][j] += 1.0e-9*xi.unit();
        }
    }

    dMatrix data_e15(data);
    for(size_t i=0; i<nRows; ++i) {
        for(size_t j=0; j<nColumns; ++j) {
            data_e15[i][j] += 1.0e-15*xi.unit();
        }
    }

    BOOST_REQUIRE( data.equivalent(data_same) );
    BOOST_REQUIRE( data_same.equivalent(data) );

    BOOST_REQUIRE( (data != data_e9) );
    BOOST_REQUIRE( (data != data_e15) );
    BOOST_REQUIRE( (data_e9 != data_e15) );

    BOOST_REQUIRE( data.equivalent(data_e9, 1.0e-8) );
    BOOST_CHECK( data.equivalent(data_e9, 1.0e-9) );

    BOOST_REQUIRE( data.equivalent(data_e15, 1.0e-14) );
    BOOST_CHECK( data.equivalent(data_e15, 1.0e-15) );

    BOOST_CHECK( data_e9.equivalent(data_e15, 1.0e-6) );
}


/////////////////////////

//
// Functions "test_main()"
// {No need for a separate "cxx_main()" when using boost::test, as it will
// perform exception handling.}
//


int test_main(int argc, char* argv[])
{
    // Split off the name of the executable from its path.
    string myName(argv[0]);
    string::size_type last_pathsep = myName.find_last_of('/');
    string myPath;
    if(last_pathsep != string::npos) {
        myPath = myName.substr(0, last_pathsep+1);
        myName.erase(0, last_pathsep+1);
    }

    // Build a vector of strings for the arg list, which is much easier to
    // handle than the old-style char** argv.
    vector<string> argVec;
    argVec.reserve(argc);
    for(int i=1; i < argc; ++i) {
        argVec.push_back(string(argv[i]));
    }


    //
    // The Tests Proper
    //


    using namespace boost;
    using namespace mpl::placeholders;

    RandomDataSrc xi;

    cout << "Seed: " << xi.print_seed() << endl;

    const char* sep="%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
        "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
    cout << sep << endl
         << sep << endl
         << sep << endl << endl
         << "  Testing class jpw_math::Matrix<T> "
         << endl << endl
         << sep << endl
         << sep << endl
         << sep << endl << endl;

    // Apply "testSuite_V<_1>(xi);" to every typename in "testing_typelist_t".

    // First, we create a metafunction expression that applies a type to the
    // testSuite_V<> template class.
    typedef mpl::lambda< testSuite_V<_1> >::type testSuite_V_mf;

    // Now apply each typename in the "testing_typelist_t" to create a
    // sequence of testSuite_V<> types, one for each element in
    // "testing_typelist_t".
    typedef mpl::transform<testing_typelist_t, testSuite_V_mf>::type
        testSuite_V_typelist_t;

    typedef mpl::empty<testSuite_V_typelist_t>::type
        testSuite_V_is_empty_t;

    // Now we need to iterate over each element of "testSuite_typelist_t"
    iterate_over_types<testSuite_V_typelist_t, testSuite_V_is_empty_t::value>
        runner_root_V(xi);


    cout << endl;
    cout << sep << endl
         << sep << endl
         << sep << endl << endl
         << "  Testing class jpw_math::MatrixAdapter<T*>"
         << endl << endl
         << sep << endl
         << sep << endl
         << sep << endl << endl;


    // See above for descriptions of what each of these steps does.
    typedef mpl::lambda< testSuite_POD<_1> >::type testSuite_POD_mf;
    typedef mpl::transform<testing_typelist_t, testSuite_POD_mf>::type
        testSuite_POD_typelist_t;
    typedef mpl::empty<testSuite_POD_typelist_t>::type
        testSuite_POD_is_empty_t;
    iterate_over_types<testSuite_POD_typelist_t,
                       testSuite_POD_is_empty_t::value
        > runner_root_POD(xi);


    cout << endl;
    cout << sep << endl
         << sep << endl
         << sep << endl << endl
         << "  Testing operators in jpw_math::Matrix<double>"
         << endl << endl
         << sep << endl
         << sep << endl
         << sep << endl << endl;


    test_dMatrix(xi);


    return 0;
}


/////////////////////////
//
// End
