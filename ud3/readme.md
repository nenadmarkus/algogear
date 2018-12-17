## ud3: micro decision trees

The file `ud3.c` contains a minimalistic yet surprisingly versatile implementation of a regression tree, a special case of [decision tree](https://en.wikipedia.org/wiki/Decision_tree_learning) that maps a vector of observations into a real value (instead to some discrete label, such as class memebership).
In the case of `ud3.c`, the obesrvations are represented as an array of real values.

The implemented algorithm should be used to approximate/estimate the functional mappings of the kind $R^m\rightarrow R^n$ from training data.

### High-level api

	api.c

### Test on a toy dataset

An image can be as a function that maps 2D spatial coordinates to color (i.e., a 3D vector representing red, green and blue intensities).

In this experiment, we approximate a sample image with an ensemble of trees.

The following command will download a suitable image:

	wget https://tkv.io/posts/visualizing-audio-with-cppns/16.jpg -O img.jpg

Now run the script `test.py` to see how an ensemble of 16 trees od depth 10 approximates this image:

	python test.py

For a better approximation, add more trees or increase their depth.