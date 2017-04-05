#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

set title "throughput vs number of threads for mutex and spin-lock synchronized adds and list operations"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "throughput"
set logscale y 10
set output 'lab2b_1.png'
set key left top
plot \
     "< grep list-none-m lab2_list.csv" using ($2):(1000000000/($7)) \
     	title 'mutex list' with linespoints lc rgb 'green', \
     "< grep list-none-s lab2_list.csv" using ($2):(1000000000/($7)) \
     	title 'spin list' with linespoints lc rgb 'blue', \
     "< grep add-m lab2_add.csv" using ($2):(1000000000/($6)) \
     	title 'add mutex' with linespoints lc rgb 'red', \
     "< grep add-s lab2_add.csv" using ($2):(1000000000/($6)) \
     	title 'add spin' with linespoints lc rgb 'orange'

