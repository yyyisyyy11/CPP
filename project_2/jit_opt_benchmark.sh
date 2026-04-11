#!/bin/bash
# JIT Optimization Benchmark: runs Dotproduct under 6 JVM configs, 3 trials each
# Extracts n=100000000 results only

JAVA_FILE="Dotproduct"
N_TARGET="100000000"
TRIALS=3
OUTDIR="jit_opt_results"

mkdir -p "$OUTDIR"

# Define configurations
declare -a CONFIG_NAMES=("interpreter" "c1_only" "c2_default" "c2_no_simd" "c2_no_unroll" "c2_no_looppred")
declare -a CONFIG_FLAGS=(
    "-Xint"
    "-XX:TieredStopAtLevel=3"
    ""
    "-XX:-UseSuperWord"
    "-XX:LoopUnrollLimit=0"
    "-XX:-UseLoopPredicate"
)

echo "========================================="
echo " JIT Optimization Benchmark"
echo " Target: n=$N_TARGET, Trials: $TRIALS"
echo "========================================="

for idx in "${!CONFIG_NAMES[@]}"; do
    name="${CONFIG_NAMES[$idx]}"
    flags="${CONFIG_FLAGS[$idx]}"
    echo ""
    echo "--- Config: $name ($flags) ---"
    
    for trial in $(seq 1 $TRIALS); do
        echo "  Trial $trial/$TRIALS ..."
        outfile="$OUTDIR/${name}_trial${trial}.csv"
        
        # Run and capture only CSV output (filter out stderr progress messages)
        java $flags "$JAVA_FILE" 2>/dev/null | grep "$N_TARGET" > "$outfile"
        
        echo "    Done. Results:"
        cat "$outfile" | while read line; do
            echo "      $line"
        done
    done
done

echo ""
echo "========================================="
echo " Collecting results..."
echo "========================================="

# Produce summary: for each config and type, show median of 3 trials
echo ""
echo "Config,Type,Trial1,Trial2,Trial3,Median"

for idx in "${!CONFIG_NAMES[@]}"; do
    name="${CONFIG_NAMES[$idx]}"
    for dtype in float double int short byte; do
        vals=()
        for trial in $(seq 1 $TRIALS); do
            val=$(grep "^${dtype}," "$OUTDIR/${name}_trial${trial}.csv" | cut -d',' -f3)
            vals+=("$val")
        done
        # Sort and take median (middle of 3)
        sorted=($(echo "${vals[@]}" | tr ' ' '\n' | sort -n))
        median="${sorted[1]}"
        echo "$name,$dtype,${vals[0]},${vals[1]},${vals[2]},$median"
    done
done

echo ""
echo "Done! Raw data in $OUTDIR/"
