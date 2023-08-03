import numpy as np
import pandas as pd
import csv
from binarizer import StandardBinarizer
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
    input_files = ["mnist_train.csv", "mnist_test.csv"]  # 输入要合并的CSV文件列表
    output_file = "train_test.csv"          # 合并后的输出文件

    merge_csv_files(input_files, output_file)


x = pd.read_csv('train_test.csv',usecols=range(1,784))
y = pd.read_csv('train_test.csv',usecols=['label'])


x_train = x.iloc[1:60000, 1:784]
x_test = x.iloc[60001:70001, 1:784]

y_train = x.iloc[1:60000, 0]
y_test = x.iloc[60001:70001, 0]

x_train = np.array(x_train)
x_test = np.array(x_test)

y_train = np.array(y_train)
y_test = np.array(y_test)


#print(x)
binarizer = StandardBinarizer(max_bits_per_feature=5)
binarizer.fit(x)

x_train = binarizer.transform(x_train)
x_train= np.hstack((x_train,y_train))
x_train = x_train.astype(int)
np.savetxt("train.csv",x_train,delimiter=',',fmt='%d')

binarizer.fit(x_test)

x = binarizer.transform(x)
#x = np.hstack((x,y))
x = x.astype(int)
np.savetxt("test1.csv",x,delimiter=',',fmt='%d')
"""
x = pd.read_csv('train_test.csv',usecols=range(1, 784))
y = pd.read_csv('train_test.csv',usecols=['label'])
x = np.array(x)
y = np.array(y)
#print(x)
binarizer = StandardBinarizer(max_bits_per_feature=5)
binarizer.fit(x)
x = binarizer.transform(x)
x = np.hstack((x,y))
x = x.astype(int)
np.savetxt("output.csv",x,delimiter=',',fmt='%d')
