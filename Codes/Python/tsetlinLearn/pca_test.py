import pandas as pd
import numpy as np
from sklearn.decomposition import PCA
from sklearn.datasets import fetch_openml
import csv
from binarizer import StandardBinarizer
x = pd.read_csv('train.csv',usecols=range(0, 3461),header = None)
x = np.array(x) 


"""
pca = PCA(0.95)

pca.fit(x)

PCA(n_components=0.95)

num = pca.n_components_

print(num)

"""

pca = PCA(n_components=264)

X_pca = pca.fit_transform(x)

X_pca = np.array(X_pca)

np.savetxt("pca_testing.csv",X_pca,delimiter=',',fmt='%d')





