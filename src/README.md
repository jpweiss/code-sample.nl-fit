The Source
==========

 

As noted in the top-level `README.md`, this repository contains a
subset of the code from my doctoral-research.  My research involved
modelling nonlinear physical systems (think “Chaos Theory”).  I've
been porting the parts written in C to modern C++, to try out some of
the more advanced modern C++ programming techniques.  Note that I
started this project before C++11.  Once the compilers catch up, I'll
go back and try out those new features.

I haven't included the entire codebase, just the nonlinear
least-squares fitting library, its dependencies, and the library that
uses it.

A good starting point is the class `FitLM_Adapter`, in the directory
`src/lib/fortlib`.  Start by reading the file
`src/lib/fortlib/README.md` before looking at the source code.
