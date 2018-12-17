import subprocess
import numpy

#
#
#

import os
import ctypes

os.system('cc fft.c -fPIC -shared -o fft.lib.so')
fftlib = ctypes.cdll.LoadLibrary('./fft.lib.so')
os.system('rm fft.lib.so')

#
#
#

log2n = 10
n = 2**log2n

x = numpy.random.rand(2*n).astype(numpy.float32)

#
#
#

X = numpy.zeros(x.shape, dtype=numpy.float32)

fftlib.fft(
	ctypes.c_void_p(x.ctypes.data),
	ctypes.c_void_p(X.ctypes.data),
	ctypes.c_uint(log2n)
)

#
#
#

x_cplx = x[0::2] + 1j*x[1::2]
X_gt = numpy.fft.fft(x_cplx)
X_gt = numpy.stack([X_gt.real, X_gt.imag]).transpose().reshape(-1)

#
#
#

print('* fft of length %d' % n)
print('* max absolute error: %f' % numpy.max(numpy.abs(X-X_gt)))
print('* max relative error: %f' % numpy.max(numpy.abs(X-X_gt)/(1e-6 + numpy.abs(X_gt))))