#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int list_elem_t;

struct  list_info 
{
    int*    data;
    int*    next;
    int*    prev;
    ssize_t size_list;
    int     err;
};

int  List_Init       (struct list_info* list, ssize_t size_list);
int  List_Verify     (struct list_info* list);
void List_Destroy    (struct list_info* list);
void List_Dump       (struct list_info* list);


int main ()
{
    struct list_info list1 = { };
    List_Init (&list1, 15);


    for ( size_t i = 1; i < 11; i++)
    {
        *(list1.data + i) = 10 *i;
    }

    List_Dump (&list1);



    List_Destroy (&list1);

    return 0;
}

int List_Init (struct list_info* list, ssize_t size_list)
{
    assert (list != 0);

    list->size_list = size_list;

    list->data = calloc (size_list, sizeof (list_elem_t));
    list->next = calloc (size_list, sizeof (list_elem_t));
    list->prev = calloc (size_list, sizeof (list_elem_t));


    *list->data = 666; 

    for (size_t i = 1; i < size_list; i++)
    {
        *(list->data + i) = -1;
    }


    for (size_t i = 1; i < size_list; i++)
    {
        *(list->next + i) = -1;
    }

    return 0;
}

void List_Destroy (struct list_info* list)
{
    assert (list != 0);

    free (list->data);
    free (list->next);
    free (list->prev);

}

int List_Verify  (struct list_info* list)
{
    assert (list != 0);

    
}

void List_Dump (struct list_info* list)
{
    assert (list != 0);

    for ( size_t i = 0; i < list->size_list; i++)
    {
        printf ("Elem number <%zu> = <%d>\n", i, *(list->data+i));
    }
}