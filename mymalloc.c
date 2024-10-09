


#include "mymalloc.h"
#include <stdio.h>


#include <unistd.h>
#include <string.h>

#ifndef LEAK
#define LEAK 0
#endif
#define MEMSIZE 4096
#define HEADERSIZE 32
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

static struct CHUNK* mylist = NULL;

void* 
mymalloc(size_t size, char *file, int line)
{
    printf ("first line : mymalloc called\n");
    fflush(NULL);
    if (!initialized)
       init ();

    struct CHUNK* tmp = mylist;
    int available_mem = 0;
    int biggest_block = 0;
    while (tmp != NULL)
    {
        if (tmp->is_used == 0 && tmp->size >= size)
        {
            printf ("request to allocate %ld is being addressed\n", size );
            tmp->is_used = 1;
            void *ptr = (void *) (tmp );
            ptr = ptr + sizeof(struct CHUNK) + size;
            struct CHUNK *newNode = (struct CHUNK *) ptr;
            printf ("new node created %ld and tmp offset = %ld\n ", newNode,tmp + sizeof(struct CHUNK) + size );
            printf ("size in node available = %ld\n", tmp->size);
            printf ("size offset  = %ld\n", (size +  (sizeof (struct CHUNK)) ));
            newNode->size = tmp->size - (size +  (sizeof (struct CHUNK)) ); 
            printf ("new size left after request to allocate %ld is %ld\n", size, newNode->size  );
            newNode->is_used = 0;
            newNode->next = NULL;
            newNode->payload = (char*)( newNode + sizeof (struct CHUNK));

            
            tmp->size = size;
            tmp->next = newNode;
            return tmp->payload;
        }
        else if (tmp->is_used)
        {
            if (tmp->size > biggest_block)
            {
                biggest_block = tmp->size;
            }
            available_mem += tmp->size;
        }
        tmp = tmp->next;

    }
    
    if (tmp == NULL)
    {
        printf ("Memory not available: memory requested =  %ld and total memory free = %d and biggest block available = %d\n", size, available_mem, biggest_block );    
        printf ("Unable to allocate %ld bytes\n", size);
    }
}


void checkmemleak (void)
{
   
   int leak = 0;
   struct CHUNK* tmp = mylist;
   while (tmp != NULL)
   {
      if (tmp->is_used == 1)
      {
        printf ("mem leak of %ld bytes\n", tmp->size);
        leak = leak + tmp->size;
      } 
      tmp = tmp->next;
   }
   printf ("leak of %d bytes detected\n", leak);
}
void init ()
{
    printf (" init sizeof chunk = %d", sizeof (struct CHUNK));
    if (!initialized)
    {
        mylist = (struct CHUNK*) &global_heap;
        mylist->is_used = 0;
        mylist->size = MEMLENGTH - sizeof (struct CHUNK);
        mylist->payload = (char*) (mylist + sizeof (struct CHUNK));
        initialized = 1;

        if (atexit(checkmemleak) != 0) {
            printf("Failed to register the tutorialspoint function with atexit\n");
            exit (2);
        }
   }
}




void compact_mem()
{
    prinf ("running compact mem");
    struct CHUNK* tmp = mylist; 
    while (tmp != NULL)  
    {
        if (tmp->is_used == 0 && tmp->next != NULL && tmp->next->is_used == 0)
        {
           tmp->size = tmp->size + tmp->next->size + sizeof (struct CHUNK);
           tmp->next = tmp->next->next;
        }
        else
        {
            tmp = tmp->next;
        }
    }
}
void  myfree(void *ptr, char *file, int line)
{
    printf (" my free called\n");
    if (!initialized)
       init ();

    struct CHUNK* tmp = mylist;   
    char found = 0;
    while (tmp != NULL)
    {
        if (tmp->payload == ptr)
        {
            //found: we need to free this block and make it available
            tmp->is_used = 0;
            found = 1;
            break;

        }
        else
           tmp = tmp->next;
        
    }
    if (found == 1)
    {
           compact_mem();
    }
    else
    {
        printf ("free: Inappropriate pointer (%s %d)\n", file, line);
    }
}



