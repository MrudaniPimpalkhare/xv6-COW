import json

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
data_txt = read_data('data.txt')
data_dist_txt = read_data('data_dist.txt')

# Extract data for count sort and merge sort
count_sort_files = data_txt[0]
count_sort_memory = data_txt[1]
count_sort_time = data_txt[2]

dist_count_sort_files = data_dist_txt[0]
dist_count_sort_memory = data_dist_txt[1]
dist_count_sort_time = data_dist_txt[2]

merge_sort_files = data_txt[3]
merge_sort_memory = data_txt[4]
merge_sort_time = data_txt[5]

dist_merge_sort_files = data_dist_txt[3]
dist_merge_sort_memory = data_dist_txt[4]
dist_merge_sort_time = data_dist_txt[5]

# File sizes to extract
file_sizes_to_extract = [43, 44, 45, 46, 2748, 2749, 2750, 2751, 10515, 10516, 10517, 10518]

# Function to extract values for specified file sizes
def extract_values(data, file_sizes):
    indices = [data[0].index(size) if size in data[0] else None for size in file_sizes]
    memory_usage = [data[1][i] if i is not None else 0 for i in indices]
    time_taken = [data[2][i] if i is not None else 0 for i in indices]
    return memory_usage, time_taken

# Extract values for count sort
count_sort_memory_values, count_sort_time_values = extract_values(data_txt, file_sizes_to_extract)

# Extract values for distributed count sort
dist_count_sort_memory_values, dist_count_sort_time_values = extract_values(data_dist_txt, file_sizes_to_extract)

# Extract values for merge sort
merge_sort_memory_values, merge_sort_time_values = extract_values(data_txt, file_sizes_to_extract)

# Extract values for distributed merge sort
dist_merge_sort_memory_values, dist_merge_sort_time_values = extract_values(data_dist_txt, file_sizes_to_extract)

# Print the extracted values
print("Count Sort Memory Usage:", count_sort_memory_values)
print("Count Sort Time Taken:", count_sort_time_values)

print("Distributed Count Sort Memory Usage:", dist_count_sort_memory_values)
print("Distributed Count Sort Time Taken:", dist_count_sort_time_values)

print("Merge Sort Memory Usage:", merge_sort_memory_values)
print("Merge Sort Time Taken:", merge_sort_time_values)

print("Distributed Merge Sort Memory Usage:", dist_merge_sort_memory_values)
print("Distributed Merge Sort Time Taken:", dist_merge_sort_time_values)