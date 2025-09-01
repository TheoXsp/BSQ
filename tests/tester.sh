#!/bin/bash

BINARY="$1"
MAPS_DIR="mouli_maps"
SOLVED_DIR="mouli_maps_solved"
OUT_DIR="tests/out"
LARGE_MAP="$MAPS_DIR/intermediate_map_10000_10000"

if [ -z "$BINARY" ]; then
    echo "Error: Please provide the binary path as argument."
    exit 1
fi

if [ ! -x "$BINARY" ]; then
    echo "Error: Binary '$BINARY' not found or not executable."
    exit 1
fi

mkdir -p "$OUT_DIR"

passed=0
failed=0
total=0

for map in "$MAPS_DIR"/*; do
    if [ -f "$map" ]; then
        basename=$(basename "$map")
        out_file="$OUT_DIR/${basename}_out"
        solved_file="$SOLVED_DIR/$basename"

        if [ ! -f "$solved_file" ]; then
            echo "Warning: No solved file for $basename, skipping."
            continue
        fi

        "$BINARY" "$map" > "$out_file"

        if diff -q "$out_file" "$solved_file" > /dev/null; then
            echo "Test $basename: PASSED"
            ((passed++))
        else
            echo "Test $basename: FAILED"
            ((failed++))
        fi
        ((total++))
    fi
done

echo "Summary: $passed/$total passed, $failed failed."

# Final time test
echo "Running time test on large map..."
TIME_OUTPUT=$( TIMEFORMAT=%R; { time "$BINARY" "$LARGE_MAP" >/dev/null 2>&1; } 2>&1 )
TIME_TAKEN=$(echo "$TIME_OUTPUT" | tail -1)

if [ -z "$TIME_TAKEN" ]; then
    echo "Time test: ERROR (failed to measure time)"
else
    if [ $(echo "$TIME_TAKEN < 0.18" | bc) -eq 1 ]; then
        echo "Time test: PASSED (took ${TIME_TAKEN}s < 0.18s)"
    else
        echo "Time test: FAILED (took ${TIME_TAKEN}s >= 0.18s)"
    fi
fi
