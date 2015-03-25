The Base-Library
================

 

This is the base-library for the code from my doctoral-research.  All
common- and general-purpose-classes reside here.

Now, the source code provided here is only a subset of the
full-codebase.  I've only included the dependencies of the other three
libraries.  So, you don't *really* need to read this file and examine
this library.  However, if you're interested, read on.

The code included here contains...
- Forward-declaration headers (so that you only pull in the full
  declaration and, more importantly, inline-definitions when you need
  it).
- Custom `iostream` manipulators.
- Tools for performing various statistics.
- Tools for initializing the pRNG with a known, reusable seed.
- A bunch of trivial `inline` functions that replaced
  preprocessor-macros in the old C-version of this code.
- A custom matrix class and its support classes.

The latter is the most interesting.  It wraps a 1-D sequence in an
interface that looks like a 2-D matrix, using policy classes to allow
the MatrixAdapter template to use an arbitrary sequence-type for its
storage.  Have a look at the source code of the following files, in
order:
- `MatrixAdapter.h`
  + Uses the standard technique of a "row-proxy" helper-class.
- `details/MatrixAdapter.tcc`
  + You should examine this file at the same time as
    `details/SequenceStorage.h`.  Look at them side-by-side.
- `details/SequenceStorage.h`
  + As noted above.
- `Matrix.h`
  + Subclass of `MatrixAdapter.h` using a `std::vector` for its
    storage.
  + Adds on some other useful member functions.
- `details/Matrix.tcc


Why create a custom matrix class?  Why not use Boost or some other
3rd-party library?


Well, I actually did try that.  My original research code used a
C-style "matrix type" (really, a 2-D C array) that used
helper-functions from the well-known text, "Numerical Recipes".
I wanted to replace that code with a C++ class that had close to the
same performance as the C-style 2-D array.

So, I performance-tested several different matrix classes, including
the one from Boost (circa 2005) and the one you see here.  I also
wanted to see if a matrix-adapter based on a `std::vector` performed
any worse than one based on a POD-array.  I compared all of them to
the performance of a C-style 2-D array.

The results were interesting.

1. Boost's matrix class didn't perform as well, most likely because it
   tried to be highly flexible and was designed for
   matrix-mathematics.

   I didn't need that.  I required efficient repeated access.

   (Note:  I did these tests in 2004-2005; Boost now has a uBLAS
    implementation that might order better performance.  Or it might
    not; I'd have to redo my performance tests to find out.  I have
    other things to do in this project that are higher priority than
    switching matrix-classes again.)

2. The MatrixAdapter performed best, but using a POD-array for the
   storage had nearly the same performance as using a `std::vector`.

I chose to keep the `MatrixAdapter` template-classes and its
`SequenceStorage` policy-class after the performance testing.
Specializing it to create the `Matrix` class was the least effort, of
course.  However, it's also a good example of modern C++ design (a la
Andrei Alexandrescu).
