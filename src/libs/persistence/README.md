The Persistence Map
===================

 

This library contains a single class:  `PersistenceMap`.


The term "Persistence Map" was coined by another research-group in my
department.  It describes a particular "cyclostationary statistic."

"Cyclostationary Statistics" are ... well, let's talk about,
"cyclostationary," first.


### Stationary & Cyclostationary Data ###


Most of the statistics you are familiar with (e.g. mean, variance)
assume that the data you're working with is, "stationary," i.e. it
doesn't depend on time.  Any change in the data is due to
random-fluctuations, not a deterministic behavior that evolves over
time.  "Cyclostationary," is a modification of this, where the data
*does* have deterministic time-evolution.  The time-dependent behavior
of cyclostationary data, however, is completely periodic, not changing
from one cycle to the next.


Consider these examples:

- A coin-toss ... is stationary.  Computing the average number of
  heads doesn't depend on the time of day, or anything else except for
  the coin itself.

- The brightness-from-the-ground of Polaris, the north-star, is
  stationary ... sort-of.  Stars increase brightness over their
  lifetime.  However, they do so very, *very slowly*, over hundreds of
  millions of years.

  Over a human lifetime, however, Polaris' brightness is effectively
  constant, so measuring it from the ground will depend only on
  atmospheric conditions.  Heck, Polaris' brightness is effectively
  constant over all of recorded human history.

  Also, the brightness of any star, observed from space, should
  only fluctuate due to measurement error.  Hence why I stated,
  "brightness-from-the-ground."  I wanted to include atmospheric
  effects.

- Daily high (or low) temperature is *cyclo*stationary.

  Winter, Spring, Summer, Fall:  the temperature changes with the
  seasons.  However, the temperatures from one Winter to another
  change ... well, how could you tell?  The seasonal-cycle is so
  strong that it would "wash-out" any other change going on.  So, you
  would remove an "average seasonal-cycle" from the high-temperature
  data to look for the non-seasonal changes.

  That "average seasonal-cycle" is a _cyclostationary mean_.  Unlike
  the more familiar (i.e. stationary) average, the cyclostationary
  mean is not a single number, but a list of means, forming the
  single-period of a fixed-length timeseries.


The procedure for computing a Cyclostationary Mean is:

1. Group your data according to the period of its "stationary cycle".
   Using our daily-high-temperature example, you'd turn a sequence of
   temperature data into a 2-D array, with the "short dimension"
   having a length of 365.

   (We're going to ignore February 29 to make the explanation easier.)

2. Now that you have each "January 3rd" in `hitemp[2][j]`, each
   "December 25" in `hitemp[357][j]`, etc., you will average over the
   index `j`.  Here's a sketch in pseudocode:

       foreach(dayIndex) {

           csAvg[dayIndex] = 0.0;

           foreach(j) {
               csAvg[dayIndex] += hitemp[dayIndex][j];
           }

           csAvg[dayIndex] /= nYears;

       }

   You would compute the cyclostationary variance, standard deviation,
   and so on in the same fashion.


### `PersistenceMap`:  Details ###


The "Persistence Map" is a type of cyclostationary
lag-autocorrelation.

I'm not going to define autocorrelation.  Read the [Wikipedia article
on autocorrelation](http://en.wikipedia.org/wiki/Autocorrelation) if
you don't know what it is.

For cyclostationary data, there are two ways to compute an
autocorrelation:

1.  Keep the cycle fixed, and lag the stationary part.
    This is an autocorrelation with the cyclical behavior effectively
    removed.

    In our previous example, you would be "lagging the years."

2.  Treat each "phase of the cycle" as a separate timeseries, and lag
    these "phases".  This is an autocorrelation **of** the cyclical
    behavior with itself.  It creates a square-matrix of
    autocorrelations, with the "phases" corresponding to the columns,
    and the "lags" corresponding to the rows (or vice-versa).

    In our previous example, you would be "lagging the days."


The second form is how one computes the Persistence Map, the
cyclostationary lag-autocorrelation of sea-surface temperature in the
equatorial Pacific.


The `PersistenceMap` class will compute not only the Persistence Map
from the sea-surface temperature data, but of the equivalent of a
Persistence Map from simulated data.  It can use a timeseries with the
annual cycle divided up into months, weeks, or even an arbitrary
division (e.g. a year with 360-"days").

The `PersistenceMap` class also computes some other quantites,
including as computing the "phases" and "lags" in units of "years".
The latter are used by the `measure` library when using the
`FitLM_Adapter` to fit the "barrier-model" to a Persistence Map.  Read
the `src/libs/measure/README.md` for a description of what this
"barrier-model" is.

