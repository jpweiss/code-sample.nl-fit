// -*- C++ -*-
// Implementation of: jpw_nld::fortlib::square_matrix_inversion
//
// Copyright (C) 2007-2008 by John Weiss
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
MatrixInverters_cc__="RCS $Id: MatrixInverters.cc 1864 2009-08-18 02:50:20Z candide $";


// Includes
//
#include <iostream>
#include <string>
#include <cmath>
#include <Manips.h>
#include "MathTools.h"
#include "Matrix.h"

#include "details/Matrix.tcc"
#include "details/MatrixRef_Writer.h"

#include "FORTLib.h"


//
// Using Decls.
//
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::scientific;
using outManips::reset;


using namespace jpw_nld::fortlib;


//
// Static variables
//


namespace {
 static const double INV_ERRTOL(1.0e-12);
 static const double INV_ERRTOP(5.0e-6);
};


//
// Typedefs
//

typedef jpw_math::details::MatrixRef_Writer<double> dmatrixRef_writer_t;


/////////////////////////


//
// External FORTRAN Functions
//


#ifdef __cplusplus
extern "C" {
#endif
    extern int gaussj_(fort_dmat_t a, fort_ivar_t n, fort_ivar_t np,
                       fort_dmat_t b, fort_ivar_t m, fort_ivar_t mp);

    extern int svdcmp_(fort_dmat_t a, fort_ivar_t m, fort_ivar_t n,
                       fort_ivar_t mp, fort_ivar_t np,
                       fort_dvec_t w, fort_dmat_t v);
#ifdef __cplusplus
}; /* end extern "C" */
#endif


/////////////////////////

//
// General Function Definitions
//


/*
 * Computes the RMS error in the inversion process.  Errors indicate
 * an ill-conditioned matrix. The work array must be a square matrix
 * of rank jpw_math::MAX(dim1,dim2) to prevent overflows. The matrices have
 * dimensions dm[dim1][dim2] and dmi[dim2][dim1].
 */
void inv_err(const dmatrix_t& dm, const dmatrix_t& dmi,
             dmatrix_t& wrk, bool adj, const string& caller)
{
    size_t dim1(dm.nRows());
    size_t dim2(dm.nColumns());
    double err12(0.0);
    double err21(0.0);

    for(unsigned i=0; i<dim1; ++i)
    {
        for(unsigned k=0; k<dim1; ++k)
        {
            wrk[i][k] = 0.0;
            for(unsigned j=0; j<dim2; ++j) {
                wrk[i][k] += dm[i][j]*dmi[j][k];
            }

            if(i == k)
            {
                /* This is here for SVD inversion, which can invert the
                   nonsingular submatrix of a singular matrix.  When it does
                   so, the singular diagonal end up being zero in both the
                   matrix and its inverse, so we ignore zero diagonals when
                   the flag "adj" is set. */
                if(!adj || (wrk[i][k] != 0.0)) {
                    err12 += jpw_math::SQR(wrk[i][k]-1.0);
                }
            } else {
                err12 += jpw_math::SQR(wrk[i][k]);
            }
        }
    }
    err12 /= static_cast<double>(dim1*dim1);
    err12 = sqrt(err12);

    for(unsigned i=0; i<dim2; ++i)
    {
        for(unsigned k=0; k<dim2; ++k)
        {
            wrk[i][k] = 0.0;
            for(unsigned j=0; j<dim1; ++j) {
                wrk[i][k] += dmi[i][j]*dm[j][k];
            }

            if(i == k)
            {
                /* This is here for SVD inversion, as in the previous loop. */
                if(!adj || (wrk[i][k] != 0.0)) {
                    err21 += jpw_math::SQR(wrk[i][k]-1.0);
                }
            } else {
                err21 += jpw_math::SQR(wrk[i][k]);
            }
        }
    }
    err21 /= static_cast<double>(dim2*dim2);
    err21 = sqrt(err21);

    // cout << scientific << err12 << " " << err21 << endl;
    double err = (err12 + err21)/2.0;
    if(err > INV_ERRTOL) {
        cerr << caller
             << "():  Inversion Error="
             << scientific << err;
        if(err > INV_ERRTOP) {
            cerr << ".  Matrix ill-conditioned(?)";
        }
        cerr << reset << endl;
    }
}



/*************************************************************

  The inversion routine wrapers.

  The inverse of the transpose is the transpose of the inverse.
  FORTRAN matrices and C/C++ matrices are related by a transpose, so we'll
  feed in our C/C++ matrix as-is and just swap the dimension numbers on
  input to the Fortran code.

*************************************************************/


/*
 * Computes the inverse of a double-matrix using the fortran routine
 * gaussj_ from Numerical Recipies.
 */
int
square_matrix_inversion::sqmInvert_GJE(const dmatrix_t& dm, dmatrix_t& dm_inv)
{
    size_t dim(dm.nRows());

    /* We need to do a check of the dimensions. */
    if(dim <= 1)
    {
        cerr << "sqmInvert_GJE(): Invalid dimensions: "
             << dim << "." << endl
             << "Aborting."
             << endl << endl;
        exit(-1);
    }

    dmatrix_t unity(dim, dim, 0.0);
    // Copy the matrix to be inverted.
    dm_inv = dm;
    for(unsigned i=0; i<dim; ++i) {
        unity[i][i] = 1.0;
    }

    dmatrixRef_writer_t dm_inv_ref(dm_inv);
    dmatrixRef_writer_t unity_ref(unity);
    int dim_i(dim);
    int stat = gaussj_(dm_inv_ref.c_data(dim, dim), &dim_i, &dim_i,
                       unity_ref.c_data(dim, dim), &dim_i, &dim_i);

    if(stat) {
        cerr << "sqmInvert_GJE(): Singular Matrix" << endl;
    } else {
        inv_err(dm, dm_inv, unity, false, "sqmInvert_GJE");
    }

    return(stat);
}


/*
 * Computes the inverse of a double-matrix via Singular Value
 * Decomposition, checking for ill-conditioned matrices.  Uses
 * svdcmp_ from Numerical Recipies.
 */
int
square_matrix_inversion::sqmInvert_SVD(const dmatrix_t& dm, dmatrix_t& dm_inv)
{
    size_t dim(dm.nRows());

    /* We need to do a check of the dimensions. */
    if(dim <= 1)
    {
        cerr << "sqmInvert_SVD(): Invalid dimensions: "
             << dim << "." << endl
             << "Aborting."
             << endl << endl;
        exit(-1);
    }

    // Some setup: make a transposed-copy of the source-matrix.
    dmatrix_t ut(dim, dim);
    for(unsigned i=0; i<dim; ++i) {
        for(unsigned j=0; j<dim; ++j) {
            ut[i][j] = dm[j][i];
        }
    }

    // Now do the decomposition.  svdcmp_() takes A as its first parameter,
    // and replaces A with U.  The last parameter is V-transpose.  However,
    // we're dealing with Fortran ordering here, so we feed in transpose(A)
    // and get back transpose(U) and V.
    int n = dim;
    int m = dim;
    dvector_t w(dim);
    dmatrix_t vt(dim, dim);
    dmatrixRef_writer_t ut_ref(ut);
    dmatrixRef_writer_t vt_ref(vt);
    int stat = svdcmp_(ut_ref.c_data(dim, dim), &m, &n,
                       &m, &n, &w[0], vt_ref.c_data(dim, dim));
    if(stat)
    {
        dmatrix_t zeros(dim, dim, 0.0);
        dm_inv.swap(zeros);
        return(1);
    }

    /* Truncates any possibly ill-conditioned values. */
    bool warnflg(false);
    for(unsigned i=0; i<dim; ++i) {
        if( (-INV_ERRTOL < w[i]) && (w[i] < INV_ERRTOL) ) {
            w[i] = 0.0;
            warnflg = true;
        }
    }
    if(warnflg) {
        cerr << "sqmInvert_SVD():\tMatrix ill-conditioned or singular" << endl
             << "\t\tAttempting fix [results may be inacurate]." << endl;
    }

    /* Now find the inverse. */
    for(unsigned i=0; i<dim; ++i) {
        for(unsigned j=0; j<dim; ++j) {
            dm_inv[i][j] = 0.0;
            for(unsigned k=0; k<dim; ++k) {
                // Throw out any singular vectors [columns of vee].
                if(w[k] != 0.0) {
                    dm_inv[i][j] += vt[k][i]*ut[k][j]/w[k];
                }
            }
        }
    }

    inv_err(dm, dm_inv, vt, true, "sqmInvert_SVD");
    return(0);
}


/////////////////////////
//
// End
