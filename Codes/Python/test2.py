import matplotlib.pyplot as plt

x = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y = [10, 15, 20, 25, 30]

plt.plot(x, y)

# 自定义x轴和y轴的刻度
plt.xticks([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], ['(5,5)', 'B', 'C', 'D', 'E'])
plt.yticks([10, 15, 20, 25, 30], ['10', '15', '20', '25', '30'])

plt.show()

