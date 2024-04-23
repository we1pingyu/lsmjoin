import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.patches import Patch
import matplotlib.lines as mlines
from csv_process import write_csv_from_txt, process_csv

test_names = ['bpk']
for test_name in test_names:
    write_csv_from_txt(test_name)
    process_csv(test_name)

bpk = pd.read_csv('lsm_join/lsm_res/bpk.csv')

# 准备绘图
fig, (ax) = plt.subplots(figsize=(4, 3))

hatches = ['xxxxxx', '//////', '++++++']
# 手动设置的参数
bar_width = 0.01  # 条形图的宽度
group_gap = 0.02   # 组之间的间隔

unique_bpk = sorted(bpk['bpk'].unique())
unique_labels = sorted(bpk['label'].unique())

# 计算每个组的起始位置
group_width = len(unique_labels) * bar_width
start_pos = np.arange(len(unique_bpk)) * (group_width + group_gap)

# 设置x轴的刻度和标签
x_ticks = [i * (len(unique_labels) * bar_width + group_gap) + (len(unique_labels) - 2) * bar_width for i in range(len(unique_bpk))]

for i, label in enumerate(unique_labels):
        sum_join_time = bpk[(bpk['label'] == label) ]['sum_join_time'].values
        if 'CComp' in label:
            marker = 'o'
        elif 'CEager' in label:
            marker = 's'
        else:
            marker = 'x'
        ax.plot(x_ticks, sum_join_time, marker=marker, fillstyle='none', linewidth=0.5, markersize=4, color='black')  # 绘制曲线



# 新的代码来绘制曲线
ax2 = ax.twinx()  # 创建第二个y轴

for i, b in enumerate(unique_bpk):
        for j, label in enumerate(unique_labels):
            # 获取对应M值和标签的sum_join_time
            # sum_join_time = bpk[(bpk['bpk'] == b) & (bpk['label'] == label) ]['sum_join_time'].values
            # cache_hit_rate = buffer_size[(buffer_size['M_MB'] == M) & (buffer_size['label'] == label) & (buffer_size['theory'] == theory)]['cache_hit_rate'].values
            fp = bpk[(bpk['bpk'] == b) & (bpk['label'] == label) ]['false_positive_rate'].values
            
            if 'CComp' in label:
                hatch = hatches[0]
            elif 'CEager' in label:
                hatch = hatches[1]
            else:
                hatch = hatches[2]
                
            color = 'black'
            # 计算条形图的位置
            x_pos = i * (len(unique_labels) * bar_width + group_gap) + j * bar_width
            # 绘制条形图
            ax2.bar(x_pos, fp * 100, width=bar_width, color=color, fill=False, hatch=hatch, edgecolor=color, linewidth=0.5)
            
ax2.set_xticks(x_ticks)
ax2.set_xticklabels(unique_bpk)

# set y max
ax2.set_ylim(0, 100)

font_size = 9
ax.set_xlabel('bpk', fontsize=font_size)
ax.set_ylabel('Join Latency (s)', fontsize=font_size)
ax2.set_ylabel('False Positive Rate (%)', fontsize=font_size)

# aj2 = aj.twinx()  # 创建第二个y轴
# aj.set_xlabel('cache_size', fontsize=font_size)
# aj.set_ylabel('Join Latency (s)', fontsize=font_size)
# aj2.set_ylabel('False Positive Rate (%)', fontsize=font_size)

legend_handles = [
    Patch(facecolor='black', hatch=hatches[0], fill=False, label='CComp-INLJ'),
    Patch(facecolor='black', hatch=hatches[1], fill=False, label='CEager-INLJ'),
    Patch(facecolor='black', hatch=hatches[2], fill=False, label='CLazy-INLJ'),
]

legend_handles2 = [
    mlines.Line2D([], [], color='black', marker='o', linestyle='None', markersize=4, fillstyle='none', label='CComp-INLJ'),
    mlines.Line2D([], [], color='black', marker='s', linestyle='None', markersize=4, fillstyle='none', label='CEager-INLJ'),
    mlines.Line2D([], [], color='black', marker='x', linestyle='None', markersize=4, fillstyle='none', label='CLazy-INLJ')
]

fig.legend(handles=legend_handles, fontsize=6, ncol=1, bbox_to_anchor=(0.83, 0.89), frameon=False)
fig.text(0.72, 0.89, 'False Positive Rate', ha='center', va='center', fontsize=6)

fig.legend(handles=legend_handles2, fontsize=6, ncol=1, bbox_to_anchor=(0.58, 0.89), frameon=False)
fig.text(0.48, 0.89, 'Join Latency', ha='center', va='center', fontsize=6)

plt.tight_layout()
plt.savefig('lsm_join/lsm_plot/8_buffperkey.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()