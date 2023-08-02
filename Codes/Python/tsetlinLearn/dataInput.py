import numpy as np
import pandas as pd
from binarizer import StandardBinarizer


x = pd.read_csv('mnist_test.csv',usecols=range(1, 784))
y = pd.read_csv('mnist_test.csv',usecols=['label'])
x = np.array(x)
y = np.array(y)
#print(x)
binarizer = StandardBinarizer(max_bits_per_feature=10)
binarizer.fit(x)
x = binarizer.transform(x)
x = np.hstack((x,y))
x = x.astype(int)
np.savetxt("test.csv",x,delimiter=',',fmt='%d')
