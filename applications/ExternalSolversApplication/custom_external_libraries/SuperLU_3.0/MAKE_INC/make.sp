############################################################################
#
#  Program:         SuperLU
#
#  Module:          make.inc
#
#  Purpose:         Top-level Definitions
#
#  Creation date:   October 2, 1995
#
#  Modified:	    February 4, 1997  Version 1.0
#		    November 15, 1997 Version 1.1
#		    September 1, 1999 Version 2.0
#
############################################################################
#
#  The machine (platform) identifier to append to the library names
#
PLAT = _sp

#
#  The name of the libraries to be created/linked to
#
TMGLIB       = libtmglib.a
SUPERLULIB   = libsuperlu_3.0.a
# 
# If you don't have ESSL, you can use the following blaslib instead:
#           BLASLIB = -lblas -lxlf -lxlf90
# which may be slower than ESSL
#
BLASDEF	     = -DUSE_VENDOR_BLAS
BLASLIB      = -lessl

#
#  The archiver and the flag(s) to use when building archive (library)
#  If your system has no ranlib, set RANLIB = echo.
#
ARCH         = ar
ARCHFLAGS    = cr
RANLIB       = ranlib

CC           = xlc
CFLAGS       = -O3 -qarch=pwr3 -qalias=allptrs
FORTRAN	     = xlf
FFLAGS       = -O3 -qarch=pwr3
LOADER       = xlc
LOADOPTS     = -bmaxdata:0x80000000
#
#  C preprocessor defs for compilation for the Fortran interface
#  (-DNoChange, -DAdd_, -DUpCase, or -DAdd__)
#
CDEFS        = -DNoChange
#
# The directory in which Matlab is installed
#
MATLAB	     = /usr/local/matlab


