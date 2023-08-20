import pandas as pd

df = pd.read_csv('binary_bc.csv',header=None)

part1 = df.iloc[:455]
part2 = df.iloc[455:569]

part1.to_csv('bc_train.csv',index = False, header=None)
part2.to_csv('bc_test.csv', index = False, header=None)



