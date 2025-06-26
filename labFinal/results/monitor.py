import time
import psutil
import subprocess
from concurrent.futures import ThreadPoolExecutor
import csv

def format_memory_size(size_in_bytes):
    if size_in_bytes < 1024:
        return f"{size_in_bytes:.3f} B"
    elif size_in_bytes < 1024 ** 2:
        return f"{size_in_bytes / 1024:.2f} KB"
    elif size_in_bytes < 1024 ** 3:
        return f"{size_in_bytes / 1024 ** 2:.2f} MB"
    else:
        return f"{size_in_bytes / 1024 ** 3:.2f} GB"

def monitorThread(pid, flag):
    max_mem = 0
    max_children = 0
    try:
        time.sleep(0.000001)
        process = psutil.Process(pid)
        mem = process.memory_full_info().uss
        max_mem = max(max_mem, mem)
        max_children = max(max_children, len(process.threads()))
    except psutil.NoSuchProcess:
        return max_children, max_mem
    
    while flag[0]:
        try:
            process = psutil.Process(pid)
            mem = process.memory_full_info().uss
            max_mem = max(max_mem, mem)
            max_children = max(max_children, len(process.threads()))
            time.sleep(0.00001)
        except psutil.NoSuchProcess:
            break
    return max_children, max_mem

# 输入配置（results/results2.csv）
test_cases = [
    "..\\circuits\\tiny",
    "..\\circuits\\small_dense",
    "..\\circuits\\med_sparse",
    "..\\circuits\\med_dense",
    "..\\circuits\\lg_sparse",
    "..\\circuits\\large_dense",
    "..\\circuits\\xl",
    "..\\circuits\\huge",
]

# channel_widths = [3, 6, 7, 20, 11, 37, 27, 31] #原算法(results.csv)
channel_widths = [3, 4, 7, 17, 9, 30, 27, 38] #优化算法(results2.csv)
thread_settings = [1, 2, 4, 8]  # 多线程设置

# 输入配置（results3.csv）
# test_cases = [
#     "..\\circuits\\small_dense",
#     "..\\circuits\\med_dense",
#     "..\\circuits\\large_dense",
#     "..\\circuits\\xl",
#     "..\\circuits\\huge",
# ]
# channel_widths = [5, 22, 39, 35, 49]
# thread_settings = [1]


pool = ThreadPoolExecutor()

with open("results2.csv", "w", newline="") as csvfile:
    fieldnames = ['test_case', 'threads', 'time_sec', 'acceleration_ratio', 'segments_used', 'peak_memory']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for test_case, width in zip(test_cases, channel_widths):
        baseline_time = None
        segments_used_1_thread = None  # 用于存储1个线程时的segments_used
        for threads in thread_settings:
            success = False
            retries = 0
            while not success:
                cmd = [
                    "../main.exe",
                    test_case, str(width), str(threads)
                ]
                print(f"Running: {cmd}")
                start = time.time()
                p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                flag = [True]
                t = pool.submit(monitorThread, pid=p.pid, flag=flag)
                out, err = p.communicate()
                flag[0] = False
                end = time.time()

                max_children, max_mem = t.result()
                duration = end - start

                if "Error: Routing not complete" in out or "Error: Routing not complete" in err:
                    print("Routing failed. Retrying...")
                    retries += 1
                    if retries >= 20:
                        print("Failed 20 times, skipping this setting.")
                        if baseline_time is None:
                            baseline_time = duration
                            acc = 1.0
                        else:
                            acc = baseline_time / duration
                        writer.writerow({
                            'test_case': test_case.split("\\")[-1],
                            'threads': threads,
                            'time_sec': round(duration, 4),
                            'acceleration_ratio': round(acc, 2),
                            'segments_used': segments_used_1_thread if threads != 1 else -1,
                            'peak_memory': format_memory_size(max_mem)
                        })
                        break
                    continue
                else:
                    success = True
                    # 提取 segments_used
                    segments = -1
                    for line in out.splitlines():
                        if "Segments used" in line:
                            segments = int(line.strip().split(":")[-1])
                            break

                    if threads == 1: 
                        segments_used_1_thread = segments

                    if baseline_time is None:
                        baseline_time = duration
                        acc = 1.0
                    else:
                        acc = baseline_time / duration

                    writer.writerow({
                        'test_case': test_case.split("\\")[-1],
                        'threads': threads,
                        'time_sec': round(duration, 4),
                        'acceleration_ratio': round(acc, 2),
                        'segments_used': segments_used_1_thread if threads != 1 else segments,
                        'peak_memory': format_memory_size(max_mem)
                    })