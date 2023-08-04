import pandas as pd

df = pd.read_csv('output.csv',header=None)

part1 = df.iloc[:500]
part2 = df.iloc[60000:60100]

part1.to_csv('train.csv',index = False, header=None)
part2.to_csv('test.csv', index = False, header=None)



