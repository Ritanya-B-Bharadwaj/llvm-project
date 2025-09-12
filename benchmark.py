import subprocess
import json
import time
import os

original_cpp = "test.cpp"
modified_cpp = "modified.cpp"
original_bin = "test"
modified_bin = "test_modified"
json_file = "run_test.json"

def compile_and_time(source_file, output_bin):
    print(f"⏱️ Compiling {source_file}...")
    subprocess.run(["clang++", "-O3", source_file, "-o", output_bin], check=True)

    print(f"🚀 Running {output_bin}...")
    start = time.time()
    subprocess.run([f"./{output_bin}"], stdout=subprocess.DEVNULL)
    end = time.time()

    return round(end - start, 6)

def main():
    if not os.path.exists(original_cpp) or not os.path.exists(modified_cpp):
        print("❌ test.cpp or modified.cpp is missing.")
        return

    t_orig = compile_and_time(original_cpp, original_bin)
    t_mod = compile_and_time(modified_cpp, modified_bin)

    print(f"\n📊 Original Runtime: {t_orig:.6f} s")
    print(f"📉 Modified Runtime: {t_mod:.6f} s")

    speedup = round(t_orig / t_mod, 4) if t_mod > 0 else None

    # Overwrite with new benchmark
    data = [{
        "benchmark": {
            "original_time_sec": t_orig,
            "modified_time_sec": t_mod,
            "speedup_ratio": speedup
        }
    }]

    with open(json_file, "w") as f:
        json.dump(data, f, indent=2)

    print("✅ Results saved to run_test.json (overwritten)")

if __name__ == "__main__":
    main()
