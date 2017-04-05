Avery Wong
904 582 269
cs111 winter 2017
lab2a

Since I dont know if my makefile will generate the graphs from the csv files I have included
the graphs and my own csv files just in case my Makefile fails to generate them. This is 
due to the fact that when the TA uses gnuplot I do not know where they have put their gnuplot
in my linux server I used the commands

~/gnuplot/bin/gnuplot lab2_add.gp
~/gnuplot/bin/gnuplot lab2_list.gp


Makefile options:

build:
compiles lab2_add.c and lab2_list.c

tests:
runs the cases and generate the results into csvfiles, lab2_list.csv and lab2_add.csv

graphs:
it runs clean to delete the csv files generated from tests then run the tests
to generate lab2_list.csv and lab2_add.csv to prevent overlay in data points
then runs 

gnuplot lab2_add.gp
gnuplot lab2_list.gp

make sure to have the right directory for gnuplot so you could run it
b/c the directory of your gnuplot might be different

tarball:

You have to run make tests then make graphs first to make 
the csv files and the graphs then run make tarball

files include:

lab2_add.c
SortedList.c
SortedList.h
lab2_list.c
lab2_add.csv
lab2_list.csv

lab2_add-1.png
lab2_add-2.png
lab2_add-3.png
lab2_add-4.png
lab2_add-5.png
lab2_list-1.png
lab2_list-2.png
lab2_list-3.png 
lab2_list-4.png

lab2_add.gp
lab2_list.gp

clean:

remove the executables lab2_add and lab2_list
removes the csv files
removes the graphs

2.1.1 causing conflicts:

Why does it take many iterations before errors are seen?

ans:
I ran the script with different iterations on the same thread
and it was true that the errors racked up with more iterations.
This is most possibly due to race conditions as each thread
is adding and subtracting, ordinarily with little number of 
threads, there are less data corruption but with more threads
there are more errors because when two threads access the counter
(critical section) and attempt to write to it, it causes and an
error.

Why does a significantly smaller number of iterations so seldom fail?

ans:
Well if you have two threads, although you dont know when which thread will
add first and subtract later, the net total additions and subtraction should be the same, however when two threads attempt to write to counter at the
same time, there is a collision in the critical section which 
will cause an error, and the error will continue to propogate.


2.1.2 cost of yielding

Why are the --yield runs so much slower?  Where is the additional time going? 
Is it possible to get valid per-operation timings if we are using the --yield 
option?  If so, explain how.  If not, explain why not.

ans:
yielding is when you stop a thread at the critical section and do a context
switch to another thread, which will obviously generate context switching 
overhead.

QUESTION 2.1.3 - measurement errors:

Why does the average cost per operation drop with increasing iterations?
If the cost per iteration is a function of the number of iterations, how do we
know how many iterations to run (or what the “correct” cost is)?

ans:
The average cost per operation drops with increasing iterations is because
we increase the number of threads and iterations but the overhead in proportion
to the number of iterations decreases thus we see a decrease of operation
per cost.

Looking at the graph as the iterations increases the cost converges asymtoptically
to a certain value,so we know how many iterations to run if we choose an iteration
that optimze the cost. Also, it is worth to know that the time I used to store
run time (ns) can overflow which would be a bound to how many iterations
I can increase to.

QUESTION 2.1.4 - costs of serialization:

Why do all of the options perform similarly for low numbers of threads?

ans:
Since seasnet have multiple cores or if a general computer have multiple cores
the thread will usualy run in parallel anyways and will not intefere with each other,
thus rendering yields and lock obsolete and the overhead time of yielding and locking
minimal.

Why do the three protected operations slow down as the number of threads rises?

ans:
As we increase the number of threads, the protection comes into play, and there 
will be more waiting time for one thread to pass their key to another or more
waiting time for waiting on another thread which increases the time it takes
to complete the operations.


Why are spin-locks so expensive for large numbers of threads?

spin-locks are expensive for large number of threads, because spin lock operate
by continually asking the cpu if the critical section is freed, so with more 
threads more threads will continue spinning which on its own adds usage to the 
cpu and thereby increases the cost

QUESTION 2.2.1 - scalability of Mutex

Compare the variation in time per protected operation vs the number of threads (for mutex-protected operations) in Part-1 and Part-2, commenting on similarities/differences and offering explanations for them.

Generally, the cost per operation increases as we increase the number of threads for both
add and list. Noticeable differences inclue:
lists generally have higher cost per operations to begin with, do the nature of inserting, and 
deleting which is O(N) whereas in add we are only add/subtracting O(1), these longer operations
means longer wait times near the critical sections

QUESTION 2.2.2 - scalability of spin locks

Compare the variation in time per protected operation vs the number of threads for Mutex vs Spin locks, commenting on similarities/differences and offering explanations for them.

ans:

For lab2-add:

I notice for small number of threads, spinlocking is faster than mutex, which can be due to
the fact that spinlocking has less overhead time, as in mutex a thread is put to sleep and
does a context switch to another thread. However with more threads, spinlocking begins performing
worse than mutexes which can be due to the fact that spinning more threads add to resource usage
which will slow down its performance.

for lab2_list

I notice that spinlocking does worse than mutexes as in the cost is higher for spinlocking,
according to my graph for both adjusted and raw. This could be due the fact that in lists
spinning is bad for lists because working with lists causes a large enough overhead in accessing
lists and modifying them outweighs the supposed benefit over mutexes. So basically, the 
overhead from spinlock is now worse than the blocking and context switching of mutexes.
