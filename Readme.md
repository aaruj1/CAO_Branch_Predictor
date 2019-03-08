# Implementation of Branch predictor simulator well suited to the SPECint95 benchmarks
----------------------------------------------------------------------------------------------------------------------------------------------
## Introduction
This is the implementation of bimodal, gshare and hybrid branch predictor.  

-----------------------------------------------------------------------
## Overview:

Model a gshare branch predictor with parameters {m, n}, where:
m is the number of low‐order PC bits used to form the prediction table index. Note: discard the lowest two bits of the PC, since these are always zero, i.e., use bits m+1 through 2 of the PC.
n is the number of bits in the global branch history register. Note: n ≤ m. Note: n may equal zero, in which case we have the simple bimodal branch predictor.


## Programming language of implementation : C

## Steps to Compile and Run the program

### 1. Compile the code using Makefile which is already present in the folder.

    $> make

### 2. To clean the binary file

    $> make clean

#### To simulate a bimodal predictor:

    $./sim bimodal <M> <tracefile>, where 
    M is the number of PC bits used to index the bimodal table

#### To simulate a gshare predictor:

    $./sim gshare <M> <N> <tracefile>, where 
    M = number of PC bits used to index the gshare table
    N = global branch history register bits used to index the gshare table

#### To simulate a hybrid predictor:

    $./sim hybrid <K> <M1> <N> <M2> <tracefile>, where 
    K = number of PC bits used to index the chooser table, 
    M1 = number of PC bits used to index the gshare table
    N = global branch history register bits used to index the gshare table 
    M2 = number of PC bits used to index the bimodal table.


## I have written a bash script to perform validation, plot graphs, and conduct a design analysis. It can be run by using command below:
The script is devide in four parts:

Part 1:

    make :  make the binary
Part 2:

    Runs the validation cases and generates output of prediction table in the validation directory and does "diff -iw <my_output> <validation_file> > diff output"
    Check the diff output to see the results.

Part 3:
    
    Run the cases specified in the the project and create the respective graphs. It uses "gnuplot" software to generate the graphs.
    
Part4:

    Runs through the specified design parameters (i.e. for bimodal runs through this configuration 7 ≤ m ≤ 12 and plots the misprediction and prediction table memory usage vs m.)
    This will help us identify the predictor which will give the minimum misprediction and minimum memory usage.
    

 
    


