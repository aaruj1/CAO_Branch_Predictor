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
echo "starting gshare validation runs"
echo "============================================"

#echo "Running the first validation run"
./sim gshare 9 3 gcc_trace.txt > ./validation_runs/val_gshare_1_antra.txt
diff -iw ./validation_runs/val_gshare_1_antra.txt ./validation_runs/val_gshare_1.txt > ./validation_runs/val_gshare_1_diff.txt
rm -rf ./validation_runs/val_gshare_1_antra.txt
#echo "Completed the first validation run"

#echo "Running the second validation run"
./sim gshare 14 8 gcc_trace.txt > ./validation_runs/val_gshare_2_antra.txt
diff -iw ./validation_runs/val_gshare_2_antra.txt ./validation_runs/val_gshare_2.txt > ./validation_runs/val_gshare_2_diff.txt
rm -rf ./validation_runs/val_gshare_2_antra.txt
#echo "Completed the second validation run"

#echo "Running the third validation run"
./sim gshare 11 5 jpeg_trace.txt > ./validation_runs/val_gshare_3_antra.txt
diff -iw ./validation_runs/val_gshare_3_antra.txt ./validation_runs/val_gshare_3.txt > ./validation_runs/val_gshare_3_diff.txt
rm -rf ./validation_runs/val_gshare_3_antra.txt
#echo "Completed the third validation run"

#echo "Running the fourth validation run"
./sim gshare 10 6 perl_trace.txt > ./validation_runs/val_gshare_4_antra.txt
diff -iw ./validation_runs/val_gshare_4_antra.txt ./validation_runs/val_gshare_4.txt > ./validation_runs/val_gshare_4_diff.txt
rm -rf ./validation_runs/val_gshare_4_antra.txt
#echo "Completed the fourth validation run"
#

echo "Check results in the validation folder with files named val_gshare_*_diff.txt"

echo
echo

echo "============================================"
echo "PART B"
echo "============================================"

echo
echo

echo "============================================"
echo "Running Simulator for gshare predictor"
echo "configurations for 7 ≤ m ≤ 12 and 2 ≤ n ≤ m"
echo "============================================"

for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=12 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    rm -rf gshare-m-$m-n-$n-$trace_file
	    rm -rf gshare_misprediction_$n-$trace_file
	done;
    done;
done;

gnuplot_script="gshare_plot.plt"
rm -rf $gnuplot_script


for trace in gcc jpeg perl; do
    rm -rf $gnuplot_script
    echo "set term png size 1024,768" >> $gnuplot_script
    echo "set output \"$trace-gshare.png\"" >> $gnuplot_script
    echo "set autoscale" >> $gnuplot_script
    echo "set grid x y" >> $gnuplot_script
    echo "unset log" >> $gnuplot_script
    echo "unset label" >> $gnuplot_script
    echo "set xtic auto" >> $gnuplot_script
    echo "set ytic auto" >> $gnuplot_script
    echo "set title \"$trace, gshare\"" >> $gnuplot_script
    echo "set xlabel \"m\"" >> $gnuplot_script
    echo "set ylabel \"branch misprediction rate\"" >> $gnuplot_script
    echo "set autoscale x" >> $gnuplot_script
    echo "set autoscale y" >> $gnuplot_script
    echo "plot \\" >> $gnuplot_script
    
    for ((m=7 ; m<=12 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    trace_file=$trace"_trace.txt"
	    ./sim gshare $m $n $trace_file > gshare-m-$m-n-$n-$trace_file
	    mispred_rate=`cat gshare-m-$m-n-$n-$trace_file | grep "misprediction rate" | awk '{print $3}' | cut -d '%' -f 1`
	    echo "$m $mispred_rate $mispred_rate" >> gshare_misprediction_$n-$trace_file
	done;
    done;

    for ((n=2; n <= 10; n+=2)); do
	echo "\"gshare_misprediction_$n-$trace_file\" using 1:2 title 'misprediction rate for n = $n' with linespoints lw 3 ps 2, '' with labels center offset 3.4,.5 notitle, \\" >> $gnuplot_script
    done ;
    n=12
    echo "\"gshare_misprediction_$n-$trace_file\" using 1:2 title 'misprediction rate for n = $n' with linespoints lw 3 ps 2, '' with labels center offset 3.4,.5 notitle" >> $gnuplot_script
    gnuplot $gnuplot_script
done;

rm -rf $gnuplot_script
for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=12 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    rm -rf gshare-m-$m-n-$n-$trace_file
	    rm -rf gshare_misprediction_$n-$trace_file
	done;
    done;
done;

echo
echo

echo "============================================"
echo "PART C"
echo "============================================"


echo "============================================"
echo "Running Simulator for gshare predictor"
echo "design for 7 ≤ m ≤ 14 and 2 ≤ n ≤ m"
echo "============================================"

for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=14 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    rm -rf gshare-m-$m-n-$n-$trace_file
	    rm -rf gshare_misprediction_$n-$trace_file
	done;
    done;
done;

gnuplot_script="gshare_plot.plt"
rm -rf $gnuplot_script


for trace in gcc jpeg perl; do
    rm -rf $gnuplot_script
    echo "set term png size 1024,768" >> $gnuplot_script
    echo "set output \"$trace-gshare-design.png\"" >> $gnuplot_script
    echo "set autoscale" >> $gnuplot_script
    echo "set grid x y" >> $gnuplot_script
    echo "unset log" >> $gnuplot_script
    echo "unset label" >> $gnuplot_script
    echo "set xtic auto" >> $gnuplot_script
    echo "set ytic auto" >> $gnuplot_script
    echo "set y2tic auto" >> $gnuplot_script
    echo "set title \"$trace, gshare\"" >> $gnuplot_script
    echo "set xlabel \"m\"" >> $gnuplot_script
    echo "set ylabel \"branch misprediction rate\"" >> $gnuplot_script
    echo "set y2label \"Predictor table size (Bytes)\"" >> $gnuplot_script
    echo "set autoscale x" >> $gnuplot_script
    echo "set autoscale y" >> $gnuplot_script
    echo "set autoscale y2" >> $gnuplot_script
    echo "plot \\" >> $gnuplot_script
    
    for ((m=7 ; m<=14 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    trace_file=$trace"_trace.txt"
	    ./sim gshare $m $n $trace_file > gshare-m-$m-n-$n-$trace_file
	    mispred_rate=`cat gshare-m-$m-n-$n-$trace_file | grep "misprediction rate" | awk '{print $3}' | cut -d '%' -f 1`
	    echo "$m $mispred_rate $(($((3*$((2**$m))))/$((2**3))))" >> gshare_misprediction_$n-$trace_file
	done;
    done;

    for ((n=2; n <= 12; n+=2)); do
	echo "\"gshare_misprediction_$n-$trace_file\" using 1:2 title 'misprediction rate for n = $n' with linespoints lw 3 ps 2 axes x1y1, \\" >> $gnuplot_script
    done ;
    n=14
    echo "\"gshare_misprediction_$n-$trace_file\" using 1:2 title 'misprediction rate for n = $n' with linespoints lw 3 ps 2 axes x1y1, \"gshare_misprediction_2-$trace_file\" using 1:3 title 'Predictor table size (Bytes)' with linespoints lw 3 ps 2 axes x1y2" >> $gnuplot_script
    gnuplot $gnuplot_script
done;

rm -rf $gnuplot_script
for trace in gcc jpeg perl; do
    trace_file=$trace"_trace.txt"
    for ((m=7 ; m<=14 ; m++)) ; do
	for ((n = 2; n <=m; n+=2)); do
	    rm -rf gshare-m-$m-n-$n-$trace_file
	    rm -rf gshare_misprediction_$n-$trace_file
	done;
    done;
done;




