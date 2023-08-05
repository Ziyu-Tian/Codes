import numpy as np
import pandas as pd
import csv
from binarizer import StandardBinarizer

# Merge train.csv and test.csv
"""
def merge_csv_files(input_files, output_file):
    with open(output_file, 'w', newline='') as outfile:
        writer = csv.writer(outfile)

        for input_file in input_files:
            with open(input_file, 'r', newline='') as infile:
                reader = csv.reader(infile)
                for row in reader:
                    writer.writerow(row)

if __name__ == "__main__":
    input_files = ["mnist_train.csv", "mnist_test.csv"]  
    output_file = "train_test.csv"          

    merge_csv_files(input_files, output_file)


"""
x = pd.read_csv('pca_testing.csv',usecols=range(0,263),header= None)
y = pd.read_csv('train.csv',usecols=[3462],header= None)
x = np.array(x)
y = np.array(y)

binarizer = StandardBinarizer(max_bits_per_feature=5)
binarizer.fit(x)
x = binarizer.transform(x)
x = np.hstack((x,y)) # combine the label-y in the last col of x
x = x.astype(int)
np.savetxt("output.csv",x,delimiter=',',fmt='%d')
