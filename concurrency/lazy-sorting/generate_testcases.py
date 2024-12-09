import os
import random
import string
from datetime import datetime, timedelta

def generate_name(max_length=7):
    length = random.randint(1, max_length)
    return ''.join(random.choice(string.ascii_lowercase) for _ in range(length))

def generate_timestamp():
    start_date = datetime(1970, 1, 1)
    end_date = datetime.now()
    random_date = start_date + timedelta(
        seconds=random.randint(0, int((end_date - start_date).total_seconds())),
    )
    return random_date.isoformat().split('.')[0]

required_paths = ["testcases", "testcases/count", "testcases/count/small", "testcases/count/medium", "testcases/count/large", "testcases/merge", "testcases/merge/small", "testcases/merge/medium", "testcases/merge/large"]

for path in required_paths:
    if not os.path.exists(path):
        os.mkdir(path)

for i in range(5, 59):
    filename = "testcases/count/small/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = int(i/5)
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")

for i in range(60, 134):
    filename = "testcases/count/medium/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = int(i/5)
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")

for i in range(135, 219):
    filename = "testcases/count/large/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = int(i/5)
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")

exit(0)

for i in range(43, 1001):
    filename = "testcases/merge/small/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = i
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")

for i in range(1001, 100001)[::50]:
    filename = "testcases/merge/medium/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = i
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")

for i in range(100001, 1000001)[::500]:
    filename = "testcases/merge/large/" + str(i)
    if(os.path.exists(filename)):
        continue
    with open(filename, "w") as f:
        n = i
        f.write(str(n) + "\n")
        for j in range(n):
            f.write(generate_name() + " " + str(random.randint(1, 99999)) + " " + generate_timestamp() + "\n")
        f.write(random.choice(["Name", "ID", "Timestamp"]) + "\n")