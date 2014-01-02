
      subroutine chkder(m,n,x,fvec,fjac,ldfjac,xp,fvecp,mode,err)
      integer m,n,ldfjac,mode
      double precision x(n),fvec(m),fjac(ldfjac,n),xp(n),fvecp(m),
     *                 err(m)
c     **********
c
c     subroutine chkder
c
c     this subroutine checks the gradients of m nonlinear functions
c     in n variables, evaluated at a point x, for consistency with
c     the functions themselves. the user must call chkder twice,
c     first with mode = 1 and then with mode = 2.
c
c     mode = 1. on input, x must contain the point of evaluation.
c               on output, xp is set to a neighboring point.
c
c     mode = 2. on input, fvec must contain the functions and the
c                         rows of fjac must contain the gradients
c                         of the respective functions each evaluated
c                         at x, and fvecp must contain the functions
c                         evaluated at xp.
c               on output, err contains measures of correctness of
c                          the respective gradients.
c
c     the subroutine does not perform reliably if cancellation or
c     rounding errors cause a severe loss of significance in the
c     evaluation of a function. therefore, none of the components
c     of x should be unusually small (in particular, zero) or any
c     other value which may cause loss of significance.
c
c     the subroutine statement is
c
c       subroutine chkder(m,n,x,fvec,fjac,ldfjac,xp,fvecp,mode,err)
c
c     where
c
c       m is a positive integer input variable set to the number
c         of functions.
c
c       n is a positive integer input variable set to the number
c         of variables.
c
c       x is an input array of length n.
c
c       fvec is an array of length m. on input when mode = 2,
c         fvec must contain the functions evaluated at x.
c
c       fjac is an m by n array. on input when mode = 2,
c         the rows of fjac must contain the gradients of
c         the respective functions evaluated at x.
c
c       ldfjac is a positive integer input parameter not less than m
c         which specifies the leading dimension of the array fjac.
c
c       xp is an array of length n. on output when mode = 1,
c         xp is set to a neighboring point of x.
c
c       fvecp is an array of length m. on input when mode = 2,
c         fvecp must contain the functions evaluated at xp.
c
c       mode is an integer input variable set to 1 on the first call
c         and 2 on the second. other values of mode are equivalent
c         to mode = 1.
c
c       err is an array of length m. on output when mode = 2,
c         err contains measures of correctness of the respective
c         gradients. if there is no severe loss of significance,
c         then if err(i) is 1.0 the i-th gradient is correct,
c         while if err(i) is 0.0 the i-th gradient is incorrect.
c         for values of err between 0.0 and 1.0, the categorization
c         is less certain. in general, a value of err(i) greater
c         than 0.5 indicates that the i-th gradient is probably
c         correct, while a value of err(i) less than 0.5 indicates
c         that the i-th gradient is probably incorrect.
c
c     subprograms called
c
c       minpack supplied ... dpmpar
c
c       fortran supplied ... dabs,dlog10,dsqrt
c
c     argonne national laboratory. minpack project. march 1980.
c     burton s. garbow, kenneth e. hillstrom, jorge j. more
c
c     **********
      integer i,j
      double precision eps,epsf,epslog,epsmch,factor,one,temp,zero
      double precision dpmpar
      data factor,one,zero /1.0d2,1.0d0,0.0d0/
c
c     epsmch is the machine precision.
c
      epsmch = dpmpar(1)
c
      eps = dsqrt(epsmch)
c
      if (mode .eq. 2) go to 20
c
c        mode = 1.
c
         do 10 j = 1, n
            temp = eps*dabs(x(j))
            if (temp .eq. zero) temp = eps
            xp(j) = x(j) + temp
   10       continue
         go to 70
   20 continue
c
c        mode = 2.
c
         epsf = factor*epsmch
         epslog = dlog10(eps)
         do 30 i = 1, m
            err(i) = zero
   30       continue
         do 50 j = 1, n
            temp = dabs(x(j))
            if (temp .eq. zero) temp = one
            do 40 i = 1, m
               err(i) = err(i) + temp*fjac(i,j)
   40          continue
   50       continue
         do 60 i = 1, m
            temp = one
            if (fvec(i) .ne. zero .and. fvecp(i) .ne. zero
     *          .and. dabs(fvecp(i)-fvec(i)) .ge. epsf*dabs(fvec(i)))
     *         temp = eps*dabs((fvecp(i)-fvec(i))/eps-err(i))
     *                /(dabs(fvec(i)) + dabs(fvecp(i)))
            err(i) = one
            if (temp .gt. epsmch .and. temp .lt. eps)
     *         err(i) = (dlog10(temp) - epslog)/epslog
            if (temp .ge. eps) err(i) = zero
   60       continue
   70 continue
c
      return
c
c     last card of subroutine chkder.
c
      end
c
c
c     This is provided in lmder1.f, so there's no need to duplicate it
c     here.
c
c
c      double precision function dpmpar(i)
c      integer i
cc     **********
cc
cc     function dpmpar
cc
cc     This function provides double precision machine parameters
cc     when the appropriate set of data statements is activated (by
cc     removing the c from column 1) and all other data statements are
cc     rendered inactive. Most of the parameter values were obtained
cc     from the corresponding Bell Laboratories Port Library function.
cc
cc     The function statement is
cc
cc       double precision function dpmpar(i)
cc
cc     where
cc
cc       i is an integer input variable set to 1, 2, or 3 which
cc         selects the desired machine parameter. If the machine has
cc         t base b digits and its smallest and largest exponents are
cc         emin and emax, respectively, then these parameters are
cc
cc         dpmpar(1) = b**(1 - t), the machine precision,
cc
cc         dpmpar(2) = b**(emin - 1), the smallest magnitude,
cc
cc         dpmpar(3) = b**emax*(1 - b**(-t)), the largest magnitude.
cc
cc     Argonne National Laboratory. MINPACK Project. June 1983.
cc     Burton S. Garbow, Kenneth E. Hillstrom, Jorge J. More
cc
cc     **********
c      integer mcheps(4)
c      integer minmag(4)
c      integer maxmag(4)
c      double precision dmach(3)
c      equivalence (dmach(1),mcheps(1))
c      equivalence (dmach(2),minmag(1))
c      equivalence (dmach(3),maxmag(1))
cc
cc     Machine constants for the IBM 360/370 series,
cc     the Amdahl 470/V6, the ICL 2900, the Itel AS/6,
cc     the Xerox Sigma 5/7/9 and the Sel systems 85/86.
cc
cc     data mcheps(1),mcheps(2) / z34100000, z00000000 /
cc     data minmag(1),minmag(2) / z00100000, z00000000 /
cc     data maxmag(1),maxmag(2) / z7fffffff, zffffffff /
cc
cc     Machine constants for the Honeywell 600/6000 series.
cc
cc     data mcheps(1),mcheps(2) / o606400000000, o000000000000 /
cc     data minmag(1),minmag(2) / o402400000000, o000000000000 /
cc     data maxmag(1),maxmag(2) / o376777777777, o777777777777 /
cc
cc     Machine constants for the CDC 6000/7000 series.
cc
cc     data mcheps(1) / 15614000000000000000b /
cc     data mcheps(2) / 15010000000000000000b /
cc
cc     data minmag(1) / 00604000000000000000b /
cc     data minmag(2) / 00000000000000000000b /
cc
cc     data maxmag(1) / 37767777777777777777b /
cc     data maxmag(2) / 37167777777777777777b /
cc
cc     Machine constants for the PDP-10 (KA processor).
cc
cc     data mcheps(1),mcheps(2) / "114400000000, "000000000000 /
cc     data minmag(1),minmag(2) / "033400000000, "000000000000 /
cc     data maxmag(1),maxmag(2) / "377777777777, "344777777777 /
cc
cc     Machine constants for the PDP-10 (KI processor).
cc
cc     data mcheps(1),mcheps(2) / "104400000000, "000000000000 /
cc     data minmag(1),minmag(2) / "000400000000, "000000000000 /
cc     data maxmag(1),maxmag(2) / "377777777777, "377777777777 /
cc
cc     Machine constants for the PDP-11.
cc
cc     data mcheps(1),mcheps(2) /   9472,      0 /
cc     data mcheps(3),mcheps(4) /      0,      0 /
cc
cc     data minmag(1),minmag(2) /    128,      0 /
cc     data minmag(3),minmag(4) /      0,      0 /
cc
cc     data maxmag(1),maxmag(2) /  32767,     -1 /
cc     data maxmag(3),maxmag(4) /     -1,     -1 /
cc
cc     Machine constants for the Burroughs 6700/7700 systems.
cc
cc     data mcheps(1) / o1451000000000000 /
cc     data mcheps(2) / o0000000000000000 /
cc
cc     data minmag(1) / o1771000000000000 /
cc     data minmag(2) / o7770000000000000 /
cc
cc     data maxmag(1) / o0777777777777777 /
cc     data maxmag(2) / o7777777777777777 /
cc
cc     Machine constants for the Burroughs 5700 system.
cc
cc     data mcheps(1) / o1451000000000000 /
cc     data mcheps(2) / o0000000000000000 /
cc
cc     data minmag(1) / o1771000000000000 /
cc     data minmag(2) / o0000000000000000 /
cc
cc     data maxmag(1) / o0777777777777777 /
cc     data maxmag(2) / o0007777777777777 /
cc
cc     Machine constants for the Burroughs 1700 system.
cc
cc     data mcheps(1) / zcc6800000 /
cc     data mcheps(2) / z000000000 /
cc
cc     data minmag(1) / zc00800000 /
cc     data minmag(2) / z000000000 /
cc
cc     data maxmag(1) / zdffffffff /
cc     data maxmag(2) / zfffffffff /
cc
cc     Machine constants for the Univac 1100 series.
cc
cc     data mcheps(1),mcheps(2) / o170640000000, o000000000000 /
cc     data minmag(1),minmag(2) / o000040000000, o000000000000 /
cc     data maxmag(1),maxmag(2) / o377777777777, o777777777777 /
cc
cc     Machine constants for the Data General Eclipse S/200.
cc
cc     Note - it may be appropriate to include the following card -
cc     static dmach(3)
cc
cc     data minmag/20k,3*0/,maxmag/77777k,3*177777k/
cc     data mcheps/32020k,3*0/
cc
cc     Machine constants for the Harris 220.
cc
cc     data mcheps(1),mcheps(2) / '20000000, '00000334 /
cc     data minmag(1),minmag(2) / '20000000, '00000201 /
cc     data maxmag(1),maxmag(2) / '37777777, '37777577 /
cc
cc     Machine constants for the Cray-1.
cc
cc     data mcheps(1) / 0376424000000000000000b /
cc     data mcheps(2) / 0000000000000000000000b /
cc
cc     data minmag(1) / 0200034000000000000000b /
cc     data minmag(2) / 0000000000000000000000b /
cc
cc     data maxmag(1) / 0577777777777777777777b /
cc     data maxmag(2) / 0000007777777777777776b /
cc
cc     Machine constants for the Prime 400.
cc
cc     data mcheps(1),mcheps(2) / :10000000000, :00000000123 /
cc     data minmag(1),minmag(2) / :10000000000, :00000100000 /
cc     data maxmag(1),maxmag(2) / :17777777777, :37777677776 /
cc
cc     Machine constants for the VAX-11.
cc
cc     data mcheps(1),mcheps(2) /   9472,  0 /
cc     data minmag(1),minmag(2) /    128,  0 /
cc     data maxmag(1),maxmag(2) / -32769, -1 /
cc
cc
cc     Machine constants for an SGI PowerChallenge R10k
cc
cc     For some reason, the compiler won't recognize hex constants, so
cc     I'm leaving in those values here for historical reasons.  The
cc     decimal equivalent is below.
cc      data mcheps(1),mcheps(2) / x3CA00000, x00000000 /
cc      data minmag(1),minmag(2) / x00100000, x00000000 /
cc      data maxmag(1),maxmag(2) / x7FEFFFFF, xFFFFFFFF /
cc
c      data mcheps(1),mcheps(2) / 1017118720,  0 /
c      data minmag(1),minmag(2) /    1048576,  0 /
c      data maxmag(1),maxmag(2) / 2146435071, -1 /
cc
c      dpmpar = dmach(i)
c      return
cc
cc     Last card of function dpmpar.
cc
c      end
