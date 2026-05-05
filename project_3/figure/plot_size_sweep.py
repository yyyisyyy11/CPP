#!/usr/bin/env python3
"""
Plot size-sweep benchmark results: improved (NEON) vs OpenBLAS (AMX).
Shows two panels:
  1. GFLOPS vs matrix size (both implementations)
  2. Gap ratio (OpenBLAS / improved) vs matrix size

Demonstrates the compute-bound → memory-bound transition.
"""

import matplotlib.pyplot as plt
import numpy as np

# ── Raw data from benchmark ──
data = [
    (128,   0.2,   35.2,  279.6,  7.93),
    (256,   0.8,   78.2,  335.5,  4.29),
    (384,   1.7,  139.1,  976.3,  7.02),
    (512,   3.0,  160.6, 1095.7,  6.82),
    (640,   4.7,  196.9, 1510.9,  7.67),
    (768,   6.8,  242.2, 1429.0,  5.90),
    (896,   9.2,  305.8, 1625.6,  5.32),
    (1024, 12.0,  308.9, 1357.4,  4.39),
    (1280, 18.8,  374.7, 1346.1,  3.59),
    (1536, 27.0,  310.3, 1323.3,  4.27),
    (2048, 48.0,  356.2,  617.9,  1.73),
    (3072,108.0,  363.1,  481.3,  1.33),
    (4096,192.0,  352.9,  477.4,  1.35),
    (6144,432.0,  361.3,  526.7,  1.46),
    (8192,768.0,  373.0,  423.0,  1.13),
]

sizes       = np.array([d[0] for d in data])
data_mb     = np.array([d[1] for d in data])
improved    = np.array([d[2] for d in data])
openblas    = np.array([d[3] for d in data])
gap         = np.array([d[4] for d in data])

# ── Style ──
plt.rcParams.update({
    'font.family': 'sans-serif',
    'font.size': 11,
    'axes.grid': True,
    'grid.alpha': 0.25,
    'figure.facecolor': 'white',
})

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 8),
                                gridspec_kw={'height_ratios': [2.2, 1], 'hspace': 0.12})

x = np.arange(len(sizes))

# ── Panel 1: GFLOPS comparison ──
ax1.plot(x, openblas, 'o-', color='#e74c3c', linewidth=2.2, markersize=7,
         label='OpenBLAS (AMX)', zorder=5)
ax1.plot(x, improved, 's-', color='#2980b9', linewidth=2.2, markersize=7,
         label='Improved (NEON)', zorder=5)

# Shade compute-bound vs memory-bound regions
transition = 10  # index of size=2048 (first clearly memory-bound)
ax1.axvspan(-0.5, transition - 0.5, alpha=0.07, color='red')
ax1.axvspan(transition - 0.5, len(sizes) - 0.5, alpha=0.07, color='blue')
ax1.axvline(x=transition - 0.5, color='gray', linestyle='--', alpha=0.4, linewidth=1)

# NEON theoretical peak
ax1.axhline(y=483, color='#2980b9', linestyle=':', alpha=0.4, linewidth=1)
ax1.text(12, 500, 'NEON peak ≈ 483 GF', fontsize=9, color='#2980b9', alpha=0.6)

# Annotations
ax1.annotate('AMX peak:\n1626 GF @896',
             xy=(6, 1625.6), xytext=(8, 1700),
             fontsize=8, color='#c0392b', ha='center',
             arrowprops=dict(arrowstyle='->', color='#c0392b', lw=1))

ax1.annotate('Gap collapses\nat 2K+',
             xy=(10, 617.9), xytext=(12, 900),
             fontsize=9, color='gray', ha='center', fontstyle='italic',
             arrowprops=dict(arrowstyle='->', color='gray', lw=1))

ax1.set_ylabel('GFLOPS', fontsize=12)
ax1.set_title('Size Sweep: Improved (NEON) vs OpenBLAS (AMX) on Apple M5\n'
              'Verifying compute-bound → memory-bound transition',
              fontsize=13, fontweight='bold')
ax1.legend(loc='upper left', fontsize=10)
ax1.set_ylim(0, 1900)
ax1.set_xlim(-0.5, len(sizes) - 0.5)
ax1.set_xticks(x)
ax1.set_xticklabels([])  # hide, shared with bottom panel

# Add regime labels
ax1.text(4.5, 1820, 'Compute-bound\n(data fits in caches)',
         fontsize=10, ha='center', color='#c0392b', alpha=0.7, fontstyle='italic')
ax1.text(12, 1820, 'Memory-bound\n(data exceeds caches)',
         fontsize=10, ha='center', color='#2471a3', alpha=0.7, fontstyle='italic')

# ── Panel 2: Gap ratio ──
colors = ['#e74c3c' if g > 3 else '#f39c12' if g > 2 else '#27ae60' for g in gap]
bars = ax2.bar(x, gap, color=colors, alpha=0.8, width=0.65, edgecolor='white', linewidth=0.5)

# Value labels
for i, g in enumerate(gap):
    ax2.text(i, g + 0.15, f'{g:.1f}×', ha='center', fontsize=8, fontweight='bold',
             color='#333')

ax2.axhline(y=1.0, color='green', linestyle='--', alpha=0.4, linewidth=1)
ax2.axvspan(-0.5, transition - 0.5, alpha=0.07, color='red')
ax2.axvspan(transition - 0.5, len(sizes) - 0.5, alpha=0.07, color='blue')
ax2.axvline(x=transition - 0.5, color='gray', linestyle='--', alpha=0.4, linewidth=1)

ax2.set_ylabel('Gap Ratio\n(OpenBLAS / Improved)', fontsize=10)
ax2.set_xlabel('Matrix Size N×N  (total data = 3×N²×4 bytes)', fontsize=11)
ax2.set_ylim(0, 10)
ax2.set_xlim(-0.5, len(sizes) - 0.5)

# X-axis: size + data size
labels = [f'{s}\n({d:.0f}MB)' if d >= 1 else f'{s}\n({d:.1f}MB)' for s, d in zip(sizes, data_mb)]
ax2.set_xticks(x)
ax2.set_xticklabels(labels, fontsize=8)

plt.tight_layout()
for fmt_path in [
    '/Users/mac/Documents/GitHub/CPP/project_3/figure/fig_size_sweep.png',
    '/Users/mac/Documents/GitHub/CPP/project_3/report_latex/fig_size_sweep.png',
]:
    plt.savefig(fmt_path, dpi=200, bbox_inches='tight')
print("Saved fig_size_sweep.png")
