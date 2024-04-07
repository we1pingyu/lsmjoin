import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch


buffer_size = pd.read_csv('lsm_join_pi/lsm_res/buffer_size.csv')

# 准备绘图
fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(9, 2))

colors = ['#A0A0A0', '#BF8A89', '#5757F6']
hatches = ['xxxxxx', '//////']


# 转换M为MB
buffer_size['M_MB'] = buffer_size['M'] / (2**20)

# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02   # 组之间的间隔

# 获取唯一的M值和标签
unique_M_MB = sorted(buffer_size['M_MB'].unique())
unique_labels = sorted(buffer_size['label'].unique())

# 计算每个组的起始位置
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_M_MB)) * (group_width + group_gap)

for i, M in enumerate(unique_M_MB):
    for theory in [0, 1]:
        for j, label in enumerate(unique_labels):
            # 获取对应M值和标签的sum_join_time
            sum_join_time = buffer_size[(buffer_size['M_MB'] == M) & (buffer_size['label'] == label) & (buffer_size['theory'] == theory)]['sum_join_time'].values
            cache_hit_rate = buffer_size[(buffer_size['M_MB'] == M) & (buffer_size['label'] == label) & (buffer_size['theory'] == theory)]['cache_hit_rate'].values
            index_build_time = buffer_size[(buffer_size['M_MB'] == M) & (buffer_size['label'] == label) & (buffer_size['theory'] == theory)]['sum_index_build_time'].values
            
            if 'CComp' in label:
                color = colors[0]
            elif 'CEager' in label:
                color = colors[2]
            elif label == '2Comp-SJ':
                color = colors[1]
                
            if theory == 0:
                hatch = hatches[0]
            else:
                hatch = hatches[1]
            
            # 计算条形图的位置
            x_pos = i * (len(unique_labels) * bar_width * 2 + group_gap) + j * bar_width * 2 + theory * bar_width
            # 绘制条形图
            ax1.bar(x_pos, sum_join_time, width=bar_width, color=color, fill=False, hatch=hatch, edgecolor=color)
            ax2.bar(x_pos, index_build_time, width=bar_width, color=color, fill=False, edgecolor=color, hatch=hatch)
            # Plot cache_hit_rate on ax2
            ax3.bar(x_pos, cache_hit_rate * 100, width=bar_width, color=color, hatch=hatch, edgecolor=color, fill=False)
            


# 设置x轴的刻度和标签
for ax in [ax1, ax2, ax3]:
    ax.set_xticks([i * (len(unique_labels) * bar_width * 2 + group_gap) + (len(unique_labels) - 1) * bar_width for i in range(len(unique_M_MB))])
    ax.set_xticklabels(['4M', '16M', '32M', '64M'])

legend_handles = [
    Patch(facecolor=colors[0], label='CComp-INLJ'),
    Patch(facecolor=colors[2], label='CEager-INLJ'),
    Patch(facecolor=colors[1], label='2Comp-SJ'),
    
    Patch(facecolor='black', hatch=hatches[0], fill=False,label='Theoretical', linewidth=0.5),
    Patch(facecolor='black', hatch=hatches[1], fill=False,label='Empirical', linewidth=0.5),
]

# legend_handles_ax1 = [
#     Patch(facecolor='black', fill=False, linewidth=0.5, label='Index Build Time'),
#     Patch(facecolor='black', fill=False, linewidth=0.5, label='', hatch=hatches[1]),
# ]

# 添加图例
fig.legend(handles=legend_handles, fontsize=6, ncol=5, bbox_to_anchor=(0.73, 1.03))
# ax1.legend(handles=legend_handles_ax1, fontsize=6, ncol=1)

# 添加轴标签
font_size = 9
ax1.set_xlabel('Buffer Size', fontsize=font_size)
ax1.set_ylabel('Join Latency (s)', fontsize=font_size)

ax2.set_xlabel('Buffer Size', fontsize=font_size)
ax2.set_ylabel('Index Build Latency (s)', fontsize=8)

ax3.set_xlabel('Buffer Size', fontsize=font_size)
ax3.set_ylabel('Cache Hit Rate (%)', fontsize=font_size)

plt.tight_layout()
plt.savefig('lsm_join_pi/lsm_plot/new_plot_5.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()
