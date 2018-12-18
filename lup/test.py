import numpy
import os
import ctypes

#
#
#

os.system('cc lup.c -fPIC -shared -o lup.lib.so')
luplib = ctypes.cdll.LoadLibrary('./lup.lib.so')
os.system('rm lup.lib.so')

#
#
#

n = 1024

A = numpy.random.randn(n, n).astype(numpy.float32)
b = numpy.ones(n, dtype=numpy.float32)

#
#
#

LU = numpy.zeros(A.shape, dtype=numpy.float32)
p = numpy.zeros(n, dtype=numpy.uint)
x = numpy.zeros(n, dtype=numpy.float32)

retval = luplib.do_lup_factorization(
	ctypes.c_void_p(A.ctypes.data),
	ctypes.c_void_p(LU.ctypes.data),
	ctypes.c_void_p(p.ctypes.data),
	ctypes.c_uint(n),
	ctypes.c_float(10e-10)
)

if retval==1:
	luplib.solve_linear_system(
		ctypes.c_void_p(LU.ctypes.data),
		ctypes.c_void_p(p.ctypes.data),
		ctypes.c_void_p(x.ctypes.data),
		ctypes.c_void_p(b.ctypes.data),
		ctypes.c_uint(n)
	)
else:
	print('* the matrix A is singular, aborting')
	import sys
	sys.exit()

#
#
#

abserr = numpy.max(numpy.abs(b - numpy.dot(A, x)))
print('* max absolute error per component: %.10f' % abserr)

#relerr = numpy.max(numpy.abs(b - numpy.dot(A, x))/(1e-6 + numpy.abs(b)))
#print('* max relative error per component: %.10f' % relerr)