import os
import matplotlib.pyplot as plt
import numpy as np
import re
import subprocess
import sys

if os.path.exists("data.txt") and os.path.exists("data_dist.txt"):
    filename = "data.txt"
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
    mergesortshit = data[4]
    filename = "data_dist.txt"
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
    distmergesortshit = data[4]
    plt.figure()
    plt.plot(data[3], mergesortshit)
    plt.figure()
    plt.plot(data[3], distmergesortshit)
    plt.show()
    exit(0)

requiredpaths = ["results", "results/count", "results/count/small", "results/count/medium", "results/count/large", "results/merge", "results/merge/small", "results/merge/medium", "results/merge/large"]
for path in requiredpaths:
    if not os.path.exists(path):
        os.mkdir(path)

subprocess.Popen(["make","mode=distributed"]).wait()
print("Hello")

def parse_file(file_path):
    contents = None
    with open(file_path, 'r') as file:
        contents = file.read()
        pattern = r'\s*User time \(seconds\): (\d+\.\d+)\s*\n\s*System time \(seconds\): (\d+\.\d+)'
        pattern2 = r'Maximum resident set size \(kbytes\): (\d+)'
        match = re.search(pattern, contents)
        match2 = re.search(pattern2, contents)

        if match and match2:
            utime, systime = float(match.group(1)), float(match.group(2))
            mem = int(match2.group(1))
            return utime + systime, mem
        else:
            raise Exception("Error parsing file")

countN = []
countMem = []
countTime = []

mergeN = []
mergeMem = []
mergeTime = []

for size in ["small", "medium", "large"]:
    st, sm = 0, 0
    # print(size)
    arr = os.listdir("testcases/count/" + size)
    arr.sort(key=lambda x: int(x))
    # print(arr)
    for file in arr:
        # breakpoint()
        input_file = "testcases/count/" + size + "/" + file
        output_file = "results/count/" + size + "/" + file
        if not os.path.exists(output_file):
            with open(input_file, "r") as infile, open(output_file, "w") as outfile:
                subprocess.Popen(["./measure.sh", "./lazy-sort"], stdin=infile, stdout=subprocess.DEVNULL, stderr=outfile).wait()
        # breakpoint()
        print(output_file)
        time, mem = parse_file(output_file)
        # print(mem)
        # breakpoint()
        st += time
        sm += mem
        if int(file) > 5 and int(file) % 5 == 0:
            countN.append(int(file)/5)
            countMem.append(sm/5)
            countTime.append(st/5)
            st, sm = 0, 0

with open("data_dist.txt", "a") as f:
    f.write(str(countN) + "\n")
    f.write(str(countMem) + "\n")
    f.write(str(countTime) + "\n")


for size in ["small", "medium", "large"]:
    st, sm = 0, 0
    # print(size)
    arr = os.listdir("testcases/merge/" + size)
    arr.sort(key=lambda x: int(x))
    # print(arr)
    for file in arr:
        # breakpoint()
        input_file = "testcases/merge/" + size + "/" + file
        output_file = "results/merge/" + size + "/" + file
        if not os.path.exists(output_file):
            with open(input_file, "r") as infile, open(output_file, "w") as outfile:
                subprocess.Popen(["./measure.sh", "./lazy-sort"], stdin=infile, stdout=subprocess.DEVNULL, stderr=outfile).wait()
        # breakpoint()
        print(output_file)
        time, mem = parse_file(output_file)
        # print(mem)
        # breakpoint()
        st += time
        sm += mem
        mergeN.append(int(file))
        mergeMem.append(sm)
        mergeTime.append(st)
        st, sm = 0, 0

with open("data_dist.txt", "a") as f:
    f.write(str(mergeN) + "\n")
    f.write(str(mergeMem) + "\n")
    f.write(str(mergeTime) + "\n")

# Plot countTime vs N
plt.figure()
plt.plot(countN, countTime)
plt.title("Count Time vs input size")
plt.xlabel("N")
plt.ylabel("Count Time")

# Plot countMem vs N
plt.figure()
plt.plot(countN, countMem)
plt.title("Count Memory vs input size")
plt.xlabel("N")
plt.ylabel("Count Memory (GB)")

# Plot mergeTime vs N
plt.figure()
plt.plot(mergeN, mergeTime)
plt.title("Merge Time vs input size")
plt.xlabel("N")
plt.ylabel("Merge Time")

# Plot mergeMem vs N
plt.figure()
plt.plot(mergeN, mergeMem)
plt.title("Merge Memory vs input size")
plt.xlabel("N")
plt.ylabel("Memory usage (KB)")

plt.show()
