Avery Wong
904 582 269
cs111 winter 2017
lab2b



make options:

make build: builds lab2_list

make clean: removes all the graphs, executables, lab_2b_list.csv, and profile.gperf

make tarball: puts all the graphs, source files, plotting script, csv files, and profile.gperf

I include the lab2_add.csv since I needed it for plotting lab2b_1.png

make profile: generates profile.gperf

note in my script I has 



more notes:

I forgot to explained why I chose to put pthread_yield in certain areas
of sortedlist in lab2a so here is why I chose where I put yield.
(these are also notes for myself)

So for inserting a node I want to wait for all the threads to find where they
want to want to place their node, otherwise when other threads insert their
node when another thread is iterating through it, the the thread my loose
track of the next node and returns an error, also I've thought of 
the corner case where after all the threads finished yielding before inserting,
on the chance that one thread starts deleting a node that happens to have 
the same value as a current node that another thread is pointing to, then that
would be impossible because the thread that is deleting would have to have
inserted another node with the same key before it deletes it, and
 since the list is sorted, then it would always delete the one it inserted 
before the one that another thread is currently pointing to because we 
are inserting before the current node we hold out pointing to.

For Deletion, the obvious choice to put the yield is before the actual deletion
from the list, so after each thread yields, the first thread that yield
 would have a dedicated amount of cpu slice to delete the node before it gives
 up the cpu to another thread. Without yield then multiple threads could
be modifying the same node's next and prev and cause the list to break

for lookup, I put the yield at the end of every iteration in the for loop,
because I want to make sure when one of the threads finds the nodes it is 
looking for and then deletes that node in the next function call in my 
thread function then when the other threads iterate to the next node, 
it wont point to the one that has been deleted

for length, I also put the yield at the end of every iteration iside
the for loop because I want to wait for all the other threads to wait for the
other threads to finish insertion before iterating, but I did not put 
the yield before the for loop because if another threads starts the process 
of deletion and deletes a node then that thread that is still counting
will be poining to a node that is currently being deleted, then there
will be an error

NOTE ABOUT DYNAMIC ALLOCATION

so I made a dynamic array of spin locks and mutex locks and I noticed there
was a difference when I made a preallocated array of fixed sized of locks with
the dynamic array. When I used a dynamic array of locks, I notice that the
 cost per operation for mutex locking is better than spin locking and the
 throughput for mutex locking is higher than spin locking, BUT
 WHEN I USE A DYNAMIC ARRAY OF LOCKS, mutex locking is way more costly
 than spin locking and thus spinlocking actually has higher throughput than
 mutex locking, so I just wanted to note these differences in the graphs
 since I used global static variables for my mutex and spin locks while
 I had to use a dynamic array in lab2b since there are multiple locks.
 I think it is probably way faster to read data from pthread_mutex_t from
 the stack than the heap and when you have to read from the heap pthread_mutex_lock
 takes longer finish 

QUESTION 2.3.1 - Cycles in the basic implementation

Where do you believe most of the cycles are spent in the 1 and 2-thread tests (for both add and list)?  Why do you believe these to be the most expensive parts of the code?

ans:

For add, when there is only one or two thread and incrementing/decrementing a counter
is very fast I would have to say most of the cycles was spent spinning or waiting for a lock
in the case of mutex locking. From my lab2b_1.png we see that as you increase the number
of threads, the throughput decreases linearly with the number of threads.

For list, I would say most of the time spent in the actual traversal, insertions, deletions of the
list since there is such a huge difference in the throuput in the add and list graphs in lab2_1.png
and since overhead for locking should be the same in lab2_add and lab2_list.

Where do you believe most of the time/cycles are being spent in the high-thread spin-lock tests?

ans:

For lists,
In the higher thread spin lock tests I would say most of the time was now spent spinning than actualy deletion since
there is a clear continuous linear decrease in throughput as we increase the number of threads

For adds,
as we increase the number of threads, spin locking is still decreasing linearly, so I believe more of the cpu
time was spent more spinning, as more threads need to share a lock, and the spinning time will increase
almost exponentially.


Where do you believe most of the time/cycles are being spent in the high-thread mutex tests?

ans:

in the high thread mutex the overhead for waiting for the locks are constant so the cost per
operation shouldnt change, theoretically. In the lab2_1.png for the add operations, mutex
locking actually started leveling off, but for lists mutex locking continously decrease in throughput.
This is probably due the fact that with higher threads, list operations are more complicated so threads
need to contend against each other for these locks, and since the operations are slower in list, locks
are held longer and the waiting time on average per thread actaully increases.


EXECUTING PROFILE
===================================================================================================


Total: 381 samples
     318  83.5%  83.5%      381 100.0% thread_func2
      42  11.0%  94.5%       42  11.0% __strcmp_sse42
       9   2.4%  96.9%       37   9.7% SortedList_insert
       8   2.1%  99.0%       26   6.8% SortedList_lookup
       4   1.0% 100.0%        4   1.0% _init
       0   0.0% 100.0%      381 100.0% __clone
       0   0.0% 100.0%      381 100.0% start_thread
ROUTINE ====================== thread_func2 in /u/eng/class/classave/cs111/lab2/lab2b/lab2_list.c
   318    381 Total samples (flat / cumulative)
     .      .  464:   return temp;
     .      .  465:   
     .      .  466: }
     .      .  467: 
     .      .  468: void* thread_func2(void* arg)
---
     .      .  469: {
     .      .  470:  
     .      .  471:    int k = 0;
     .      .  472:   int* num = (int*) arg;
     .      .  473:   // printf("++++++++++++++++++++++++num %d\n", *num);
     .      .  474:   SortedListElement_t* curr;
     .      .  475:   for (k = 0; k < niters; k++)
     .      .  476:     {
     .      .  477:       switch(synchro)
     .      .  478:       {
     .      .  479:       case 'm':
     .      .  480: 	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  481: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  482: 	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  483: 	break;
     .      .  484:       case 's':
   251    251  485: 	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
     .     37  486: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  487: 	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
     .      .  488: 	break;
     .      .  489:       default:
     .      .  490: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  491:       }
     .      .  492:     }
     .      .  493:   
     .      .  494:   counter = 0;
     .      .  495:        switch(synchro)
     .      .  496:       {
     .      .  497:       case 'm':
     .      .  498: 	for (k = 0; k < num_lists; k++)
     .      .  499: 	  {
     .      .  500: 	    pthread_mutex_lock(&sub_mutex_lock[k]);
     .      .  501: 	    counter+=SortedList_length(sub_list[k]);
     .      .  502: 	    pthread_mutex_unlock(&sub_mutex_lock[k]);
     .      .  503: 	  }
     .      .  504: 	break;
     .      .  505:       case 's':
     .      .  506: 	for (k = 0; k < num_lists; k++)
     .      .  507: 	  {
     6      6  508: 	    while(__sync_lock_test_and_set(&sub_spin_lock[k], 1));
     .      .  509: 	    counter+=SortedList_length(sub_list[k]); 
     .      .  510: 	    __sync_lock_release(&sub_spin_lock[k]);
     .      .  511: 	  }
     .      .  512: 	break;
     .      .  513:       default:
     .      .  514: 	for (k = 0; k < num_lists; k++)
     .      .  515: 	  counter+=SortedList_length(sub_list[k]); 
     .      .  516:       }
     .      .  517:         
     .      .  518:        for (k = 0; k < niters; k++)
     .      .  519:     {
     .      .  520:       switch(synchro)
     .      .  521:       {
     .      .  522:       case 'm':
     .      .  523: 	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  524: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  525: 	SortedList_delete(curr);
     .      .  526: 	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  527: 	break;
     .      .  528:       case 's':
    61     61  529: 	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
     .     26  530: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  531: 	SortedList_delete(curr);
     .      .  532: 	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
     .      .  533: 	break;
     .      .  534:       default: 
     .      .  535: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  536: 	SortedList_delete(curr);       
     .      .  537:       }
     .      .  538:     }
     .      .  539:        
     .      .  540:        return NULL;
     .      .  541: }
---
     .      .  542: 
     .      .  543: void freesubs()
     .      .  544: {
     .      .  545:   free(sub_spin_lock);
     .      .  546:   free(sub_mutex_lock);
ROUTINE ====================== thread_func2 in /u/eng/class/classave/cs111/lab2/lab2b/lab2_list.c
   318    381 Total samples (flat / cumulative)
     .      .  464:   return temp;
     .      .  465:   
     .      .  466: }
     .      .  467: 
     .      .  468: void* thread_func2(void* arg)
---
     .      .  469: {
     .      .  470:  
     .      .  471:    int k = 0;
     .      .  472:   int* num = (int*) arg;
     .      .  473:   // printf("++++++++++++++++++++++++num %d\n", *num);
     .      .  474:   SortedListElement_t* curr;
     .      .  475:   for (k = 0; k < niters; k++)
     .      .  476:     {
     .      .  477:       switch(synchro)
     .      .  478:       {
     .      .  479:       case 'm':
     .      .  480: 	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  481: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  482: 	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  483: 	break;
     .      .  484:       case 's':
   251    251  485: 	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
     .     37  486: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  487: 	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
     .      .  488: 	break;
     .      .  489:       default:
     .      .  490: 	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
     .      .  491:       }
     .      .  492:     }
     .      .  493:   
     .      .  494:   counter = 0;
     .      .  495:        switch(synchro)
     .      .  496:       {
     .      .  497:       case 'm':
     .      .  498: 	for (k = 0; k < num_lists; k++)
     .      .  499: 	  {
     .      .  500: 	    pthread_mutex_lock(&sub_mutex_lock[k]);
     .      .  501: 	    counter+=SortedList_length(sub_list[k]);
     .      .  502: 	    pthread_mutex_unlock(&sub_mutex_lock[k]);
     .      .  503: 	  }
     .      .  504: 	break;
     .      .  505:       case 's':
     .      .  506: 	for (k = 0; k < num_lists; k++)
     .      .  507: 	  {
     6      6  508: 	    while(__sync_lock_test_and_set(&sub_spin_lock[k], 1));
     .      .  509: 	    counter+=SortedList_length(sub_list[k]); 
     .      .  510: 	    __sync_lock_release(&sub_spin_lock[k]);
     .      .  511: 	  }
     .      .  512: 	break;
     .      .  513:       default:
     .      .  514: 	for (k = 0; k < num_lists; k++)
     .      .  515: 	  counter+=SortedList_length(sub_list[k]); 
     .      .  516:       }
     .      .  517:         
     .      .  518:        for (k = 0; k < niters; k++)
     .      .  519:     {
     .      .  520:       switch(synchro)
     .      .  521:       {
     .      .  522:       case 'm':
     .      .  523: 	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  524: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  525: 	SortedList_delete(curr);
     .      .  526: 	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
     .      .  527: 	break;
     .      .  528:       case 's':
    61     61  529: 	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
     .     26  530: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  531: 	SortedList_delete(curr);
     .      .  532: 	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
     .      .  533: 	break;
     .      .  534:       default: 
     .      .  535: 	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
     .      .  536: 	SortedList_delete(curr);       
     .      .  537:       }
     .      .  538:     }
     .      .  539:        
     .      .  540:        return NULL;
     .      .  541: }
---
     .      .  542: 
     .      .  543: void freesubs()
     .      .  544: {
     .      .  545:   free(sub_spin_lock);
     .      .  546:   free(sub_mutex_lock);
==========================================================================================================


Where (what lines of code) are consuming most of the cycles when the spin-lock version of the list exerciser is run with a large number of threads?
Why does this operation become so expensive with large numbers of threads?

Based on my data the cpu has been in line:

   251    251  485: 	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));

thee most meaning
the profiler interrupted the program and examined this instruction 251 times
and spend 83.5 percent of the time in the threading function, so we can
see clearly that the time spinning comprises most of the cpu time usage

The operation becomes more expensive with more threads because more threads are now spinning longer and there are
more threads spinning in general meaning, more of the cpu is used to keep the threads spinning and thus from lab2_1.png
you can see throughput continuallly decrease with the number of threads due to the fact when spinning the thread
does not allow another thread to take its place or at least until the quantum expires.


QUESTION 2.3.3 - Mutex Wait Time:
Look at the average time per operation (vs # threads) and the average wait-for-mutex time (vs #threads).  

Why does the average lock-wait time rise so dramatically with the number of contending threads?

ans:
With higher number of threads, there are more threads waiting for the lock meaning that the nth threads has to wait
for n-1 threads to grab and release the lock which will increase the average wait time since each successive thread
will have longer wait times

Why does the completion time per operation rise (less dramatically) with the
number of contending threads?

ans:
first of all for completion time I started the time before the threads are created and ended the time after
the threads finish joining, so doing so the average time per operation will be lower than the average wait time
since the number of operations is equal to the number of locks. Also, the lookup time is O(n), insert time is O(n) and
deletion time is O(1) since I counted each of these operations as equal weights, whereas the wait time for the locking
only increases with more threads, so therefore the actual time for completing the insertion/deletion/lookup will
be consitently faster than grabbing the locks.

How is it possible for the wait time per operation to go up faster (or higher) than the completion time per operation?

ans:

As we increase the number of threads, the time for insertions/deletion/lookup is the same, except we are doing more of these
operations when we increase the number of threads, so basically we see more of a linear growth for avg op time vs threads
whereas in avg wait time vs threads, we see more of an exponential growth as with more thread contention, each successive
thread has to wait longer since it has to wait for n-1 threads to finish locking and unlocking.


QUESTION 2.3.4 - Performance of Partitioned Lists
Explain the change in performance of the synchronized methods as a function of the number of lists.

ans:
1) The throughput increased with more lists
2) With more lists the throughput leveled off faster with more threads and
was more stable with more lists

Should the throughput continue increasing as the number of lists is further increased?  If not, explain why not.

ans:
based on my graphs, the throughput decreased from 1 to more threads, started increasing steadily and then leveld off with increasing
threads but only with higher number of lists I think this make sense since with more lists, there are less contention with other threads
with insertion/deletion/lookups
and thus are able to complete more of these operations per time

It seems reasonable to suggest the throughput of an N-way partitioned list should be equivalent to the throughput of a single list with fewer (1/N) threads.  Does this appear to be true in the above curves?  If not, explain why not.

ans:
Based on my graphs with more partitions, the throughput is higher regardless with the number of threads. For example, a 4-lists has higher throughput than 1-lists.
However the throughput of a N-way list is almost equivalent to a single list with less threads. Take for example with a 8 way list with 8 threads and a single list with
1 thread, we can see they are roughly the same except the 8-way is a little better, partially maybe due to the fact that you can do more than one insertion per time interval
same for 4-lists and 4-threads with single lists and l-threads, so it would be reasonable to conclude the the statement is valid, since it is supported by
my graphs.
