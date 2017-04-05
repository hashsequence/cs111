#define _GNU_SOURCE
#include "SortedList.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
      
  if (element == NULL || list ==NULL)
    {
      return;
    }
  
 
  SortedListElement_t* curr = list->next;
      for (; curr != list; curr = curr->next)
	{
	  //  if (element->key <  curr->key)
	  if(strcmp(element->key, curr->key) <= 0)
	    {
	      break;
	    }
	}

        if (opt_yield & INSERT_YIELD)
	   pthread_yield();

	  element->next = curr;
	  element->prev = curr->prev;
	  curr->prev->next = element;
	  curr->prev = element;    
}

int SortedList_delete(SortedListElement_t *element)
{
  if (element == NULL)
    {
      return 1;
    }
  if (element->next->prev == element->prev->next)
    {
      if(opt_yield & DELETE_YIELD)
	pthread_yield();
      element->next->prev = element->prev;
      element->prev->next = element->next;
      return 0;
    }
  return 1;

}

SortedListElement_t*  SortedList_lookup(SortedList_t *list, const char *key)
{
 SortedListElement_t* curr;
 for (curr = list->next; curr != list; curr = curr->next)
   {
     if(strcmp(curr->key, key) == 0)
       {
	 return curr;
       }
       if(opt_yield & LOOKUP_YIELD)
       pthread_yield();

   } 
 return NULL;
}

int SortedList_length(SortedList_t *list)
{
  int counter = 0;
 SortedListElement_t* curr;
 for (curr = list->next; curr != list; curr = curr->next)
   {
      counter++;
      if(opt_yield & LOOKUP_YIELD)
       pthread_yield();

   }
 return counter;
}
