Fitting a Nonlinear Model to Data:  `FitLM_Adapter` in Action
=============================================================

 

Placeholder file.  More to come...

Outline:

1. Start with a quick description of the "barrier-model" and a really
   quick reference to the PersistenceMap (with emphasis on what the
   phases_as_1D() and lags_as_1D() are returning (what those
   flattened-matrixes contain)).
2. What the BarrierModels.h functors are doing, in broad strokes.
   + Trying to eliminate code-duplication in the BarrierModel()
     functor class.
   + Also trying to eliminated function-calls or if-blocks.
3. Modern C++ Design through Template-Policy classes.
   What the business with this separate 'calculate<>' is.
4. Why there's both a *.cc and a *.tcc file.
5. How FitLM_BarrierAdapter pulls it all together.
   + There's not much in it other than 3 member constants.
   + The c'tor and op() are just dispatch fns.

Yer gonna notice some cruft in "BarrierModel".
