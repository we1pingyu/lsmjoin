import matplotlib.pyplot as plt
import pandas as pd
from csv_process import write_overall_csv, process_csv

test_name = 'overall'
write_overall_csv(test_name)
process_csv(test_name)
# read df
df = pd.read_csv('lsm_join/lsm_plot/overall.csv')

# pair info

pairs1 = [['P-INLJ'], ['P-ISJ'], ['P-Eager-ISJ', 'P-Lazy-ISJ', 'P-Comp-ISJ'], ['P-CEager-ISJ', 'P-CLazy-ISJ', 'P-CComp-ISJ'], ['P-Grace-HJ']]
x_labels1 = ['P-INLJ', 'P-ISJ', 'P*-ISJ', 'P-C*-ISJ', 'HJ']

pairs2 = [['Eager-INLJ', 'Lazy-INLJ', 'Comp-INLJ'], ['CEager-INLJ', 'CLazy-INLJ', 'CComp-INLJ'], ['Grace-HJ']]
x_labels2 = ['*-INLJ', 'C*-INLJ', 'HJ']

pairs3 = [['NISJ'], ['1Eager-ISJ', '1Lazy-ISJ', '1Comp-ISJ'], ['1CEager-ISJ', '1CLazy-ISJ', '1CComp-ISJ'], ['2Eager-ISJ', '2Lazy-ISJ', '2Comp-ISJ'], ['2CEager-ISJ', '2CLazy-ISJ', '2CComp-ISJ']]
x_labels3 = ['NISJ', '1*-ISJ', '1C*-ISJ', '2*-ISJ', '2C*-ISJ']

all_pairs = [pairs1, pairs2, pairs3]
all_x_lables = [x_labels1, x_labels2, x_labels3]

# 确定条形图的宽度和间隔
bar_width = 0.05  # 条形图的宽度
group_gap = 0.05   # 不同组之间的间隔

# 颜色设置
colors = ['#003f5c', '#bc5090', '#ffa600', '#5F8670']  

# 为图例创建自定义图标
from matplotlib.patches import Patch
legend_patches = [
    Patch(facecolor=colors[0], label='Eager', linewidth=0.5, edgecolor='black'),
    Patch(facecolor=colors[1], label='Lazy', linewidth=0.5, edgecolor='black'),
    Patch(facecolor=colors[2], label='Comp', linewidth=0.5, edgecolor='black'),
    Patch(facecolor=colors[3], label='Non-Index', linewidth=0.5, edgecolor='black'),
]

# 分两行显示图表
datasets = df['dataset'].unique()
rows = 3
cols = len(datasets)  # 确保有足够的列来容纳所有的数据集

# 初始化绘图，设置为两行
fig, axes = plt.subplots(rows, cols, figsize=(4 * cols, 4 * rows))

# 存储每一行的最大y值
max_y_values = [0] * rows

for row, pairs, x_labels in zip(range(rows), all_pairs, all_x_lables):
    # 计算每个group的起始位置
    group_start_positions = [0]  # 第一个group从位置0开始

    for pair in pairs[:-1]:  # 跳过最后一个pair
        next_position = group_start_positions[-1] + len(pair) * bar_width + group_gap
        group_start_positions.append(next_position)

    for i, dataset in enumerate(datasets):
        col = i % cols
        current_ax = axes[row, col] if rows > 1 else axes[col]  # 当只有一行时直接索引 axes
        
        group_positions = []
        for j, pair in enumerate(pairs):
            start_position = group_start_positions[j]
            
            for k, label in enumerate(pair):
                position = start_position + k * bar_width
                
                join_time = df[(df['dataset'] == dataset) & (df['label'] == label)]['join_time'].values[0] if label in df[df['dataset'] == dataset]['label'].values else 0
                
                index_build_time = df[(df['dataset'] == dataset) & (df['label'] == label)]['index_build_time'].values[0] if label in df[df['dataset'] == dataset]['label'].values else 0
                
                color = colors[3]
                
                if 'Eager' in label:
                    color = colors[0]
                elif 'Lazy' in label:
                    color = colors[1]
                elif 'Comp' in label:
                    color = colors[2]
                    
                
                current_ax.bar(position, join_time, width=bar_width, color=color, edgecolor='black')
                current_ax.bar(position, index_build_time, width=bar_width, edgecolor=color, fill=False,
                            hatch="///", bottom=join_time)
            
            # 将当前组的中心位置添加到列表中
            group_positions.append(start_position + (len(pair) * bar_width) / 2 - bar_width / 2)
        
        # 设置 x 轴标签和标题
        current_ax.set_xticks(group_positions)
        current_ax.set_xticklabels(x_labels)
        current_ax.set_xlabel('')
        current_ax.set_ylabel('System Latency (s)' if col == 0 else '', fontsize=15)
        if row == 0:
            current_ax.set_title(dataset)
        max_y_values[row] = max(max_y_values[row], current_ax.get_ylim()[1])
    
# 设置统一的y轴范围，并隐藏非首图的y轴标记
for i, ax in enumerate(axes.flat):
    row = i // cols
    ax.set_ylim(0, max_y_values[row])
    if i % cols != 0:  # 如果不是每行的第一个图表
        ax.set_yticklabels([])

fig.legend(handles=legend_patches, bbox_to_anchor=(0.61, 0.96),
        ncol=4, fontsize=12)

# 调整布局
plt.subplots_adjust(top=0.9, wspace=0.03, hspace=0.1)  # 调整这个值以确保上方有足够的空间给图例
# plt.tight_layout()
plt.savefig('lsm_join/lsm_plot/3_overall.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()
