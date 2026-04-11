import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

# ==================== Raw Data ====================

n_values = [128, 1000, 10000, 100000, 1000000, 10000000, 100000000]

# C-O0
c_o0 = {
    "float":  [0.000977, 0.005127, 0.050781, 0.430176, 3.353027, 14.509033, 134.387207],
    "double": [0.000000, 0.000977, 0.008057, 0.090820, 1.610107, 21.595947, 238.629150],
    "int":    [0.000000, 0.000977, 0.008057, 0.079102, 0.782959,  7.789795,  79.414062],
    "short":  [0.000000, 0.001221, 0.008057, 0.086914, 0.768066,  9.623047,  78.922852],
    "char":   [0.000000, 0.000977, 0.008057, 0.080078, 0.781006,  7.795898,  91.204834],
}

# C-O3
c_o3 = {
    "float":  [0.000000, 0.000977, 0.015137, 0.122070, 1.118896,  4.855957,  51.001953],
    "double": [0.000000, 0.000000, 0.007080, 0.050049, 0.523193,  6.023926,  62.668945],
    "int":    [0.000000, 0.000000, 0.000000, 0.007080, 0.071045,  0.964111,  11.112061],
    "short":  [0.000000, 0.000000, 0.002197, 0.016113, 0.162842,  1.831787,  16.626953],
    "char":   [0.000000, 0.000000, 0.001953, 0.017090, 0.170898,  1.687012,  22.991211],
}

# Java
java = {
    "float":  [0.001666, 0.006166, 0.058291, 0.598875, 0.548583,  5.189500,  51.917958],
    "double": [0.002084, 0.006125, 0.056833, 0.568292, 0.551916,  5.307542,  53.231625],
    "int":    [0.001958, 0.006125, 0.055750, 0.579584, 0.246625,  2.304292,  23.623166],
    "short":  [0.001667, 0.005958, 0.061000, 0.586500, 0.246333,  2.325750,  23.256542],
    "char":   [0.001458, 0.006000, 0.053584, 0.592000, 0.246750,  2.321125,  23.250083],
}

# Type name mapping for display
type_labels = {
    "float": "float (4B)",
    "double": "double (8B)",
    "int": "int (4B)",
    "short": "short (2B)",
    "char": "signed char / byte (1B)",
}

# ==================== Generate Text Table ====================

def fmt(v):
    """Format a time value with appropriate precision."""
    if v < 0.001:
        return f"{v*1000:>8.3f} µs"
    elif v < 1:
        return f"{v:>10.6f} ms"
    else:
        return f"{v:>10.4f} ms"

with open("/Users/mac/Documents/GitHub/CPP/project_2/benchmark_results.txt", "w") as f:
    f.write("=" * 90 + "\n")
    f.write("                    Dot Product Benchmark Results (Single Run)\n")
    f.write("=" * 90 + "\n")
    f.write("Platform: macOS (Apple Silicon)\n")
    f.write("Date: 2026-04-11\n")
    f.write("Seed: 42 (fixed)\n\n")
    f.write("Configurations:\n")
    f.write("  [C-O0]  gcc (no optimization)\n")
    f.write("  [C-O3]  gcc -O3 optimization\n")
    f.write("  [Java]  java (JIT)\n\n")

    # --- By data type ---
    f.write("=" * 90 + "\n")
    f.write("                        1. Grouped by Data Type\n")
    f.write("=" * 90 + "\n\n")

    for dtype in ["float", "double", "int", "short", "char"]:
        f.write(f"--- {type_labels[dtype]} ---\n")
        f.write(f"{'n':>15} | {'C-O0 (ms)':>14} | {'C-O3 (ms)':>14} | {'Java (ms)':>14}\n")
        f.write("-" * 15 + "-+-" + "-" * 14 + "-+-" + "-" * 14 + "-+-" + "-" * 14 + "\n")
        for i, n in enumerate(n_values):
            f.write(f"{n:>15,} | {c_o0[dtype][i]:>14.6f} | {c_o3[dtype][i]:>14.6f} | {java[dtype][i]:>14.6f}\n")
        f.write("\n")

    # --- By vector size ---
    f.write("=" * 90 + "\n")
    f.write("                     2. Grouped by Vector Size (n)\n")
    f.write("=" * 90 + "\n\n")

    for i, n in enumerate(n_values):
        f.write(f"--- n = {n:,} ---\n")
        f.write(f"{'Data Type':>18} | {'C-O0 (ms)':>14} | {'C-O3 (ms)':>14} | {'Java (ms)':>14}\n")
        f.write("-" * 18 + "-+-" + "-" * 14 + "-+-" + "-" * 14 + "-+-" + "-" * 14 + "\n")
        for dtype in ["float", "double", "int", "short", "char"]:
            label = type_labels[dtype].split(" (")[0]
            f.write(f"{label:>18} | {c_o0[dtype][i]:>14.6f} | {c_o3[dtype][i]:>14.6f} | {java[dtype][i]:>14.6f}\n")
        f.write("\n")

    # --- Speedup ratios ---
    f.write("=" * 90 + "\n")
    f.write("               3. Speedup Ratios (at n = 100,000,000)\n")
    f.write("=" * 90 + "\n\n")

    f.write("--- O3 vs O0 Speedup (C-O0 / C-O3) ---\n")
    for dtype in ["float", "double", "int", "short", "char"]:
        r = c_o0[dtype][-1] / c_o3[dtype][-1] if c_o3[dtype][-1] > 0 else float('inf')
        label = type_labels[dtype].split(" (")[0]
        f.write(f"  {label:<12}: {c_o0[dtype][-1]:>8.2f} / {c_o3[dtype][-1]:>8.2f} = {r:>5.2f}x\n")

    f.write("\n--- C-O3 vs Java ---\n")
    for dtype in ["float", "double", "int", "short", "char"]:
        c = c_o3[dtype][-1]
        j = java[dtype][-1]
        ratio = c / j
        if ratio < 1:
            note = f"C-O3 faster, {1/ratio:.2f}x"
        elif ratio > 1.1:
            note = f"Java faster, {ratio:.2f}x"
        else:
            note = "roughly equal"
        label = type_labels[dtype].split(" (")[0]
        f.write(f"  {label:<12}: {c:>8.2f} / {j:>8.2f} = {ratio:>5.2f}x  ({note})\n")

    f.write("\n" + "=" * 90 + "\n")

print("benchmark_results.txt generated.")

# ==================== Generate Charts ====================

colors = {"C-O0": "#FF6B6B", "C-O3": "#4ECDC4", "Java": "#FFD93D"}
markers = {"C-O0": "o", "C-O3": "s", "Java": "^"}

plt.rcParams.update({
    "figure.facecolor": "#1a1a2e",
    "axes.facecolor": "#16213e",
    "axes.edgecolor": "#e0e0e0",
    "axes.labelcolor": "#e0e0e0",
    "xtick.color": "#e0e0e0",
    "ytick.color": "#e0e0e0",
    "text.color": "#e0e0e0",
    "grid.color": "#2a2a4a",
    "grid.alpha": 0.6,
    "font.size": 11,
    "font.family": "sans-serif",
})

fig, axes = plt.subplots(2, 3, figsize=(18, 10))
fig.suptitle("Dot Product Benchmark: C (O0) vs C (O3) vs Java  [Single Run]",
             fontsize=18, fontweight="bold", color="white", y=0.98)

axes_flat = axes.flatten()
MIN_VAL = 0.0001  # replace 0s for log scale

for idx, dtype in enumerate(["float", "double", "int", "short", "char"]):
    ax = axes_flat[idx]

    for label, src in [("C-O0", c_o0), ("C-O3", c_o3), ("Java", java)]:
        times = [max(t, MIN_VAL) for t in src[dtype]]
        ax.plot(n_values, times,
                color=colors[label], marker=markers[label],
                linewidth=2.2, markersize=7, label=label,
                markeredgecolor="white", markeredgewidth=0.8, alpha=0.9)

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_title(f"{type_labels[dtype]}", fontsize=13, fontweight="bold",
                 pad=8, color="#f0f0f0")
    ax.set_xlabel("Vector Size (n)")
    ax.set_ylabel("Runtime (ms)")
    ax.legend(loc="upper left", fontsize=9, framealpha=0.3, edgecolor="#555")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

axes_flat[5].axis("off")
plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.savefig("/Users/mac/Documents/GitHub/CPP/project_2/benchmark_chart.png",
            dpi=200, bbox_inches="tight")
print("benchmark_chart.png generated.")
