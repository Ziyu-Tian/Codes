import pandas as pd
import numpy as np
from sklearn.decomposition import PCA
from sklearn.datasets import fetch_openml
x = pd.read_csv('train.csv',usecols=range(0, 3461),header = None)

X = np.array(x) 

pca = PCA(0.95)

pca.fit(X)

PCA(n_components=0.95)

num = pca.n_components_
"""
print(num)
"""

n_components = num
pca = PCA(n_components=n_components)

X_pca = pca.fit_transform(X)

print(X_pca)
print(X_pca.shape)

#print(pca.explained_variance_ratio_)



