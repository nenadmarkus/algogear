import numpy
import os
import ctypes

#
#
#

os.system('cc wmedian.c -fPIC -shared -o wmedian.lib.so')
wmedianlib = ctypes.cdll.LoadLibrary('./wmedian.lib.so')
os.system('rm wmedian.lib.so')

#
#
#

n = 4096

x = numpy.random.rand(n).astype(numpy.float32)
w = numpy.random.rand(n).astype(numpy.float32)

#
#
#

wmedianlib.wmedian.restype = ctypes.c_float

r1 = wmedianlib.wmedian(
	ctypes.c_void_p(x.ctypes.data),
	ctypes.c_void_p(w.ctypes.data),
	ctypes.c_uint(len(x))
)

#
#
#


# taken from <https://gist.github.com/tinybike/d9ff1dad515b66cc0d87>
def weighted_median(data, weights):
	"""
	Args:
	  data (list or numpy.array): data
	  weights (list or numpy.array): weights
	"""
	data, weights = numpy.array(data).squeeze(), numpy.array(weights).squeeze()
	s_data, s_weights = map(numpy.array, zip(*sorted(zip(data, weights))))
	midpoint = 0.5 * sum(s_weights)
	if any(weights > midpoint):
		w_median = (data[weights == numpy.max(weights)])[0]
	else:
		cs_weights = numpy.cumsum(s_weights)
		idx = numpy.where(cs_weights <= midpoint)[0][-1]
		if cs_weights[idx] == midpoint:
			w_median = numpy.mean(s_data[idx:idx+2])
		else:
			w_median = s_data[idx+1]
	return w_median

r2 = weighted_median(x, w)

#
#
#

diff = abs(r2 - r1)/(min(r2, r1) + 1e-6)

print(diff)