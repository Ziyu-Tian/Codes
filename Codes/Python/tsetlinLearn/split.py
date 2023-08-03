import pandas as pd

df = pd.read_csv('output.csv',header=None)

part1 = df.iloc[:60000]
part2 = df.iloc[60000:70000]

part1.to_csv('train.csv',index = False, header=None)
part2.to_csv('test.csv', index = False, header=None)



