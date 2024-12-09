import matplotlib.pyplot as plt
import numpy as np

# Function to read data from a file
def read_data(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        data = []
        current_array = []
        for line in lines:
            line = line.strip()
            if line.startswith('[') and line.endswith(']'):
                current_array.extend(eval(line))
                data.append(current_array)
                current_array = []
            else:
                current_array.extend(eval(line))
        if current_array:
            data.append(current_array)
    return data

# Read data from files
data_normal = read_data('data.txt')
data_dist = read_data('data_dist.txt')

# Extract data for count sort and merge sort
count_sort_files = data_normal[0]
count_sort_memory = data_normal[1]
count_sort_time = data_normal[2]

dist_count_sort_files = data_dist[0]
dist_count_sort_memory = data_dist[1]
dist_count_sort_time = data_dist[2]

merge_sort_files = data_normal[3]
merge_sort_memory = data_normal[4]
merge_sort_time = data_normal[5]

dist_merge_sort_files = data_dist[3]
dist_merge_sort_memory = data_dist[4]
dist_merge_sort_time = data_dist[5]

# list = [43, 44, 45, 46, 2748, 2749, 2750, 2751,10515, 10516, 10517, 10518]

# memory_merge = []
# time_merge = []
# dist_memory_merge = []
# dist_time_merge = []

# for i in list: 
#     memory_merge.append(merge_sort_memory[i-43])
#     time_merge.append(merge_sort_time[i-43])

# for j in list:
#     dist_memory_merge.append(dist_merge_sort_memory[j-43])
#     dist_time_merge.append(dist_merge_sort_time[i-43])

# print("memory for merge: ")
# print(memory_merge)

# print("time for merge")
# print(time_merge)

# print("dist memroy")
# print(dist_memory_merge)

# print("dist time")
# print(dist_time_merge)




# Plot comparison of times for count sort and distributed count sort
plt.figure(figsize=(10, 6))
plt.plot(count_sort_files, count_sort_time, label='Count Sort Time', marker='o')
plt.plot(dist_count_sort_files, dist_count_sort_time, label='Distributed Count Sort Time', marker='o')
plt.xlabel('Number of Files')
plt.ylabel('Time (s)')
plt.title('Comparison of Count Sort and Distributed Count Sort Times')
plt.legend(loc='upper left')  # Specify a fixed location for the legend
plt.grid(True)
plt.savefig('count_sort_time_comparison.png')

# Plot comparison of times for merge sort and distributed merge sort
plt.figure(figsize=(10, 6))
plt.plot(merge_sort_files, merge_sort_time, label='Merge Sort Time', marker='o')
plt.plot(dist_merge_sort_files, dist_merge_sort_time, label='Distributed Merge Sort Time', marker='o')
plt.xlabel('Number of Files')
plt.ylabel('Time (s)')
plt.title('Comparison of Merge Sort and Distributed Merge Sort Times')
plt.legend(loc='upper left')  # Specify a fixed location for the legend
plt.grid(True)
plt.savefig('merge_sort_time_comparison.png')

# Plot comparison of memory usage for count sort and distributed count sort
labels = [str(f) for f in count_sort_files]
x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots(figsize=(10, 6))
rects1 = ax.bar(x - width/2, count_sort_memory, width, label='Count Sort Memory')
rects2 = ax.bar(x + width/2, dist_count_sort_memory, width, label='Distributed Count Sort Memory')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_xlabel('Number of Files')
ax.set_ylabel('Memory Usage (MB)')
ax.set_title('Comparison of Memory Usage for Count Sort and Distributed Count Sort')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend(loc='upper left')  # Specify a fixed location for the legend

fig.tight_layout()
plt.grid(True)
plt.savefig('count_sort_memory_comparison.png')

# Plot comparison of memory usage for merge sort and distributed merge sort
labels = [str(f) for f in merge_sort_files]
x = np.arange(len(labels))  # the label locations

fig, ax = plt.subplots(figsize=(10, 6))
rects1 = ax.bar(x - width/2, merge_sort_memory, width, label='Merge Sort Memory')
rects2 = ax.bar(x + width/2, dist_merge_sort_memory, width, label='Distributed Merge Sort Memory')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_xlabel('Number of Files')
ax.set_ylabel('Memory Usage (MB)')
ax.set_title('Comparison of Memory Usage for Merge Sort and Distributed Merge Sort')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend(loc='upper left')  # Specify a fixed location for the legend

fig.tight_layout()
plt.grid(True)
plt.savefig('merge_sort_memory_comparison.png')

print("Graphs have been saved as images.")