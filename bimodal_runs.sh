#!/bin/bash

# Create the binary
echo "making the binary"
make

echo "cleaning up object files"
make clean


# Run the validation runs
echo
echo

echo "============================================"
echo "PART A"
echo "============================================"

echo
echo
echo "============================================"
echo "starting validation runs"
echo "============================================"

#echo "Running the first validation run"
./sim bimodal 6 gcc_trace.txt > ./validation_runs/val_bimodal_1_antra.txt
diff -iw ./validation_runs/val_bimodal_1_antra.txt ./validation_runs/val_bimodal_1.txt > ./validation_runs/val_bimodal_1_diff.txt
rm -rf ./validation_runs/val_bimodal_1_antra.txt
#echo "Completed the first validation run"

#echo "Running the second validation run"
./sim bimodal 12 gcc_trace.txt > ./validation_runs/val_bimodal_2_antra.txt
diff -iw ./validation_runs/val_bimodal_2_antra.txt ./validation_runs/val_bimodal_2.txt > ./validation_runs/val_bimodal_2_diff.txt
rm -rf ./validation_runs/val_bimodal_2_antra.txt
#echo "Completed the second validation run"

#echo "Running the third validation run"
./sim bimodal 4 jpeg_trace.txt > ./validation_runs/val_bimodal_3_antra.txt
diff -iw ./validation_runs/val_bimodal_3_antra.txt ./validation_runs/val_bimodal_3.txt > ./validation_runs/val_bimodal_3_diff.txt
rm -rf ./validation_runs/val_bimodal_3_antra.txt
#echo "Completed the third validation run"

#echo "Running the fourth validation run"
./sim bimodal 5 perl_trace.txt > ./validation_runs/val_bimodal_4_antra.txt
diff -iw ./validation_runs/val_bimodal_4_antra.txt ./validation_runs/val_bimodal_4.txt > ./validation_runs/val_bimodal_4_diff.txt
rm -rf ./validation_runs/val_bimodal_4_antra.txt
#echo "Completed the fourth validation run"
#

echo "Check results in the validation folder with files named val_bimodal_*_diff.txt"

echo
echo

echo "============================================"
echo "PART B"
echo "============================================"

echo
echo
echo "============================================"
echo "Running Simulator for bimodal predictor"
echo "configurations for 7 ≤set grid x y2 m ≤ 12."
echo "============================================"

for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=12 ; m++)) ; do
	rm -rf bimodal-m-$m-$trace_file
    done;
    rm -rf bimodal_misprediction_$trace_file
done;

rm -rf plot.plt


for trace in gcc jpeg perl ; do
    for ((m=7 ; m<=12 ; m++)) ; do
	trace_file=$trace"_trace.txt"
	./sim bimodal $m $trace_file > bimodal-m-$m-$trace_file
	mispred_rate=`cat bimodal-m-$m-$trace_file | grep "misprediction rate" | awk '{print $3}' | cut -d '%' -f 1`
	echo "$m $mispred_rate $mispred_rate" >> bimodal_misprediction_$trace_file
    done;
    echo "set term png size 1024,768" >> plot.plt
    echo "set output \"$trace-bimodal.png\"" >> plot.plt
    echo "set autoscale" >> plot.plt
    echo "set grid x y" >> plot.plt
    echo "unset log" >> plot.plt
    echo "unset label" >> plot.plt
    echo "set xtic auto" >> plot.plt
    echo "set ytic auto" >> plot.plt
    echo "set title \"$trace, bimodal\"" >> plot.plt
    echo "set xlabel \"m\"" >> plot.plt
    echo "set ylabel \"branch misprediction rate\"" >> plot.plt
    echo "set autoscale x" >> plot.plt
    echo "set autoscale y" >> plot.plt
#    echo "set xtics" >> plot.plt
#    echo "set ytics" >> plot.plt
    echo "plot \"bimodal_misprediction_$trace_file\" using 1:2 title 'misprediction rate' with linespoints lw 3 ps 2, '' with labels center offset 3.4,.5 notitle" >> plot.plt

    echo "============================================"
    echo "Plotting the graph for bimodal simulator"
    echo "============================================"
    gnuplot plot.plt
done;


for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=12 ; m++)) ; do
	rm -rf bimodal-m-$m-$trace_file
    done;
    rm -rf bimodal_misprediction_$trace_file
done;

rm -rf plot.plt


echo
echo

echo "============================================"
echo "PART C"
echo "============================================"

echo
echo

echo "============================================"
echo "Running Simulator for bimodal predictor"
echo "for design with 7 ≤ m ≤ 15."
echo "============================================"

for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=15 ; m++)) ; do
	rm -rf bimodal-m-$m-$trace_file
    done;
    rm -rf bimodal_misprediction_$trace_file
done;


for trace in gcc jpeg perl ; do
    rm -rf plot.plt
    for ((m=7 ; m<=15 ; m++)) ; do
	trace_file=$trace"_trace.txt"
	./sim bimodal $m $trace_file > bimodal-m-$m-$trace_file
	mispred_rate=`cat bimodal-m-$m-$trace_file | grep "misprediction rate" | awk '{print $3}' | cut -d '%' -f 1`
	echo "$m $mispred_rate $(($((3*$((2**$m))))/$((2**3))))" >> bimodal_misprediction_$trace_file
    done;
    echo "set term png size 1024,768" >> plot.plt
    echo "set output \"$trace-bimodal-design.png\"" >> plot.plt
    echo "set autoscale" >> plot.plt
    echo "set grid x y" >> plot.plt
    echo "unset log" >> plot.plt
    echo "unset label" >> plot.plt
    echo "set xtic auto" >> plot.plt
    echo "set ytic auto" >> plot.plt
    echo "set title \"$trace, bimodal\"" >> plot.plt
    echo "set xlabel \"m\"" >> plot.plt
    echo "set ylabel \"branch misprediction rate\"" >> plot.plt
    echo "set y2label \"Predictor table size (Bytes)\"" >> plot.plt
    echo "set autoscale x" >> plot.plt
    echo "set autoscale y" >> plot.plt
    echo "set autoscale y2" >> plot.plt
    echo "set y2tics" >> plot.plt
    echo "plot \"bimodal_misprediction_$trace_file\" using 1:2 title 'misprediction rate' with linespoints lw 3 ps 2 axes x1y1, \"bimodal_misprediction_$trace_file\" using 1:3 title 'Predictor table size (Bytes)' with linespoints lw 3 ps 2 axes x1y2" >> plot.plt

    echo "============================================"
    echo "Plotting the graph for bimodal simulator"
    echo "============================================"
    gnuplot plot.plt
done;


for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=15 ; m++)) ; do
	rm -rf bimodal-m-$m-$trace_file
    done;
    rm -rf bimodal_misprediction_$trace_file
done;

rm -rf plot.plt
