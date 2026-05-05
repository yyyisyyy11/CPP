#!/bin/bash
# ── 64K Benchmark Setup Script ──
# Run this on a fresh ARM cloud server (Ubuntu/Debian)
# Usage: bash setup_and_run.sh

set -e

echo "=== Step 1: Install dependencies ==="
sudo apt-get update
sudo apt-get install -y gcc libomp-dev libopenblas-dev

echo ""
echo "=== Step 2: Check system ==="
echo "RAM: $(free -h | awk '/Mem:/{print $2}')"
echo "CPU: $(nproc) cores"
echo "Arch: $(uname -m)"

echo ""
echo "=== Step 3: Create 16GB swap (prevent OOM) ==="
if [ ! -f /swapfile ]; then
    sudo fallocate -l 16G /swapfile
    sudo chmod 600 /swapfile
    sudo mkswap /swapfile
    sudo swapon /swapfile
    echo "Swap enabled: 16 GB"
else
    sudo swapon /swapfile 2>/dev/null || true
    echo "Swap already exists"
fi
free -h

echo ""
echo "=== Step 4: Compile ==="
gcc -O3 -fopenmp \
    -o test_64k \
    test_64k.c matrix.c matmul_plain.c matmul_improved.c \
    -lopenblas -lm

echo "Compiled OK."

echo ""
echo "=== Step 5: Run 64K benchmark ==="
echo "Using $(nproc) threads for improved, 8 threads for OpenBLAS (save memory)..."
export OMP_NUM_THREADS=$(nproc)
export OPENBLAS_NUM_THREADS=8

# Run with nohup so SSH disconnect won't kill it
nohup ./test_64k > result_64k.txt 2>&1 &
PID=$!
echo "Running in background (PID=$PID)..."
echo "Monitor with: tail -f ~/code/result_64k.txt"
echo "Check if done: ps -p $PID"

# Wait and show output
wait $PID
echo ""
echo "=== Result ==="
cat result_64k.txt
echo ""
echo "=== Done! Result saved to ~/code/result_64k.txt ==="
