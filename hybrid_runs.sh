#!/bin/bash

# Create the binary
echo "making the binary"
make

echo "cleaning up object files"
make clean

# Run the validation runs
echo "============================================"
echo "starting hybrid validation runs"
echo "============================================"

echo "Running the first validation run"
./sim hybrid 8 14 10 5 gcc_trace.txt > ./validation_runs/val_hybrid_1_antra.txt
diff -iw ./validation_runs/val_hybrid_1_antra.txt ./validation_runs/val_hybrid_1.txt > ./validation_runs/val_hybrid_1_diff.txt
rm -rf ./validation_runs/val_hybrid_1_antra.txt
echo "Completed the first validation run"

echo "Running the second validation run"
./sim hybrid 5 10 7 5 jpeg_trace.txt > ./validation_runs/val_hybrid_2_antra.txt
diff -iw ./validation_runs/val_hybrid_2_antra.txt ./validation_runs/val_hybrid_2.txt > ./validation_runs/val_hybrid_2_diff.txt
rm -rf ./validation_runs/val_hybrid_2_antra.txt
echo "Completed the second validation run"


