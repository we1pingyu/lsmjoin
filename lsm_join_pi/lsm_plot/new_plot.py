import matplotlib.pyplot as plt
import pandas as pd

# 读取数据
df = pd.read_csv('lsm_join_pi/overall.csv')

# 数据集名称列表
datasets = ['User', 'Unif', 'Skew', 'OSM', 'Movie', 'Face']

# 配对信息
pairs = [['P-INLJ', 'P-ISJ'], ['Eager-INLJ', '1Eager-ISJ'],
         ['Lazy-INLJ', '1Lazy-ISJ'], ['Comp-INLJ', '1Comp-ISJ'], 
         ['CEager-INLJ', '1CEager-ISJ'], ['CLazy-INLJ', '1CLazy-ISJ'], 
         ['CComp-INLJ', '1CComp-ISJ'], ['P-Grace-HJ', 'Grace-HJ']]

x_labels = ['Primary', 'Eager', 'Lazy', 'Comp', 'CEager', 'CLazy', 'CComp', 'Grace']

# 确定条形图的宽度和间隔
bar_width = 0.35
gap = 0.15  # 不同对之间的间隔

# 颜色和虚线填充设置
colors = ['grey', 'white']  # 左边使用灰色，右边使用白色但配合hatch
hatches = ['', '///']  # 右边条形使用虚线填充

# 为图例创建自定义图标
from matplotlib.patches import Patch
legend_patches = [
    Patch(facecolor='grey', label='INLJ', linewidth=0.5, edgecolor='black'),
    Patch(facecolor='white', hatch='///', label='SJ', linewidth=0.5, edgecolor='black'),
    Patch(facecolor='white', label='HJ', linewidth=0.5, edgecolor='black')  # 新增HJ图例
]

# 分两行显示图表
rows = 2
cols = (len(datasets) + 1) // 2  # 确保有足够的列来容纳所有的数据集

# 初始化绘图，设置为两行
fig, axes = plt.subplots(rows, cols, figsize=(4 * cols, 4 * rows))

for i, dataset in enumerate(datasets):
    row = i // cols
    col = i % cols
    current_ax = axes[row, col] if rows > 1 else axes[col]  # 当只有一行时直接索引 axes
    
    # 模拟从 df 中筛选特定 dataset 的数据
    current_df = df[df['dataset'] == dataset]
    
    # 绘制条形图
    for j, pair in enumerate(pairs):
        for k, label in enumerate(pair):
            join_time = current_df[current_df['label'] == label]['join_time'].values[0] if label in current_df['label'].values else 0
             
            position = j * (bar_width * 2 + gap) + k * bar_width
            
            if label == 'P-Grace-HJ' or label == 'Grace-HJ':
                color = 'white'
                hatch = ''
            else:
                hatch = hatches[k]
                color = colors[k]
            
            current_ax.bar(position, join_time, width=bar_width, color=color, hatch=hatch, edgecolor='black')
    
    # 设置 x 轴标签和标题
    current_ax.set_xticks([j * (bar_width * 2 + gap) + bar_width / 2 for j in range(len(pairs))])
    current_ax.set_xticklabels(x_labels, rotation=45)
    current_ax.set_xlabel('')
    current_ax.set_ylabel('Join Latency (s)' if col == 0 else '', fontsize=15)
    current_ax.set_title(dataset)

# 显示图例，仅在最后一个子图中显示
if rows > 1:
    axes[-1, -1].legend(handles=legend_patches, loc='upper right')
else:
    axes[-1].legend(handles=legend_patches, loc='upper right')
# 调整布局
plt.tight_layout()
plt.savefig('lsm_join_pi/new_plot.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()





