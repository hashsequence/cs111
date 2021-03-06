#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include "SortedList.h"

int niters = 1;
int nthreads = 1;
int num_lists = 1;
int finalexit = 0;
int opt_yield = 0;
int my_synch_lock = 0;
char synchro = 'n';
long long counter = 0;
pthread_mutex_t my_mutex_lock;

SortedList_t* my_list;
SortedListElement_t **e_arr;


int insert_flag = 0;
int delete_flag = 0;
int lookup_flag = 0;

void yield_arg_parser(char* arg);

SortedList_t* init_empty_list();

void freelist(SortedList_t* list);

SortedListElement_t* init_Element_List(int iterations);

void* thread_func(void* arg);

/*********************************************/
SortedList_t** sub_list;
int* sub_spin_lock;
pthread_mutex_t* sub_mutex_lock;
unsigned long** e_hash;
struct timespec* threadtime_s;
struct timespec* threadtime_e;
unsigned long** threadtime_c;
unsigned long total_thread_mutex_time = 0;
unsigned long avg_thread_time_per_mutex_lock = 0;

unsigned long hashf(const char* key);
void init_sub_lists();
void init_sub_locks();
unsigned long* init_e_hash(int k, int iterations);
void* thread_func2(void* arg);
void freesubs();
/*********************************************/

int main(int argc, char **argv)
{
  pthread_t* threads;
  int check;
  int i = 0;
  int k = 0;
  int d;
  int option_index;
  
  long  num_of_operations = 0;
  long  tot_run_time = 0;
  long  avg_time_per_op = 0;
 
  struct timespec begin, end;
  
  int* thread_num;

  char nameroot[] = "list";
  char yieldopts1[] = "-none";
  char yieldopts2[] = "-i";
  char yieldopts3[] = "-d";
  char yieldopts4[] = "-l";
  char yieldopts5[] = "-id";
  char yieldopts6[] = "-il";
  char yieldopts7[] = "-dl";
  char yieldopts8[] = "-idl";
  char syncopts1[] = "-none";
  char syncopts2[] = "-s";
  char syncopts3[] = "-m";
  char *yieldopt = yieldopts1;
  char *syncopt = syncopts1;

  static struct option long_options[]=
    {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {"lists",required_argument,0,'l'},
      {0,0,0,0}
    };

  while((d = getopt_long(argc, argv, "", long_options, &option_index)) != -1)
    {
      switch(d)
	{
	case 't':
	  nthreads = atoi(optarg);
	  break;
	case 'i':
	  niters = atoi(optarg);
	  break;
	case 'y':
	  yield_arg_parser(optarg);
	  break;
	case 's':
	  if (strlen(optarg) != 1 || (optarg[0] != 'm' && optarg[0] != 's'))
	    {
	      fprintf(stderr, "ERROR: sync argument has to be either m, s, or c\n");
	      exit(1);
	    }
	  synchro = optarg[0];
	  if (synchro == 'm')
	    {
	      syncopt = syncopts3;
	    }
	  else if (synchro == 's')
	    {
	      syncopt = syncopts2;
	    }
	  else;
	
	  break;
	case 'l':
	  num_lists = atoi(optarg);
	  break;
	default:
	  fprintf(stderr,"ERROR: option not available\n");
	}
    }

  /*
  my_list = init_empty_list();
  e_arr = (SortedListElement_t**)malloc(sizeof(SortedListElement_t*)*nthreads);
  srand(time(NULL));
  for (i = 0; i < nthreads; i++)
    {
      e_arr[i] = init_Element_List(niters); 
    }
  */
  
  /*
  for (k = 0; k < nthreads; k++)
    {
      printf("-----------thread %d------------\n",k+1);
  for (i = 0; i < niters; i++)
    {
      printf("%d %s\n",i+1,e_arr[k][i].key);
    }
    }
  */
  //  printf("before init_sub_list()\n");
  init_sub_lists();
  //printf("after init_sub_list()\n");
  threads = (pthread_t*)malloc(sizeof(pthread_t)*nthreads);
  thread_num = (int*)malloc(sizeof(int)*nthreads);

  
  
  if(threads == NULL)
    {
      fprintf(stderr, "error allocating memory for threads\n");
      exit(1);
    }
  /*
  if (synchro == 'm')
    {
      pthread_mutex_init(&my_mutex_lock, NULL);
    }
  */
  check = clock_gettime(CLOCK_MONOTONIC, &begin);
  if (check == -1)
    {
      fprintf(stderr, "ERROR: clock_gettime begin failed\n");
      exit(1);
    }
    
  
  for (i = 0; i < nthreads; i++)
    {
      thread_num[i] = i;
      // printf("-------------------thread_num[%d]\n",i);
      check = pthread_create(&threads[i],NULL, thread_func2, &thread_num[i]);
      if (check != 0)
	{
	  fprintf(stderr, "ERROR: creating threads, return value: %d \n", check);
	  exit(1);
	}
    }
  

  for (i = 0; i < nthreads; i++)
    {
      check = pthread_join(threads[i],NULL);
      if (check != 0)
        {
          fprintf(stderr, "ERROR: joining threads, return value: %d \n",check);
          exit(1);
	}
    }
      check = clock_gettime(CLOCK_MONOTONIC, &end);
      if (check == -1)
	{
	  fprintf(stderr,"ERROR: clock_gettime end failed\n");
	  exit(1);
	}
      
      /*            
      i = 0;
      SortedList_t *kurr;
      for(k = 0; k < num_lists; k++)
	  {
	    kurr = sub_list[k];
	    for (kurr = sub_list[k]->next; kurr != sub_list[k]; kurr = kurr->next)
	      {
		printf("%d: %s\n",i,kurr->key);
		i++;
	      } 
	  }
      */
      if (insert_flag == 1 && delete_flag == 0 && lookup_flag == 0)
      {
	yieldopt = yieldopts2;
      }
      else if (insert_flag == 0 && delete_flag == 1 && lookup_flag == 0)
      {
	yieldopt = yieldopts3;
      } 
      else if (insert_flag == 0 && delete_flag == 0 && lookup_flag == 1)
      {
	yieldopt = yieldopts4;
      }
      else if (insert_flag == 1 && delete_flag == 1 && lookup_flag == 0)
      {
	yieldopt = yieldopts5;
      }
      else if (insert_flag == 1 && delete_flag == 0 && lookup_flag == 1)
      {
	yieldopt = yieldopts6;
      }
      else if (insert_flag == 0 && delete_flag == 1 && lookup_flag == 1)
      {
	yieldopt = yieldopts7;
      }
      else if (insert_flag == 1 && delete_flag == 1 && lookup_flag == 1)
      {
	yieldopt = yieldopts8;
      }
      else;
            
      for (i = 0; i < nthreads; i++)
	{
	  for(k = 0; k < niters; k++)
	    {
	      total_thread_mutex_time += threadtime_c[i][k];
	      //	      	      printf("threadtime_c[%d][%d]: %d\n", i,k, threadtime_c[i][k]);
	    }
	}
      
      avg_thread_time_per_mutex_lock = total_thread_mutex_time / (niters*nthreads*3); 

      for (i = 0; i < nthreads; i++)
	{
	  free(e_arr[i]);
	}
      free(threads);
      free(e_arr);
      free(thread_num);
      freesubs();
      num_of_operations = nthreads * niters * 3;
      tot_run_time = (end.tv_sec - begin.tv_sec) * 1000000000 + (end.tv_nsec - begin.tv_nsec);
      avg_time_per_op = (tot_run_time) / (num_of_operations);
      
      printf("%s%s%s,%d,%d,%d,%ld,%ld,%ld,%lu\n", nameroot, yieldopt, syncopt, nthreads, niters,num_lists, num_of_operations,tot_run_time,avg_time_per_op, avg_thread_time_per_mutex_lock);

      exit(finalexit);
}

void yield_arg_parser(char* arg)
{
  
  int k = 0;
  char arr[] = "idl";
  for (k = 0; arg[k] != '\0'; k++)
    {
      if (arg[k] == arr[0])
	{
	  opt_yield |= INSERT_YIELD;
	  insert_flag  = 1;
	}
      else if (arg[k] == arr[1])
	{
	  opt_yield |= DELETE_YIELD;
	  delete_flag = 1;
	}
      else if (arg[k] == arr[2])
	{
	  opt_yield |= LOOKUP_YIELD;
	  lookup_flag = 1;
	}
      else
	{
	  finalexit = 1;
	}
    }
}



SortedList_t* init_empty_list()
{
  SortedList_t* temp = (SortedList_t*)malloc(sizeof(SortedList_t));
  temp->key = NULL;
  temp->next = temp;
  temp->prev = temp;
  return temp;
}


void freelist(SortedList_t* list)
{
  SortedList_t* curr = list->next; 
  while(curr != list) 
    {
      curr = curr->next;
      free(curr->prev);
    }
  free(curr);
}

SortedListElement_t* init_Element_List(int iterations)
{
  int k = 0, j = 0;
  int randomlength;
  char randomascii;
  char* new_key;
  SortedListElement_t* all_elements = (SortedListElement_t*)malloc(sizeof(SortedListElement_t)*iterations);

  if (all_elements == NULL)
    {
      fprintf(stderr,"ERROR: allocating memory for all elements\n");
      exit(1);
    }
  
  
  for (k = 0; k < iterations; k++)
    {
       randomlength = rand()%(50+1-1)+1;
       new_key = (char*)malloc(sizeof(char)*randomlength+1);
       for (j = 0; j < randomlength; j++)
	 {
	   randomascii = rand()%(126+1-32)+32;
	   new_key[j] = randomascii;
	 }
       new_key[randomlength] = '\0';
       all_elements[k].key = new_key;
    }
  return all_elements;
  
}

void* thread_func(void* arg)
{
  int k = 0;
  int* num = (int*) arg;
  //   printf("++++++++++++++++++++++++num %d\n", *num);
  SortedListElement_t* curr;
  for (k = 0; k < niters; k++)
    {
      switch(synchro)
      {
      case 'm':
	pthread_mutex_lock(&my_mutex_lock);
	SortedList_insert(my_list, &(e_arr[*num][k])); 
	pthread_mutex_unlock(&my_mutex_lock);
	break;
      case 's':
	while(__sync_lock_test_and_set(&my_synch_lock, 1));
	SortedList_insert(my_list, &(e_arr[*num][k]));
	__sync_lock_release(&my_synch_lock);
	break;
      default:
	SortedList_insert(my_list, &(e_arr[*num][k]));
      }
    }
  counter  = SortedList_length(my_list);
         
  for (k = 0; k < niters; k++)
    {
      switch(synchro)
      {
      case 'm':
	pthread_mutex_lock(&my_mutex_lock);
	curr = SortedList_lookup(my_list, e_arr[*num][k].key);
	SortedList_delete(curr);
	pthread_mutex_unlock(&my_mutex_lock);
	break;
      case 's':
	while(__sync_lock_test_and_set(&my_synch_lock, 1));
	curr = SortedList_lookup(my_list, e_arr[*num][k].key);
	SortedList_delete(curr);
	__sync_lock_release(&my_synch_lock);
	break;
      default: 
	curr = SortedList_lookup(my_list, e_arr[*num][k].key);
	SortedList_delete(curr);       
      }
    }
   counter  = SortedList_length(my_list);
  
   return NULL;
}

unsigned long  hashf(const char* cp)
{
  /* D. J. Bernstein hash function */
  unsigned long  hash = 5381;
  while (*cp)
    hash = 33 * hash ^ (unsigned char) *cp++;
 return  hash % num_lists;

}

void init_sub_lists()
{
  sub_list = (SortedList_t**)malloc(sizeof(SortedList_t*)*num_lists);
  int k = 0;
  int j = 0;
  for (k = 0; k < num_lists; k++)
    {
      sub_list[k] = init_empty_list();
    }
  init_sub_locks();
  e_arr = (SortedListElement_t**)malloc(sizeof(SortedListElement_t*)*nthreads);
  e_hash = (unsigned long**)malloc(sizeof(unsigned long*)*nthreads);
  threadtime_c  = (unsigned long**)malloc(sizeof(unsigned long*)*nthreads);
  srand(time(NULL));
  for (k = 0; k < nthreads; k++)
    {
      e_arr[k] = init_Element_List(niters);
      e_hash[k] = init_e_hash(k,niters);
    }
  for (j = 0; j < nthreads; j++)
    {
      threadtime_c[j] = (unsigned long*)malloc(sizeof(unsigned long)*niters);
      for (k = 0; k < niters; k++)
	{
	  threadtime_c[j][k] = 0;
	}
    }
  threadtime_s = (struct timespec*)malloc(sizeof(struct timespec)*nthreads);
  threadtime_e = (struct timespec*)malloc(sizeof(struct timespec)*nthreads);
}

void init_sub_locks()
{
  int k = 0;
  if (synchro == 'm')
    {
      sub_mutex_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*num_lists);
      for (k = 0; k < num_lists; k++)
	{
	  pthread_mutex_init(&sub_mutex_lock[k],NULL);
	}
    }
  else if (synchro == 's')
    {
      sub_spin_lock = (int*)malloc(sizeof(int)*num_lists);
      for (k = 0; k < num_lists; k++)
	{
	  sub_spin_lock[k] = 0;
	}
    }
}

unsigned long* init_e_hash(int k, int iterations)
{
  int j = 0;
  unsigned long* temp  = (unsigned long*)malloc(sizeof(unsigned long)*iterations);
  for (j = 0; j < iterations; j++)
    {
      temp[j] = hashf(e_arr[k][j].key);
    }
  return temp;
  
}

void* thread_func2(void* arg)
{
  int check = 0;
  int k = 0;
  int* num = (int*) arg;
  //  printf("++++++++++++++++++++++++num %d\n", *num);
  SortedListElement_t* curr;
  for (k = 0; k < niters; k++)
    {
      switch(synchro)
      {
      case 'm': 
	check = 0;
	check = clock_gettime(CLOCK_MONOTONIC, &threadtime_s[*num]);
	if (check == -1)
	  {
	    fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	  }
	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
	check = clock_gettime(CLOCK_MONOTONIC, &threadtime_e[*num]);
	if (check == -1)
	  {
	    fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	  }
	threadtime_c[*num][k] += (threadtime_e[*num].tv_sec - threadtime_s[*num].tv_sec) * 1000000000 + (threadtime_e[*num].tv_nsec - threadtime_s[*num].tv_nsec);
	//printf("threadtime_c[%d][%d]: %d\n", *num,k, threadtime_c[*num][k]);
	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
	break;
      case 's':
	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
	break;
      default:
	SortedList_insert(sub_list[e_hash[*num][k]], &(e_arr[*num][k])); 
      }
    }
  
  counter = 0;
       switch(synchro)
      {
      case 'm':
	for (k = 0; k < num_lists; k++)
	  {
	    check = 0;
	    check = clock_gettime(CLOCK_MONOTONIC, &threadtime_s[*num]);
	    if (check == -1)
	      {
		fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	      }
	    pthread_mutex_lock(&sub_mutex_lock[k]);
	    check = clock_gettime(CLOCK_MONOTONIC, &threadtime_e[*num]);
	    if (check == -1)
	      {
		fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	      }
	    threadtime_c[*num][k] += (threadtime_e[*num].tv_sec - threadtime_s[*num].tv_sec) * 1000000000 + (threadtime_e[*num].tv_nsec - threadtime_s[*num].tv_nsec);
	    counter+=SortedList_length(sub_list[k]);
	    pthread_mutex_unlock(&sub_mutex_lock[k]);
	  }
	break;
      case 's':
	for (k = 0; k < num_lists; k++)
	  {
	    while(__sync_lock_test_and_set(&sub_spin_lock[k], 1));
	    counter+=SortedList_length(sub_list[k]); 
	    __sync_lock_release(&sub_spin_lock[k]);
	  }
	break;
      default:
	for (k = 0; k < num_lists; k++)
	  counter+=SortedList_length(sub_list[k]); 
      }
        
       for (k = 0; k < niters; k++)
    {
      switch(synchro)
      {
      case 'm':
	check = 0;
	check = clock_gettime(CLOCK_MONOTONIC, &threadtime_s[*num]);
	if (check == -1)
	   {
	     fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	   }
	pthread_mutex_lock(&sub_mutex_lock[e_hash[*num][k]]);
	check = clock_gettime(CLOCK_MONOTONIC, &threadtime_e[*num]);
	if (check == -1)
          {
	     fprintf(stderr, "ERROR: clock_gettime begin failed\n");
	  }
        threadtime_c[*num][k] += (threadtime_e[*num].tv_sec - threadtime_s[*num].tv_sec) * 1000000000 + (threadtime_e[*num].tv_nsec - threadtime_s[*num].tv_nsec);
	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
	SortedList_delete(curr);
	pthread_mutex_unlock(&sub_mutex_lock[e_hash[*num][k]]);
	break;
      case 's':
	while(__sync_lock_test_and_set(&sub_spin_lock[e_hash[*num][k]], 1));
	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
	SortedList_delete(curr);
	__sync_lock_release(&sub_spin_lock[e_hash[*num][k]]);
	break;
      default: 
	curr = SortedList_lookup(sub_list[e_hash[*num][k]], e_arr[*num][k].key);
	SortedList_delete(curr);       
      }
    }
       
       return NULL;
}

void freesubs()
{
  free(sub_spin_lock);
  free(sub_mutex_lock);
  free(sub_list);
  int k = 0;
  for (k = 0; k < nthreads; k++)
    {
      free(e_hash[k]);
      free(threadtime_c[k]);
    }
  free(e_hash);
  free(threadtime_c);
  free(threadtime_e);
  free(threadtime_s);
}
