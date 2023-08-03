import pandas as pd
import numpy as np
from sklearn.decomposition import PCA
from sklearn.datasets import fetch_openml
x = pd.read_csv('train_test.csv',usecols=range(1, 784))

X = np.array(x) 

"""
n_components = 'mle'
pca = PCA(n_components=n_components)

X_pca = pca.fit_transform(X)

print(X_pca.shape)
print(pca.explained_variance_ratio_)

"""

