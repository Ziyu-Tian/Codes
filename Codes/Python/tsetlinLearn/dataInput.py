import numpy as np
import pandas as pd
from binarizer import StandardBinarizer


x = pd.read_csv('IRIS.csv',usecols=['sepal_length','sepal_width','petal_length','petal_width'])
y = pd.read_csv('IRIS.csv',usecols=['species'])
x = np.array(x)
y = np.array(y)
#print(x)
binarizer = StandardBinarizer(max_bits_per_feature=10)
binarizer.fit(x)
x = binarizer.transform(x)
x = np.hstack((x,y))
x = x.astype(int)
np.savetxt("output.csv",x,delimiter=',',fmt='%d')
