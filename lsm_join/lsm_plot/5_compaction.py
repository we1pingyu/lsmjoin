import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.lines as mlines

# 假设 bpk 和 T 是通过读取文件获得的数据框
bpk = pd.read_csv('lsm_join/lsm_res/bpk.csv')
T = pd.read_csv('lsm_join/lsm_res/T.csv')
T_t = pd.read_csv('lsm_join/lsm_res/T_t.csv')
K = pd.read_csv('lsm_join/lsm_res/K.csv')

# 创建数据框的数组
dfs = [{'df': T, 'title': 'T'}, {'df': T_t, 'title': 'T_t'} ,{'df': K, 'title': 'K'}]

# 设置图的大小和子图布局
fig, axes = plt.subplots(1, 3, figsize=(9, 2), sharey=True) # 一行两列

colors = ['#3E8D27', '#A22025', '#1432F5']

for i, (ax, df_info) in enumerate(zip(axes, dfs)):
    df = df_info['df']
    title = df_info['title']
    fillstyle= 'none'
    if title == 'T_t':
        fillstyle = 'top'
        title = 'T'
    
    for label, group in df.groupby('label'):
        if 'CComp' in label:
            color = colors[0]
        elif 'CEager' in label:
            color = colors[2]
        elif label == '2Comp-SJ':
            color = colors[1]
        
        if 'INLJ' in label:
            marker = 'o'
        elif 'SJ' in label:
            marker = 's'
        
        ax.plot(group[title], group['sum_join_time'], marker=marker, fillstyle=fillstyle, color=color, linewidth=0.5, markersize=4)
        ax.plot(group[title], group['sum_index_build_time'], linestyle='--', marker=marker, fillstyle=fillstyle, color=color, linewidth=0.5, markersize=4)
    
    if i == 0:
        ax.set_ylabel('System Latency (s)')
    ax.set_xlabel(title)
    if title == 'bpk' or title == 'T':
        ax.set_xticks([2, 5, 10, 20])
    elif title == 'K':
        ax.set_xticks([2, 4, 6, 8])
    

# 创建图例
legend_handles = [
    mlines.Line2D([], [], color='black', linestyle='-', linewidth=0.5, label='Join Time'),
    mlines.Line2D([], [], color='black', linestyle='--', linewidth=0.5, label='Index Build Time'),
    
    mlines.Line2D([], [], color='black', marker='o', linestyle='None', markersize=4, fillstyle='none', label='Practical'),
    mlines.Line2D([], [], color='black', marker='o', linestyle='None', markersize=4, fillstyle='top', label='Theroetical'),
    
    mlines.Line2D([], [], color=colors[0], marker='o', linestyle='None', markersize=4, fillstyle='none', label='CComp-INLJ'),
    mlines.Line2D([], [], color=colors[2], marker='o', linestyle='None', markersize=4, fillstyle='none', label='CEager-INLJ'),
    mlines.Line2D([], [], color=colors[1], marker='s', linestyle='None', markersize=4, fillstyle='none', label='2Comp-SJ')
]
fig.legend(handles=legend_handles, bbox_to_anchor=(0.76, 0.94), ncol=7, fontsize=6)


plt.tight_layout()
plt.savefig('lsm_join/5_compaction.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()