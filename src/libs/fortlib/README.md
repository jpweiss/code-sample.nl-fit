C++ Facades for BLAS and LMDER
==============================

 

This directory contains C++ code that acts as a Facade to parts of the
BLAS and LMDER libraries.

All of the `*.f` files here come from those two libraries **and are
NOT MY code!**


> BLAS and LMDER are two FORTRAN libraries widely-used by the
> scientific community (hence why they're written in FORTRAN).
> Only the parts of BLAS and LMDER that I required for my research are
> here.  The actual libraries are far more extensive.


**WARNING**:

This library is **not** **reentrant!!!**  It cross-links to FORTRAN
libraries whose thread-safety is completely unknown.


---


### `FitLM`:  Motivation for `FitLM_Adapter` ###


`FitLM_Adapter` is, as the name implies, an application of the Adapter
pattern to the class `FitLM`.


`FitLM` is a lightweight functor style wrapper-class around calls to
the FORTRAN library, LMDER, which performs nonlinear least-squares
fitting.  Not only must `FitLM` handle the “impedance-mismatch”
between C/C++ and FORTRAN, it tries to whittle away at the complexity
of the LMDER library.  The main function in LMDER takes **24**
arguments and returns a status code with 8 possible values.  Moreover,
the first of those 24 arguments is an 8-argument C-function-pointer
that is actually used in LMDER as a FORTRAN function.  `FitLM` reduces
this mess down to 8 arguments (spread between the constructor and the
functor call) and converts the return status to an enum with
self-explanatory tag names.

Nevertheless, `FitLM` still has some unsatisfying requirements:

* It still requires a C function-pointer ... not a C++
  function-pointer ... that will be called from a FORTRAN library.

* All data passed to and from LMDER must be in the form of pointers.
  + Passing pointers all over the place gives C++ programmers the
    willies (or should).
  + However, copying the data from these pointers to C arrays to C++
    data structures, and vice-versa, is a performance hit.

* LMDER is much more general-purpose.  To use it to fit a nonlinear
  function to a dataset, one needs to combine the dataset with the
  function you're fitting it to.

  This is a “conceptual impedance-mismatch”, and an unnecessary one,
  at that.


It would be far nicer, and more in line with C++ programming, if we
had a class that did the following:

* Takes a C++ functor that computes the function being fit, and does
  **nothing** else.

* Takes a C++ data structure containing the dataset to fit ...
  **separate** from the function being fit to it and from any other
  dataset.

* Avoids copying the dataset between the C++ object and a C-array.


### `FitLM_Adapter` ###


(Notes:

 This Adapter's main purpose is to separate the model and the dataset.

 It does not completely eliminate pointer-based arrays, due to the
 constraints imposed by the external call to the FORTRAN fn.,
 `lmder_`.
)


Placeholder file.  More to come...


---

TODO:  Complete:

You are reading this file because you've been directed here, as a
prerequisite for examining the `FitLM_Adapter` class.

TBA:  Give an overview of each file and a sense of the structure.

TBA:  Reference the Doxygen, and make sure the Doxy-comments are
complete.

--

TBA:  Describe the non-LMDER stuff here, including why it's here and
why I'm not using something else (along with future plans for pruning
'em).  Include:

--

TBA:  Put this someplace where it makes sense:
