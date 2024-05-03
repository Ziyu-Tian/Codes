import matplotlib.pyplot as plt
import numpy as np
plt.rc('font',family='Times New Roman')
x_data = ['Xor','Iris','Noisy_Xor']
ax_data = [420,494,427]
ax1_data = [545,561,557]

fig=plt.figure(dpi=600)
ax=fig.add_subplot(111)

ax.set_ylim([400,510])#range of left-y
ax.set_yticks=np.arange(400,510) #range of left-y
ax.set_yticklabels=np.arange(400,510) #range of left-y

bar_width = 0.3
ax.set_ylabel('LUT Numbers',fontsize=10,fontweight='bold');
lns1=ax.bar(x=np.arange(len(x_data)), width=bar_width, height=ax_data, label='LUT Numbers', fc = 'steelblue',alpha=0.8)

for a,b in enumerate(ax_data):
    plt.text(a,b+0.0005,'%s' % b,ha='center')
    
    
ax1 = ax.twinx() # this is the important function   

ax1.set_ylim([530,600]) #range of right-y
ax1.set_yticks=np.arange(530,600) #range of right-y
ax1.set_yticklabels=np.arange(530,600) #range of right-y
ax1.set_ylabel('FF Numbers',fontsize=10,fontweight='bold');
lns2=ax1.bar(x=np.arange(len(x_data))+bar_width, width=bar_width, height=ax1_data,label='FF Numbers',fc = 'indianred',alpha=0.8)

for a,b in enumerate(ax1_data):
    plt.text(a+0.3,b+0.001,'%s' % b,ha='center')
    
plt.xticks(np.arange(len(x_data))+bar_width/2, x_data)

ax.set_xlabel('Dataset Type',fontsize=10,fontweight='bold')

fig.legend(loc=1,bbox_to_anchor=(0.28,1),bbox_transform=ax.transAxes)

plt.savefig('./LUT.png')  #图表输出
