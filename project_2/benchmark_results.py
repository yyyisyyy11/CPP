"""
Benchmark results collector and visualizer.
Reads CSV output from C (O0, O3) and Java benchmarks,
merges into benchmark_results.txt, and generates charts.
"""

import subprocess
import csv
import io
import os
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
from datetime import date

matplotlib.rcParams['font.family'] = 'sans-serif'
matplotlib.rcParams['font.sans-serif'] = ['Arial', 'Helvetica', 'DejaVu Sans']

PROJ_DIR = os.path.dirname(os.path.abspath(__file__))
RESULTS_FILE = os.path.join(PROJ_DIR, "benchmark_results.txt")
CHART_FILE = os.path.join(PROJ_DIR, "benchmark_chart.png")

TYPES = ["float", "double", "int", "short", "char"]
TYPE_LABELS = {"char": "signed char / byte"}
SIZES = [128, 1000, 10000, 100000, 1000000, 10000000, 100000000]


def run_and_parse(cmd, label):
    """Run a command, parse CSV output (type,n,avg_ms), return dict."""
    print(f"  Running {label}...")
    result = subprocess.run(cmd, capture_output=True, text=True, shell=True,
                            cwd=PROJ_DIR)
    if result.returncode != 0:
        print(f"  ERROR: {result.stderr[:200]}")
        return {}

    data = {}
    reader = csv.DictReader(io.StringIO(result.stdout))
    for row in reader:
        dtype = row["type"].strip()
        # Normalize Java "byte" to "char" for consistency
        if dtype == "byte":
            dtype = "char"
        n = int(row["n"])
        avg_ms = float(row["avg_ms"])
        data[(dtype, n)] = avg_ms
    return data


def compile_c():
    """Compile C benchmarks with O0 and O3."""
    print("Compiling C benchmarks...")
    os.system(f"cd {PROJ_DIR} && gcc -o dotproduct_O0 dotproduct.c")
    os.system(f"cd {PROJ_DIR} && gcc -O3 -o dotproduct_O3 dotproduct.c")


def collect_all():
    """Run all benchmarks and collect results."""
    compile_c()

    print("\nCollecting benchmark data (5-trial averages):")
    c_o0 = run_and_parse(f"cd {PROJ_DIR} && ./dotproduct_O0", "C-O0")
    c_o3 = run_and_parse(f"cd {PROJ_DIR} && ./dotproduct_O3", "C-O3")
    java = run_and_parse(f"cd {PROJ_DIR} && java Dotproduct.java", "Java")

    return c_o0, c_o3, java


def write_results(c_o0, c_o3, java):
    """Write formatted results to benchmark_results.txt."""
    with open(RESULTS_FILE, "w") as f:
        f.write("=" * 80 + "\n")
        f.write("                      Dot Product Benchmark Results\n")
        f.write("=" * 80 + "\n")
        f.write(f"Platform: macOS (Apple Silicon)\n")
        f.write(f"Date: {date.today()}\n")
        f.write(f"Seed: 42 (fixed for reproducibility)\n")
        f.write(f"Trials: 1 (single run)\n")
        f.write("\nConfigurations:\n")
        f.write("  [C-O0]  gcc (no optimization)        : gcc -o dotproduct_O0 dotproduct.c\n")
        f.write("  [C-O3]  gcc -O3 optimization         : gcc -O3 -o dotproduct_O3 dotproduct.c\n")
        f.write("  [Java]  java (JIT)                    : java Dotproduct.java\n")

        # --- By data type ---
        f.write("\n" + "=" * 80 + "\n")
        f.write("                        1. Grouped by Data Type\n")
        f.write("=" * 80 + "\n")

        for dtype in TYPES:
            label = TYPE_LABELS.get(dtype, dtype)
            f.write(f"\n--- {label} ({_type_size(dtype)} bytes) ---\n")
            f.write(f"{'n':>14s} | {'C-O0 (ms)':>12s} | {'C-O3 (ms)':>11s} | {'Java (ms)':>11s}\n")
            f.write("-" * 15 + "|" + "-" * 14 + "|" + "-" * 13 + "|" + "-" * 13 + "\n")
            for n in SIZES:
                v0 = c_o0.get((dtype, n), 0)
                v3 = c_o3.get((dtype, n), 0)
                vj = java.get((dtype, n), 0)
                f.write(f"{n:>14,d} | {v0:>12.4f} | {v3:>11.4f} | {vj:>11.4f}\n")

        # --- By vector size ---
        f.write("\n" + "=" * 80 + "\n")
        f.write("                   2. Grouped by Vector Size (n)\n")
        f.write("=" * 80 + "\n")

        for n in SIZES:
            f.write(f"\n--- n = {n:,d} ---\n")
            f.write(f"  {'Data Type':15s} | {'C-O0 (ms)':>12s} | {'C-O3 (ms)':>11s} | {'Java (ms)':>11s}\n")
            f.write("  " + "-" * 16 + "|" + "-" * 14 + "|" + "-" * 13 + "|" + "-" * 13 + "\n")
            for dtype in TYPES:
                label = TYPE_LABELS.get(dtype, dtype)
                v0 = c_o0.get((dtype, n), 0)
                v3 = c_o3.get((dtype, n), 0)
                vj = java.get((dtype, n), 0)
                f.write(f"  {label:15s} | {v0:>12.4f} | {v3:>11.4f} | {vj:>11.4f}\n")

        # --- Speedup ratios ---
        f.write("\n" + "=" * 80 + "\n")
        f.write("                   3. Speedup Ratios (at n = 100,000,000)\n")
        f.write("=" * 80 + "\n")

        big_n = 100000000

        f.write("\n--- O3 vs O0 Speedup (C-O0 / C-O3) ---\n")
        for dtype in TYPES:
            label = TYPE_LABELS.get(dtype, dtype)
            v0 = c_o0.get((dtype, big_n), 1)
            v3 = c_o3.get((dtype, big_n), 1)
            ratio = v0 / v3 if v3 > 0 else 0
            f.write(f"  {label:13s}:  {v0:.2f} / {v3:.2f} = {ratio:5.2f}x\n")

        f.write("\n--- C-O0 vs Java (C-O0 / Java) ---\n")
        for dtype in TYPES:
            label = TYPE_LABELS.get(dtype, dtype)
            v0 = c_o0.get((dtype, big_n), 1)
            vj = java.get((dtype, big_n), 1)
            ratio = v0 / vj if vj > 0 else 0
            note = "(Java faster)" if ratio > 1 else "(C-O0 faster)"
            f.write(f"  {label:13s}:  {v0:.2f} / {vj:.2f} = {ratio:5.2f}x  {note}\n")

        f.write("\n--- C-O3 vs Java (C-O3 / Java) ---\n")
        for dtype in TYPES:
            label = TYPE_LABELS.get(dtype, dtype)
            v3 = c_o3.get((dtype, big_n), 1)
            vj = java.get((dtype, big_n), 1)
            ratio = v3 / vj if vj > 0 else 0
            if ratio == 0:
                note = "(no data)"
            elif ratio > 1:
                note = "(Java faster)"
            elif ratio < 0.9:
                note = f"(C-O3 faster, {1/ratio:.2f}x)"
            else:
                note = "(roughly equal)"
            f.write(f"  {label:13s}:  {v3:.2f} / {vj:.2f} = {ratio:5.2f}x  {note}\n")

        f.write("\n" + "=" * 80 + "\n")

    print(f"\nResults written to {RESULTS_FILE}")


def _type_size(dtype):
    return {"float": 4, "double": 8, "int": 4, "short": 2, "char": 1}[dtype]


def make_chart(c_o0, c_o3, java):
    """Generate benchmark chart with subplots per data type."""
    fig, axes = plt.subplots(2, 3, figsize=(16, 10))
    fig.suptitle("Dot Product Benchmark: C (O0) vs C (O3) vs Java",
                 fontsize=15, fontweight='bold')

    # Dark theme
    fig.patch.set_facecolor('#1e1e2e')
    colors = {'C-O0': '#f38ba8', 'C-O3': '#94e2d5', 'Java': '#f9e2af'}
    markers = {'C-O0': 'o', 'C-O3': 's', 'Java': '^'}

    for idx, dtype in enumerate(TYPES):
        ax = axes[idx // 3][idx % 3]
        ax.set_facecolor('#1e1e2e')

        label = TYPE_LABELS.get(dtype, dtype)

        for name, data in [("C-O0", c_o0), ("C-O3", c_o3), ("Java", java)]:
            times = [data.get((dtype, n), None) for n in SIZES]
            valid_n = [n for n, t in zip(SIZES, times) if t is not None and t > 0]
            valid_t = [t for t in times if t is not None and t > 0]
            if valid_n:
                ax.plot(valid_n, valid_t, marker=markers[name], label=name,
                        color=colors[name], linewidth=1.8, markersize=6, alpha=0.9)

        ax.set_xscale('log')
        ax.set_yscale('log')
        ax.set_title(f"Data Type: {label}", color='white', fontsize=12)
        ax.set_xlabel("Vector Size (n)", color='#cdd6f4', fontsize=9)
        ax.set_ylabel("Runtime (ms)", color='#cdd6f4', fontsize=9)
        ax.legend(fontsize=8, facecolor='#313244', edgecolor='#45475a',
                  labelcolor='white')
        ax.tick_params(colors='#cdd6f4')
        ax.grid(True, alpha=0.2, color='#585b70')
        for spine in ax.spines.values():
            spine.set_color('#45475a')

    # Hide empty subplot
    axes[1][2].set_visible(False)

    plt.tight_layout()
    plt.savefig(CHART_FILE, dpi=150, facecolor=fig.get_facecolor(),
                bbox_inches='tight')
    plt.close()
    print(f"Chart saved to {CHART_FILE}")


if __name__ == "__main__":
    c_o0, c_o3, java = collect_all()
    write_results(c_o0, c_o3, java)
    make_chart(c_o0, c_o3, java)
    print("\nDone!")
