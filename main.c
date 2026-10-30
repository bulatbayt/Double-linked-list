#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "color.h"

#define FREE_ELEM -1

typedef int list_elem_t;

struct  list_info 
{
    int*    data;
    int*    next;
    int*    prev;
    ssize_t size_list;
    int     err;
    int*    free;
};

enum List_Error 
{
    LIST_OK          = 0,
    LIST_NULL        = 0 << 1,
    DATA_NULL        = 0 << 2,
    NEXT_NULL        = 0 << 3,
    PREV_NULL        = 0 << 4,
    POISON_BAD       = 0 << 5,
    LOOPING          = 0 << 6,
};

int  List_Init       (struct list_info* list, ssize_t size_list);
int  List_Verify     (struct list_info* list);
void List_Destroy    (struct list_info* list);
int  List_Dump       (struct list_info* list, FILE* write_dump);

int  Add             (struct list_info* list, list_elem_t value);
int  Delete          (struct list_info* list, list_elem_t value);

void Print_Error (struct list_info* list);

int main ()
{
    struct list_info list1 = { };
    FILE* write_dump = fopen ("Dump_list.txt", "w");

    List_Init (&list1, 15);

    list1.next[0] = 0;
    list1.next[1] = 2;
    list1.next[2] = 3;
    list1.next[3] = 4;
    list1.next[4] = 5;
    list1.next[5] = 6;
    list1.next[6] = 7;
    list1.next[7] = 8;
    list1.next[8] = 9;
    list1.next[9] = 0;

    list1.prev[1] = 0;
    list1.prev[2] = 1;
    list1.prev[3] = 2;
    list1.prev[4] = 10;
    list1.prev[5] = 4;
    list1.prev[6] = 5;
    list1.prev[7] = 6;
    list1.prev[8] = 7;
    list1.prev[9] = 8;
    list1.prev[10] = 3;


    for ( size_t i = 1; i < 10; i++)
    {
        *(list1.data + i) = 10 *i;
    }

    if ( List_Dump (&list1, write_dump) == -1)
    {
        List_Destroy (&list1);
        fclose (write_dump);
    }


    Add (&list1, 35);

    Delete (&list1, 70);
    Delete (&list1, 35);


    if ( List_Dump (&list1, write_dump) == -1)
    {
        List_Destroy (&list1);
        fclose (write_dump);
    }

    List_Destroy (&list1);


    fclose (write_dump);
    return 0;
}

int  List_Init          (struct list_info* list, ssize_t size_list)
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

void List_Destroy       (struct list_info* list)
{
    assert (list != 0);

    free (list->data);
    free (list->next);
    free (list->prev);
}

int  List_Verify        (struct list_info* list) // сделать проверку на конечность ( оборот длины size)
{
    assert (list != 0);

    if (list == NULL)
    {
        list->err |= LIST_NULL;
        return -1;
    }

    if (list->data == NULL)
        list->err |= DATA_NULL;

    if (list->next == NULL)
        list->err |= NEXT_NULL;

    if (list->prev == NULL)
        list->err |= PREV_NULL;

    size_t size_oborot = 0;
    size_t i = 1;

    while (list->next[i] != 0)
    {
        i = list->next[i];
        size_oborot++;
        if (size_oborot >= 15)
        {
            list->err |= LOOPING;
            break;
        }
    }

    return 0;
}

int  List_Dump          (struct list_info* list, FILE* write_dump) // а если первый не стоит на первом месте
{
    assert (list != 0);

    Print_Error (list);

    int j = 1;
    int i = 1;

    /*for ( size_t i = 1; i < list->size_list; i++)
    {
        
        printf ("Elem number <%zu> = <%d>\n", i, *(list->data+j));

        j = list->next[i];

        if (j == 0)
        {
            printf ("Finish elem\n");
            break;
        }
    }*/

    while ( j != 0) 
    {
        printf ("Elem number <%zu> = <%d>\n", i, *(list->data+j));
        i++;
        j = list->next[j];

        if (i >= list->size_list)
        {
            printf ("infinite loop\n");
            return -1;
        }
    }

    fprintf (write_dump, "\n\n================Data==================\n");
    for ( size_t i = 0; i < list->size_list; i++)
    {
        fprintf (write_dump, "Elem number (in data) <%zu> = %d\n", i, *(list->data+i));
    }   

    fprintf (write_dump, "\n\n================Next==================\n"); 
    for ( size_t i = 0; i < list->size_list; i++)
    {
        fprintf (write_dump, "Elem number (in next) <%zu> = %d\n", i, *(list->next+i));
    }      

    fprintf (write_dump, "\n\n================Prev==================\n"); 
    for ( size_t i = 0; i < list->size_list; i++)
    {
        fprintf (write_dump, "Elem number (in prev) <%zu> = %d\n", i, *(list->prev+i));
    }      
    

    return 0;
}

void Print_Error        (struct list_info* list)
{
    List_Verify (list);

    if (list->err == 0)
    {
        printf (GREEN"Stack OK\n"RESET_COLOR);
    }

    else
    {
        if (list->err && LIST_NULL) printf (RED"List == NULL\n"RESET_COLOR);
        if (list->err && DATA_NULL) printf (RED"Data == NULL\n"RESET_COLOR);
        if (list->err && NEXT_NULL) printf (RED"Next == NULL\n"RESET_COLOR);
        if (list->err && PREV_NULL) printf (RED"Prev == NULL\n"RESET_COLOR);
        if (list->err && LOOPING  ) printf (RED"Cycle of infinite length\n"RESET_COLOR);
    }
}

int  Add                (struct list_info* list, list_elem_t value) // пока добавляем ручками, нужно будет сделать функцию, которая определяет, за каким элемент будет идти наш добавленный элемент
{
    ssize_t point = 0;
    while (*(list->data+point) != -1)
    {
        point++;

        if (point >= list->size_list)
        {
            printf ("No free elements\n");
            return -1;
        }
    }

    *(list->data + point ) = value; 

    // теперь для next

    *(list->next + point) = 4;
    *(list->next + 4 - 1) = point;
    
    return 0;
}

int  Delete             (struct list_info* list, list_elem_t value)
{
    size_t number_elem = 0;
    while (number_elem < list->size_list)
    {
        if (list->data[number_elem] == value)
            break;

        number_elem ++;
        if (number_elem == list->size_list)
        {
            printf ("Don't succeed in find elem <%d>", value); // заменить %d 
            return -1;
        }

    }
    size_t find_elem = list->prev[number_elem];
    list->next[find_elem] = list->next[number_elem];
    list->prev[list->next[number_elem]] = list->prev[number_elem];
    list->next[number_elem] = FREE_ELEM;
    list->prev[number_elem] = FREE_ELEM;
    //найдем элемент, который указывает на numer_elem в next, присвоим ему то, на что указывал next
    
    return 0;
}
