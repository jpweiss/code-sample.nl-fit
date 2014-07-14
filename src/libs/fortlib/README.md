C++ Facades for BLAS and LMDER
==============================

 

This directory contains C++ code that acts as a Facade to parts of the
BLAS and LMDER libraries.

All of the `*.f` files here come from those two libraries *and are NOT
MY code!*


> BLAS and LMDER are two FORTRAN libraries widely-used by the
> scientific community (hence why they're written in FORTRAN).
> Only the parts of BLAS and LMDER that I required for my research are
> here.  The actual libraries are far more extensive.


Placeholder file.  More to come...

--

FIXME:  Complete:

You are reading this file because you've been directed here, as a
prerequisite for examining the `FitLM_Adapter` class.

FIXME:  Give an overview of each file and a sense of the structure.

FIXME:  Reference the Doxygen, and make sure the Doxy-comments are
complete.

---

FIXME:  Describe the non-LMDER stuff here, including why it's here and
why I'm not using something else (along with future plans for pruning
'em).  Include:

---

FIXME:  Put this someplace where it makes sense:

> This library is **not** **reentrant!!!**  It cross-links to FORTRAN
> libraries whose thread-safety is completely unknown.
