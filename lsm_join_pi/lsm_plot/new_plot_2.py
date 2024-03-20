import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# 读取数据
df = pd.read_csv('lsm_join_pi/overall.csv')

# 筛选特定数据集为'User'，同时删除以'P-'开头的label
df = df[(df['dataset'] == 'User') & (~df['label'].str.startswith('P-'))]

# 定义pairs
pairs = [
    ['2CEager-ISJ', '2CComp-ISJ', '2CLazy-ISJ'],
    ['2Eager-ISJ', '2Comp-ISJ', '2Lazy-ISJ'],
    ['1CEager-ISJ', '1CComp-ISJ', '1CLazy-ISJ'],
    ['1Eager-ISJ', '1Comp-ISJ', '1Lazy-ISJ']
]

# 初始化绘图，图形高度基于所有标签的数量
fig, ax = plt.subplots(figsize=(5, len(pairs)))  # 为每个条形提供足够的空间

bar_width = 0.05  # 每个条形的宽度
gap_between_bars = 0.0  # 条形之间的间隔
current_position = 0  # 当前y轴位置

labels_position = []  # 存储每个标签的位置，用于设置y轴刻度

# 遍历pairs中的每个组
for pair in pairs:
    for label in pair:
        current_df = df[df['label'] == label]
        if not current_df.empty:
            index_build_time = current_df['index_build_time'].iloc[0]
            join_time = current_df['join_time'].iloc[0]
            
            # 绘制index_build_time条形
            ax.barh(current_position, -index_build_time, height=bar_width, color='grey', edgecolor='black', label='Index Build Time' if current_position == 0 else "")
            # 绘制join_time条形
            ax.barh(current_position, join_time, height=bar_width, color='white', edgecolor='black', label='Join Time' if current_position == 0 else "")
            
            labels_position.append(current_position)  # 添加当前位置到列表
            current_position += bar_width + gap_between_bars  # 更新位置
    
    current_position += 0.1  # 在每组pair之后增加额外的间隔

# 设置y轴的标签为每个标签名
ax.set_yticks(labels_position)
ax.set_yticklabels(sum(pairs, []))  # 使用sum将pairs列表扁平化

# 添加网格线以提高可读性
# ax.grid(True)

# 调整x轴的标签，以显示正负值
ax.set_xticks(ax.get_xticks())
ax.set_xticklabels([str(abs(x)) for x in ax.get_xticks()])

# 设置图例，确保只显示一次
ax.legend(loc='upper left')

# 调整布局
plt.tight_layout()
plt.savefig('lsm_join_pi/new_plot_2.pdf', bbox_inches="tight", pad_inches=0.02)
# plt.show()
