import numpy
import os
import ctypes
import cv2
import time

#
#
#

os.system('cc api.c -O3 -fPIC -shared -o ud3.lib.so')
ud3lib = ctypes.cdll.LoadLibrary('./ud3.lib.so')
os.system('rm ud3.lib.so')

#
# load the image
#

img = cv2.imread('img.png').astype(numpy.float32)/255.0

nrows = img.shape[0]
ncols = img.shape[1]
nchns = 3

targets = numpy.ascontiguousarray(img.copy().reshape(-1, nchns))

#
# generate the tree inputs
#

# construct a 2D array in which each row has integers between 0 and nrows-1
rowmat = numpy.tile(numpy.linspace(0, nrows-1, nrows, dtype=numpy.float32), ncols).reshape(ncols, nrows).T
# construct a 2D array in which each column has integers between 0 and ncols-1
colmat = numpy.tile(numpy.linspace(0, ncols-1, ncols, dtype=numpy.float32), nrows).reshape(nrows, ncols)

inputs = numpy.stack([rowmat, colmat]).transpose(1, 2, 0).reshape(-1, 2)
inputs = numpy.ascontiguousarray(inputs)

#
# learn the trees with gradient boosting
#

ntrees = 16
tdepth = 10

trees = numpy.zeros(ntrees, dtype=numpy.int64)

start = time.time()

ud3lib.new_ensemble(
	ctypes.c_float(0.25),
	ctypes.c_void_p(trees.ctypes.data), ctypes.c_int(ntrees), ctypes.c_int(tdepth),
	ctypes.c_void_p(targets.ctypes.data), ctypes.c_int(targets.shape[1]),
	ctypes.c_void_p(inputs.ctypes.data), ctypes.c_int(inputs.shape[1]),
	ctypes.c_void_p(0),
	ctypes.c_int(inputs.shape[0]),
	ctypes.c_int(32)
)

print('* elapsed time (learning): %d' % int(time.time() - start))

#
# compute the approximation
#

predictions = numpy.zeros(targets.shape, dtype=numpy.float32)

start = time.time()

ud3lib.run_ensemble(
	ctypes.c_void_p(trees.ctypes.data), ctypes.c_int(ntrees),
	ctypes.c_void_p(inputs.ctypes.data), ctypes.c_int(inputs.shape[1]),
	ctypes.c_void_p(predictions.ctypes.data), ctypes.c_int(predictions.shape[1]),
	ctypes.c_int(inputs.shape[0])
)

print('* elapsed time (prediction): %d' % int(time.time() - start))

ud3lib.del_ensemble(
	ctypes.c_void_p(trees.ctypes.data), ctypes.c_int(ntrees)
)

#
# display the results
#

orig = img
pred = predictions.reshape(nrows, ncols, nchns)

show = numpy.zeros((orig.shape[0], 2*orig.shape[1], 3), dtype=numpy.float32)

show[:, 0:orig.shape[1], :] = orig
show[:, orig.shape[1]:, :]  = pred

cv2.imshow('original | approximated', show)
cv2.waitKey(0)