import numpy as np
import pandas as pd
import csv
from binarizer import StandardBinarizer
import random

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
x = pd.read_csv('train_test.csv',usecols=range(0,3),header= None)
y = pd.read_csv('train_test.csv',usecols=[4],header=None)
x = np.array(x)
y = np.array(y)

binarizer = StandardBinarizer(max_bits_per_feature=3)
binarizer.fit(x)
x = binarizer.transform(x)

x = np.hstack((x,y)) # combine the label-y in the last col of x
x = x.astype(int)
np.savetxt("binary_iris.csv",x,delimiter=',',fmt='%d')


csv_file_path = "binary_iris.csv"  
lines = []
with open(csv_file_path, 'r') as csvfile:
    csvreader = csv.reader(csvfile)
    for row in csvreader:
        lines.append(row)


random.shuffle(lines)


shuffled_csv_file_path = "shuffled_binary_iris.csv"  
with open(shuffled_csv_file_path, 'w', newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    csvwriter.writerows(lines)



df = pd.read_csv('shuffled_binary_iris.csv',header=None)

part1 = df.iloc[:120]
part2 = df.iloc[120:150]

part1.to_csv('iris_train.csv',index = False, header=None)
part2.to_csv('iris_test.csv', index = False, header=None)
