import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

colors = [ "#a59344",
    "#7b463b",
    "#00b89f",
    "#191970",
    "#3d1c02",
    "#456789",
    "#05a3ad",
    "#4c4f56",
    "#4d3c2d",
    "#166461",
    "#9bafad",
    "#990066",
    "#5ba8ff",
    "#002c2b"]

# Read csv
df_ori = pd.read_csv('output.csv')

indexes = np.arange(len(df_ori.groupby(['r_index', 's_index', 'join_algorithm'])))
# assign a color to each combination, key: f'{r_index}-{s_index}-{join_algorithm}', value: color
color_map = {}
for i, (key, group) in enumerate(df_ori.groupby(['r_index', 's_index', 'join_algorithm'])):
    label = f'{key[0]}-{key[1]}-{key[2]}'
    color_map[label] = colors[i]

# Define the r_tuples values we're interested in
r_tuples_values = [1e6, 8e6, 16e6]
# r_tuples_values = [1e6]

# Set the width of the bars and the number of combinations
bar_width = 0.2

# Create a figure and axes for the subplots, vertical and not sharing y-axis
fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(8, 5), sharey=True)  # Adjusted figsize for vertical display

tables = [4, 5]
for k, table in enumerate(tables):
    df = df_ori[df_ori['Table'] == table]
    n_combinations = len(df.groupby(['r_index', 's_index', 'join_algorithm']))
    # Loop through each r_tuples value and create a subplot
    for i, r_value in enumerate(r_tuples_values):
        # Filter the dataframe for the current r_tuples value
        df_filtered = df[df['r_tuples'] == r_value]
        
        # Create an array with the position of each group of bars along the x-axis
        x_pos = np.arange(len(df_filtered.groupby(['r_index', 's_index', 'join_algorithm'])))
        # Offset for bars, reduced spacing
        offset_width = bar_width * n_combinations / 2
        offsets = np.linspace(-offset_width, offset_width, n_combinations)
        
        # Loop through each combination of 'r_index', 's_index', 'join_algorithm'
        for j, ((r_index, s_index, join_algorithm), group) in enumerate(df_filtered.groupby(['r_index', 's_index', 'join_algorithm'])):
            # Create stacked bars for each combination
            label = f'{r_index}-{s_index}-{join_algorithm}'
            axes[k][i].bar(offsets[j], group['sum_join_time'], width=bar_width, edgecolor='white', label=label, color=color_map[label])
            
            axes[k][i].bar(offsets[j], group['sum_index_build_time'], color='lightgray', width=bar_width, edgecolor='white', bottom=group['sum_join_time'])
        
        # Add title and axis names
        axes[k][i].set_title(f'Data Size = {int(r_value/1e6)}M')
        # axes[i].set_xlabel('Combinations')
        axes[k][i].set_ylabel('Time (s)')  # Set y-axis label
        axes[k][i].set_xticklabels([])  # 隐藏x轴坐标
        
        # Set x-axis tick labels to represent each combination
        combination_labels = [f'{r}-{s}-{a}' for (r, s, a), group in df_filtered.groupby(['r_index', 's_index', 'join_algorithm'])]
    
# Create a single legend for the first figures of each row
handles, labels = axes[0][0].get_legend_handles_labels()
handles_1, labels_1 = axes[1][0].get_legend_handles_labels()
handles.extend(handles_1)
labels.extend(labels_1)


fig.legend(handles, labels, loc='center left', bbox_to_anchor=(0.15, 0.94), ncol=len(labels) // 3 + 1)

# Adjust the layout so the labels and title fit
# fig.tight_layout(rect=[0, 0, 1, 0.95])
plt.subplots_adjust(hspace=0.2, top=0.85)

# Show the plot
plt.show()
