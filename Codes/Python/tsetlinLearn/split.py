import pandas as pd

df = pd.read_csv('shuffled_binary_iris.csv',header=None)

part1 = df.iloc[:120]
part2 = df.iloc[120:150]

part1.to_csv('iris_train.csv',index = False, header=None)
part2.to_csv('iris_test.csv', index = False, header=None)



