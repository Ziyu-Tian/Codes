import numpy as np
import pandas as pd
import csv
from sklearn.decomposition import PCA
from sklearn.datasets import fetch_openml
from binarizer import StandardBinarizer
"""
def merge_csv_files(input_files, output_file): # function to merge two csv file 
    with open(output_file, 'w', newline='') as outfile:
        writer = csv.writer(outfile)

        for input_file in input_files:
            with open(input_file, 'r', newline='') as infile:
                reader = csv.reader(infile)
                for row in reader:
                    writer.writerow(row)

# Combine the training and testing data

input_file = ["mnist_train.csv","mnist_test.csv"]

output_file = "train_test.csv"

merge_csv_files(input_file,output_file)


# PCA Process 

x = pd.read_csv('train_test.csv',header=None)


Col = x.shape[1]


x = pd.read_csv('train_test.csv',usecols=range(1, Col),header=None)
y = pd.read_csv('train_test.csv',usecols=[0],header=None)



pca = PCA(0.95)

pca.fit(x)

PCA(n_components=0.95)

num = pca.n_components_


pca = PCA(n_components=num)

X_pca = pca.fit_transform(x)

X_pca = np.array(X_pca)

np.savetxt("pca.csv",X_pca,delimiter=',',fmt='%d')

"""


# Binarizer 

x = pd.read_csv('train_test.csv',header=None)

#
Col = x.shape[1]
x = pd.read_csv('train_test.csv',usecols=range(1, Col),header=None)
y = pd.read_csv('train_test.csv',usecols=[0],header=None)
#

x = np.array(x)

binarizer = StandardBinarizer(max_bits_per_feature=5)
binarizer.fit(x)
x = binarizer.transform(x)
x = np.hstack((x,y)) # combine the label-y in the last col of x
x = x.astype(int)
print(x.shape[1])
print(x.shape[0])
np.savetxt("output.csv",x,delimiter=',',fmt='%d')


# Split into train and test 


x = pd.read_csv('output.csv',header=None)

part1 = x.iloc[:60000]
part2 = x.iloc[60000:70000]

part1.to_csv('train.csv',index = False, header=None)
part2.to_csv('test.csv', index = False, header=None)








