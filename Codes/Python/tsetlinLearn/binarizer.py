import typing

import numpy as np


class StandardBinarizer:

    number_of_features: int
    max_bits_per_feature: int
    unique_values: typing.List[np.ndarray]

    def __init__(self, max_bits_per_feature=25):
	# 25 is the default value of max_bits_per_feature if no value been passed
        self.max_bits_per_feature = max_bits_per_feature
        return

    def fit(self, X):
        self.number_of_features = 0
        self.unique_values = []
	# X.shape[1] get the columns-number of X
        for i in range(X.shape[1]):
		# np.unique() return the no-repeated array
		# X[:,i] for the whole lines in colomn i
            uv = np.unique(X[:, i])[1:]
		# X[:,i][1:] delete the first colum (column 0)
            if uv.size > self.max_bits_per_feature:
		# uv.size can get the size of matrix
                unique_values = np.empty(0)
		# np.empty(0) generate a zero-order matrix
                step_size = 1.0 * uv.size / self.max_bits_per_feature
		
                pos = 0.0
                while int(pos) < uv.size and unique_values.size < self.max_bits_per_feature:
                    unique_values = np.append(unique_values, np.array(uv[int(pos)]))
                    pos += step_size
            else:
                unique_values = uv

            self.unique_values.append(unique_values)
            self.number_of_features += self.unique_values[-1].size
        return

    def transform(self, X):
        X_transformed = np.zeros((X.shape[0], self.number_of_features))

        pos = 0
        for i in range(X.shape[1]):
            for j in range(self.unique_values[i].size):
                X_transformed[:, pos] = (X[:, i] >= self.unique_values[i][j])
                pos += 1

        return X_transformed

    def fit_transform(self, X):
        self.fit(X)
        return self.transform(X)































