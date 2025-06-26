import subprocess
import time
import csv
from concurrent.futures import ThreadPoolExecutor, as_completed

test_cases = [
    ".\\circuits\\tiny",
    ".\\circuits\\small_dense",
    ".\\circuits\\med_sparse",
    ".\\circuits\\med_dense",
    ".\\circuits\\lg_sparse",
    ".\\circuits\\large_dense",
    ".\\circuits\\xl",
    ".\\circuits\\huge",
]

special_cases_start_width = {
    "tiny": 3,
    "small_dense": 4,
    "med_sparse": 7,
    "med_dense": 17,
    "lg_sparse": 10,
    "large_dense": 30,
    "xl": 26,
    "huge": 37
}

max_retries = 10
max_width = 100
num_threads = 1  # 固定使用1个线程测试W
exe_path = ".\\main2.exe"

def test_min_width(test_case):
    base_name = test_case.split("\\")[-1]
    start_width = special_cases_start_width.get(base_name, 1)
    width = start_width
    while width <= max_width:
        for attempt in range(max_retries):
            cmd = [exe_path, test_case, str(width), str(num_threads)]
            proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=300)
            output = proc.stdout + proc.stderr
            if "Error: Routing not complete" not in output:
                print(f"[{base_name}] ✅ Success at W={width} (attempt {attempt+1})")
                return base_name, width
            else:
                print(f"[{base_name}] ❌ W={width} attempt {attempt+1} failed")
                time.sleep(0.2)

        width += 1
    return base_name, "Not Found"

# 线程池并发执行
results = []
with ThreadPoolExecutor(max_workers=8) as executor:
    futures = [executor.submit(test_min_width, test_case) for test_case in test_cases]
    for future in as_completed(futures):
        results.append(future.result())

# 写入CSV
with open("min_width_results.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["Test Case", "Min Width"])
    writer.writerows(results)

print("\n✅ 所有测试完成，最小通道宽度如下：")
for name, w in results:
    print(f"  {name}: {w}")
