The Libraries
=============

 

As mentioned in both the top-level `README.md` and in `src/README.md`,
A good starting point is the class `FitLM_Adapter`, in the directory
`src/lib/fortlib`.  If you're in a hurry, start by reading the file
`src/lib/fortlib/README.md` before looking at the source code.

The remainder of this file describes what this directory contains.

---

The libraries, each of which resides in its own subdirectory, are:

- utils
  + Base-library.
  + Hasn't been converted completely to C++.
  + The source code provided here is only a subset of what's in the
    full-codebase.  I've only included the dependencies of the other
    three libraries.

- fortlib
  + Depends on `utils` and the BLAS and LMDER libraries.
  + BLAS and LMDER are two FORTRAN libraries widely-used by the
    scientific community (hence why they're written in FORTRAN).  I
    may look for a C/C++ equivalent in the future.
  + The *non*-FORTRAN code are C++ adapters for the FORTRAN calls.
  + This is where the source for `FitLM_Adapter` resides.

- persistence
  + Depends on `utils`.
  + An important part of my doctoral research was a form of
    autocorrelation dubbed a “Persistence Map”.  This library contains
    the source code for generating them.
  + It's present in this code-sample **solely** because it's a
    dependency of the `measure` library.  You don't really need to pay
    any attention to it, unless your curious.

- measure
  + Depends on `utils`, `fortlib` and `persistence`.
  + Fits a “Persistence Map” to a theoretical model used to measure
    certain features.
  + For the purposes of this code-sample, it demonstrates how
    `FitLM_Adapter` is used.  Specifically, the classes in the
    file `BarrierModels.h` define functor-types used by the first
    template-parameter of `FitLM_Adapter`.  The file
    `FitLM_BarrierAdapter.h` binds together those functor-types with
    `FitLM_Adapter`, creating the class whose instances will perform
    the actual optimization.

(Each of these subdirectories, of course, contains its own `README.md`
file.)
