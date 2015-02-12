A Unit Test Using the Boost-MPL and Template-Typelists
======================================================

 

This is a `Boost::Test`-based unit-test for my `Matrix` template-class.
This kind of class lends itself well to unit-testing via
typelist-iteration.

Since this *is* a unit-test for class `Matrix`, you should have a look
at `src/libs/utils/README.md` for where to find its source-code and an
overview of why I decided to "roll my own" class.

`RandomDataSrc.h` comes from a separate C++ library of mine (the file
`libjpwTools.a`).

---

This isn't the cleanest unit-test that I've written with Boost::Test,
but it does showcase what I could do if faced with a template class
requiring complex testing.

Now, I had a few different choices for how I could design this
unit-test, including what I'd use from the Boost-MPL to perform the
typelist-iteration.  I went with a custom type-iterator template
class, as I needed to pass my `RandomDataSrc` to the tests.

(Whenever I write a unit-test that uses randomly-generated test data,
I design the test so that I can reuse the seed from a test run when
debugging.  Creating new instances of `RandomDataSrc` makes this
difficult.  If you look at the code, you'll see that I haven't added
support for passing in a seed.  That's because this unit-test passed
on the first or second run, so I didn't need it.)
