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

label_settings = {
    'CComp-INLJ': {'color': colors[0], 'marker': 'o'},
    'CEager-INLJ': {'color': colors[2], 'marker': 'o'},
    '2Comp-ISJ': {'color': colors[1], 'marker': 'o'},
    '2CLazy-ISJ': {'color': colors[1], 'marker': 's'},
    '1CLazy-ISJ': {'color': colors[0], 'marker': 's'}
}


for i, (ax, df_info) in enumerate(zip(axes, dfs)):
    df = df_info['df']
    attribute = df_info['title']
    fillstyle= 'none'
    title = attribute
    if attribute == 'T_t':
        attribute = 'T'
        title = 'T (Theoretical)'
    
    for label, group in df.groupby('label'):
        color = label_settings[label]['color']
        marker = label_settings[label]['marker']
        
        ax.plot(group[attribute], group['sum_join_time'], marker=marker, fillstyle=fillstyle, color=color, linewidth=0.5, markersize=4)
        ax.plot(group[attribute], group['sum_index_build_time'], linestyle='--', marker=marker, fillstyle=fillstyle, color=color, linewidth=0.5, markersize=4)
    
    if i == 0:
        ax.set_ylabel('System Latency (s)')
    ax.set_xlabel(title)
    if attribute == 'bpk' or attribute == 'T':
        ax.set_xticks([2, 5, 10, 20])
    elif attribute == 'K':
        ax.set_xticks([2, 4, 6, 8])
    

# 创建图例
legend_handles = [
    mlines.Line2D([], [], color='black', linestyle='-', linewidth=0.5, label='Join Time'),
    mlines.Line2D([], [], color='black', linestyle='--', linewidth=0.5, label='Index Build Time'),
]

for label, setting in label_settings.items():
    legend_handles.append(mlines.Line2D([], [], color=setting['color'], marker=setting['marker'], linestyle='None', markersize=4, fillstyle='none',label=label))

fig.legend(handles=legend_handles, bbox_to_anchor=(0.76, 1.14), ncol=7, fontsize=6)


plt.tight_layout()
plt.savefig('lsm_join/lsm_plot/5_compaction.pdf', bbox_inches="tight", pad_inches=0.02)
plt.close()