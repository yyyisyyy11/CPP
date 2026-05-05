#!/usr/bin/env python3
"""Generate performance visualization charts for matmul optimization report."""

import matplotlib.pyplot as plt
import matplotlib
import numpy as np

matplotlib.rcParams['font.family'] = ['Arial', 'Helvetica', 'sans-serif']
matplotlib.rcParams['font.size'] = 11

# ── Data ──────────────────────────────────────────────────────────────
phases = [
    'Plain\n(i-j-k)',
    'Loop\nReorder\n+Tiling',
    'B\nPacking',
    'NEON\nSIMD',
    'Micro\nKernel\n8×16',
    'A+B\nPacking',
    'OpenMP\n10T',
    'BLIS\nTiling',
]

gflops_128  = [1.4,  13.1, 11.0, 10.6, 22.2, 23.3, 25.7,  25.7]
gflops_1024 = [2.0,  22.4, 25.8, 29.3, 59.5, 59.7, 267.8, 316.6]
gflops_8192 = [None, 16.3, 27.2, 28.7, 47.8, 47.6, 309.0, 385.5]

openblas_1024 = 1243.5
openblas_8192 = 514.3

# ── Figure 1: Performance progression ─────────────────────────────────
fig1, ax1 = plt.subplots(figsize=(12, 6))

x = np.arange(len(phases))

# Plot 1024 and 8192 lines
ax1.plot(x, gflops_1024, 'o-', color='#2563eb', linewidth=2.5,
         markersize=8, label='1024×1024', zorder=5)

# 8192 has None for plain, plot from index 1
x_8192 = [i for i, v in enumerate(gflops_8192) if v is not None]
y_8192 = [v for v in gflops_8192 if v is not None]
ax1.plot(x_8192, y_8192, 's-', color='#dc2626', linewidth=2.5,
         markersize=8, label='8192×8192', zorder=5)

# Add value labels
for i, v in enumerate(gflops_1024):
    offset = 12 if v < 200 else -18
    ax1.annotate(f'{v:.1f}', (i, v), textcoords='offset points',
                 xytext=(0, offset), ha='center', fontsize=8.5,
                 color='#2563eb', fontweight='bold')

for i, v in zip(x_8192, y_8192):
    offset = -18 if v > 100 else 12
    ax1.annotate(f'{v:.1f}', (i, v), textcoords='offset points',
                 xytext=(0, offset), ha='center', fontsize=8.5,
                 color='#dc2626', fontweight='bold')

# OpenBLAS reference lines
ax1.axhline(y=openblas_8192, color='#dc2626', linestyle='--', alpha=0.4, linewidth=1.5)
ax1.text(len(phases)-0.5, openblas_8192+10, f'OpenBLAS 8K={openblas_8192:.0f}',
         color='#dc2626', alpha=0.6, fontsize=9, ha='right')

# Shading for phases
ax1.axvspan(5.5, 7.5, alpha=0.08, color='green', label='Multi-thread region')
ax1.axvspan(-0.5, 0.5, alpha=0.08, color='gray')

ax1.set_xticks(x)
ax1.set_xticklabels(phases, fontsize=9)
ax1.set_ylabel('GFLOPS', fontsize=13, fontweight='bold')
ax1.set_title('Matrix Multiplication Optimization Progress on Apple M5',
              fontsize=14, fontweight='bold', pad=15)
ax1.legend(loc='upper left', fontsize=11, framealpha=0.9)
ax1.set_ylim(bottom=0, top=max(max(gflops_1024), max(y_8192)) * 1.25)
ax1.grid(axis='y', alpha=0.3)
ax1.set_xlim(-0.5, len(phases) - 0.5)

plt.tight_layout()
fig1.savefig('/Users/mac/Documents/GitHub/CPP/project_3/fig_optimization_progress.png',
             dpi=200, bbox_inches='tight')
print("Saved: fig_optimization_progress.png")

# ── Figure 2: Final comparison bar chart ──────────────────────────────
fig2, ax2 = plt.subplots(figsize=(9, 5.5))

sizes = ['16×16', '128×128', '1024×1024', '8192×8192']
improved = [0.03, 25.7, 316.6, 385.5]
openblas = [1.17, 279.6, 1243.5, 514.3]

x2 = np.arange(len(sizes))
width = 0.35

bars1 = ax2.bar(x2 - width/2, improved, width, label='Our Implementation (10T)',
                color='#2563eb', edgecolor='white', linewidth=0.5, zorder=3)
bars2 = ax2.bar(x2 + width/2, openblas, width, label='OpenBLAS (10T)',
                color='#f97316', edgecolor='white', linewidth=0.5, zorder=3)

# Value labels
for bar in bars1:
    h = bar.get_height()
    if h > 1:
        ax2.text(bar.get_x() + bar.get_width()/2, h + 15,
                 f'{h:.1f}', ha='center', va='bottom', fontsize=9,
                 fontweight='bold', color='#2563eb')

for bar in bars2:
    h = bar.get_height()
    if h > 1:
        ax2.text(bar.get_x() + bar.get_width()/2, h + 15,
                 f'{h:.1f}', ha='center', va='bottom', fontsize=9,
                 fontweight='bold', color='#f97316')

# Percentage labels
for i in range(len(sizes)):
    if improved[i] > 1 and openblas[i] > 1:
        pct = improved[i] / openblas[i] * 100
        mid_x = x2[i]
        mid_y = max(improved[i], openblas[i]) + 60
        ax2.text(mid_x, mid_y, f'{pct:.0f}%', ha='center', fontsize=10,
                 color='#059669', fontweight='bold',
                 bbox=dict(boxstyle='round,pad=0.3', facecolor='#ecfdf5',
                           edgecolor='#059669', alpha=0.8))

ax2.set_xticks(x2)
ax2.set_xticklabels(sizes, fontsize=11)
ax2.set_ylabel('GFLOPS', fontsize=13, fontweight='bold')
ax2.set_title('Our Implementation vs OpenBLAS (10 threads, Apple M5)',
              fontsize=14, fontweight='bold', pad=15)
ax2.legend(fontsize=11, loc='upper left', framealpha=0.9)
ax2.set_ylim(0, max(max(improved), max(openblas)) * 1.2)
ax2.grid(axis='y', alpha=0.3, zorder=0)

plt.tight_layout()
fig2.savefig('/Users/mac/Documents/GitHub/CPP/project_3/fig_vs_openblas.png',
             dpi=200, bbox_inches='tight')
print("Saved: fig_vs_openblas.png")

# ── Figure 3: Speedup waterfall ───────────────────────────────────────
fig3, ax3 = plt.subplots(figsize=(10, 5))

short_phases = ['Plain', 'Reorder\n+Tile', 'B Pack', 'NEON', 'μKernel', 'A+B Pack', 'OMP 10T', 'BLIS']
vals = gflops_1024
colors = ['#94a3b8', '#60a5fa', '#60a5fa', '#818cf8', '#818cf8', '#818cf8', '#f97316', '#ef4444']

bars3 = ax3.bar(range(len(vals)), vals, color=colors, edgecolor='white',
                linewidth=0.8, zorder=3)

for i, (bar, v) in enumerate(zip(bars3, vals)):
    ax3.text(bar.get_x() + bar.get_width()/2, v + 5,
             f'{v:.1f}', ha='center', fontsize=9, fontweight='bold')

# Speedup annotations
for i in range(1, len(vals)):
    if vals[i] > vals[i-1] * 1.05:
        speedup = vals[i] / vals[i-1]
        ax3.annotate(f'×{speedup:.1f}', xy=(i, vals[i]),
                     xytext=(i-0.5, vals[i] * 0.7),
                     fontsize=8, color='#059669', fontweight='bold',
                     arrowprops=dict(arrowstyle='->', color='#059669', lw=1.2))

ax3.axhline(y=openblas_1024, color='#f97316', linestyle='--', alpha=0.5, linewidth=1.5)
ax3.text(len(vals)-0.3, openblas_1024-40, f'OpenBLAS = {openblas_1024:.0f}',
         color='#f97316', alpha=0.7, fontsize=9, ha='right')

ax3.set_xticks(range(len(short_phases)))
ax3.set_xticklabels(short_phases, fontsize=9)
ax3.set_ylabel('GFLOPS', fontsize=13, fontweight='bold')
ax3.set_title('Optimization Waterfall — 1024×1024 Matrix', fontsize=14, fontweight='bold', pad=15)
ax3.set_ylim(0, openblas_1024 * 1.15)
ax3.grid(axis='y', alpha=0.3, zorder=0)

plt.tight_layout()
fig3.savefig('/Users/mac/Documents/GitHub/CPP/project_3/fig_waterfall_1024.png',
             dpi=200, bbox_inches='tight')
print("Saved: fig_waterfall_1024.png")

# ── Figure 4: Speedup waterfall 8192 ──────────────────────────────────
fig4, ax4 = plt.subplots(figsize=(10, 5))

short_phases_8k = ['Reorder\n+Tile', 'B Pack', 'NEON', 'μKernel', 'A+B Pack', 'OMP 10T', 'BLIS']
vals_8k = [16.3, 27.2, 28.7, 47.8, 47.6, 309.0, 385.5]
colors_8k = ['#60a5fa', '#60a5fa', '#818cf8', '#818cf8', '#818cf8', '#f97316', '#ef4444']

bars4 = ax4.bar(range(len(vals_8k)), vals_8k, color=colors_8k, edgecolor='white',
                linewidth=0.8, zorder=3)

for i, (bar, v) in enumerate(zip(bars4, vals_8k)):
    ax4.text(bar.get_x() + bar.get_width()/2, v + 5,
             f'{v:.1f}', ha='center', fontsize=9, fontweight='bold')

# Speedup annotations
for i in range(1, len(vals_8k)):
    if vals_8k[i] > vals_8k[i-1] * 1.05:
        speedup = vals_8k[i] / vals_8k[i-1]
        ax4.annotate(f'×{speedup:.1f}', xy=(i, vals_8k[i]),
                     xytext=(i-0.5, vals_8k[i] * 0.7),
                     fontsize=8, color='#059669', fontweight='bold',
                     arrowprops=dict(arrowstyle='->', color='#059669', lw=1.2))

ax4.axhline(y=openblas_8192, color='#f97316', linestyle='--', alpha=0.5, linewidth=1.5)
ax4.text(len(vals_8k)-0.3, openblas_8192+10, f'OpenBLAS = {openblas_8192:.0f}',
         color='#f97316', alpha=0.7, fontsize=9, ha='right')

ax4.set_xticks(range(len(short_phases_8k)))
ax4.set_xticklabels(short_phases_8k, fontsize=9)
ax4.set_ylabel('GFLOPS', fontsize=13, fontweight='bold')
ax4.set_title('Optimization Waterfall — 8192×8192 Matrix', fontsize=14, fontweight='bold', pad=15)
ax4.set_ylim(0, openblas_8192 * 1.25)
ax4.grid(axis='y', alpha=0.3, zorder=0)

plt.tight_layout()
fig4.savefig('/Users/mac/Documents/GitHub/CPP/project_3/fig_waterfall_8192.png',
             dpi=200, bbox_inches='tight')
print("Saved: fig_waterfall_8192.png")

print("\nAll 4 figures generated successfully!")
