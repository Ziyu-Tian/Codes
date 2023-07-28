"""
import matplotlib.pyplot as plt

# 创建示例数据
x1 = [1, 2, 3, 4, 5]
x2 = [1.5, 2.5, 3.5, 4.5, 5.5]
y1 = [10, 15, 12, 8, 20]
y2 = [5, 10, 8, 6, 15]


# 绘制第一个 x 轴对应的折线图
plt.plot(x1, y1, label='Parameter 1', color='blue')
plt.xlabel('X Axis 1')
plt.ylabel('Parameter 1 Values', color='blue')
plt.tick_params(axis='y', labelcolor='blue')

# 创建第二个 x 轴，并绘制对应的折线图
plt.twiny()
plt.plot(x2, y2, label='Parameter 2', color='red')
plt.xlabel('X Axis 2')
plt.ylabel('Parameter 2 Values', color='red')
plt.tick_params(axis='y', labelcolor='red')

# 显示图例
plt.grid()

# 显示图形
plt.show()
"""
"""
import matplotlib.pyplot as plt

# 创建示例数据
x = [1, 2, 3, 4, 5]
y = [10, 15, 12, 8, 20]

# 绘制折线图
plt.plot(x, y)

# 设置 x 轴和 y 轴标签
plt.xlabel('X Axis')
plt.ylabel('Y Axis')

# 显示网格
plt.grid()

# 显示图形
plt.show()
"""

import matplotlib.pyplot as plt

# 创建示例数据
x1 = [1, 2, 3, 4, 5]
x2 = [1.5, 2.5, 3.5, 4.5, 5.5]
y1 = [10, 15, 12, 8, 20]
y2 = [5, 10, 8, 6, 15]

# 绘制第一个 x 轴对应的折线图
plt.plot(x1, y1, label='Parameter 1', color='blue')
plt.xlabel('X Axis 1')
plt.ylabel('Parameter 1 Values', color='blue')
plt.tick_params(axis='y', labelcolor='blue')

plt.grid(axis = 'both')

# 创建第二个 x 轴，并绘制对应的折线图
plt.twiny()
plt.plot(x2, y2, label='Parameter 2', color='red')
plt.xlabel('X Axis 2')
plt.ylabel('Parameter 2 Values', color='red')
plt.tick_params(axis='y', labelcolor='red')


# 显示图例
plt.legend()

# 显示图形
plt.show()

