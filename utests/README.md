One Unit Test
=============

 

As noted in the top-level `README.md`, this repository contains a
subset of the code from my doctoral-research.  I've therefore included
only one of its unit-tests.  (The rest of the unit-tests are tedious,
cryptic, or boring.)

Have a look at the file `templ.classes/README.md` for info about the
unit-test itself.  If you're interested in hearing more about my
testing methodology, read on!

The file `TestException.h` isn't used by `templ.classes`.  However,
I've left in here as an example of one of my unit-testing tools.

---

Ordinarily, I like to write unit-tests for my more complex classes.
*However*, for this project, I chose not to create unit-tests for
every ported file.

Firstly, I'm porting from C to C++, so most of the changes are design
changes.  Secondly, I'm porting an existing codebase, which benefits
more from regression testing than unit-testing.  Since there are no
independent libraries, I can generally use the final programs
themselves to do any regression-testing.  Where I have to
regression-test a library, the test is usually quite tedious and the
code uninteresting.  ;)

Lastly, my research involved modelling nonlinear physical systems —
“nonlinear” as in “Chaos Theory”.  Since the programs generate
“chaotic” data, their output are sensitive to **any**
calculation-errors … even down to machine-precision!  In fact, I have
to be careful when upgrading GCC on my home Linux system.  If the new
compiler ABI introduces changes in the final digit of a `double`, my
regression tests will fail!<br/>
The upshot is that the modelling-software itself is a sufficient
regression-test.
