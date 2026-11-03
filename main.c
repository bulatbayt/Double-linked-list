#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "color.h"

/*
Добавить Head Tail
Кольцевой список, служебная и информационная часть
Графический дамп списка
*/
#define FREE_ELEM -1
#define POISON_ELEM 666

typedef int list_elem_t;

struct list_info 
{
    list_elem_t* data;
    int*         next;
    int*         prev;
    ssize_t      size_list;
    int          err;
    int          head;
    int          tail;
    int*         free;
};


enum List_Error 
{
    LIST_OK          = 0,
    LIST_NULL        = 1 << 1,
    DATA_NULL        = 1 << 2,
    NEXT_NULL        = 1 << 3,
    PREV_NULL        = 1 << 4,
    POISON_BAD       = 1 << 5,
    LOOPING          = 1 << 6,
};

int  List_Init       (struct list_info* list, ssize_t size_list);
int  List_Verify     (struct list_info* list);
void List_Destroy    (struct list_info* list);
int  List_Dump       (struct list_info* list, FILE* write_dump);

int  Add             (struct list_info* list, list_elem_t value);
int  Delete          (struct list_info* list, list_elem_t value);

void Print_Error     (struct list_info* list);
int  Find_Free       (struct list_info* list);
void Dump_Graph      (struct list_info* list, FILE* graph_write);
void Dump_To_Html    (struct list_info* list, const char* html_filename, const char* png_filename);



int main ()
{
    struct list_info list1 = { };
    FILE* write_dump = fopen ("Dump_list.txt", "w");
    FILE* graph_dump = fopen ("list_dump.dot", "w");

    List_Init (&list1, 15);

    Add (&list1, 10);
    Add (&list1, 20);
    Add (&list1, 30);
    Add (&list1, 40);
    Add (&list1, 50);

    Delete (&list1, 30);

    Add (&list1, 35);
    Add (&list1, 70);
    Add (&list1, 65);
    Add (&list1, 45);

    Dump_To_Html (&list1, "log.html", "list_dump.png");
    List_Dump (&list1, write_dump);

    List_Destroy (&list1);
    fclose (write_dump);
    fclose (graph_dump);
    return 0;
}


int  List_Init    (struct list_info* list, ssize_t size_list)
{
    assert (list != 0);

    list->size_list = size_list;

    list->data = calloc (size_list, sizeof (list_elem_t));
    list->next = calloc (size_list, sizeof (int));
    list->prev = calloc (size_list, sizeof (int));

    if (list->data == NULL || list->next == NULL || list->prev == NULL)
        return -1;

    list->data[0] = POISON_ELEM; 
    list->next[0] = 0; 
    list->prev[0] = 0;
    
    list->head = 0;   
    list->tail = 0;  

    
    for (int i = 1; i < size_list; i++)
    {
        list->data[i] = FREE_ELEM;
        list->next[i] = -1;
        list->prev[i] = -1;
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



int  List_Dump       (struct list_info* list, FILE* write_dump)
{
    assert (list != 0);

    Print_Error (list);

    size_t j = 1;

    for ( size_t i = 1; i < list->size_list; i++)
    {
        
        printf ("Elem number <%zu> = <%d>\n", i, *(list->data+j));

        j = list->next[j];

        if (j == 0)
        {
            printf ("Finish elem\n");
            break;
        }
    }

    fprintf (write_dump, "Head = %d, Tail = %d\n", list->head, list->tail);

    
    fprintf (write_dump, "\n\n================Data==================\n");
    for (int k = 0; k < list->size_list; k++)
    {
        fprintf (write_dump, "Elem number (in data) <%d> = %d\n", k, list->data[k]);
    }   

    fprintf (write_dump, "\n\n================Next==================\n"); 
    for (int k = 0; k < list->size_list; k++)
    {
        fprintf (write_dump, "Elem number (in next) <%d> = %d\n", k, list->next[k]);
    }      

    fprintf (write_dump, "\n\n================Prev==================\n"); 
    for (int k = 0; k < list->size_list; k++)
    {
        fprintf (write_dump, "Elem number (in prev) <%d> = %d\n", k, list->prev[k]);
    }      

    return 0;
}

int  List_Verify     (struct list_info* list)
{
    if (list == NULL) return -1;
    list->err = LIST_OK;

    
    if (list->next[0] != list->head) list->err |= POISON_BAD;
    if (list->prev[0] != list->tail) list->err |= POISON_BAD;

    
    int size_oborot = 0;
    int i = list->head; 

    while (i != 0)
    {
        if (i < 0 || i >= list->size_list) 
        {
            list->err |= POISON_BAD;
            return -1;
        }

        i = list->next[i];
        size_oborot++;
        
        if (size_oborot > list->size_list)
        {
            list->err |= LOOPING;
            return -1;
        }
    }

    return 0;
}

void Print_Error     (struct list_info* list)
{
    List_Verify (list);

    if (list->err == 0)
    {
        printf (GREEN"List OK\n"RESET_COLOR);
    }
    else
    {
        if (list->err & LIST_NULL)  printf (RED"List == NULL\n"RESET_COLOR);
        if (list->err & DATA_NULL)  printf (RED"Data == NULL\n"RESET_COLOR);
        if (list->err & NEXT_NULL)  printf (RED"Next == NULL\n"RESET_COLOR);
        if (list->err & PREV_NULL)  printf (RED"Prev == NULL\n"RESET_COLOR);
        if (list->err & POISON_BAD) printf (RED"Bad index (poison)\n"RESET_COLOR);
        if (list->err & LOOPING)    printf (RED"Cycle of infinite length\n"RESET_COLOR);
    }
}

int  Find_Free       (struct list_info* list)
{
    for (int i = 1; i < list->size_list; i++)
    {
        if (list->data[i] == FREE_ELEM)
            return i;
    }
    return -1;
}



int  Add          (struct list_info* list, list_elem_t value)
{
    int point = Find_Free (list);

    if (point == -1)
    {
        printf ("No free elements\n");
        return -1;
    }

    
    int current = list->head; 
    int prev_current = 0; 

    while (current != 0)
    {
        prev_current = current;
        current = list->next[current];
    }

    
    list->data[point] = value;

    list->next[point] = current;
    list->prev[point] = prev_current;

    list->next[prev_current] = point;
    list->prev[current] = point; 


    if (prev_current == 0) 
        list->head = point;

    if (current == 0) 
        list->tail = point;

    return 0;
}

int  Delete       (struct list_info* list, list_elem_t value)
{
    int number_elem = list->head; 
    
    while (number_elem != 0)
    {
        if (list->data[number_elem] == value)
            break;

        number_elem = list->next[number_elem];
    }

    if (number_elem == 0)
    {
        printf ("Don't succeed in find elem <%d>\n", value);
        return -1;
    }

    int prev_elem = list->prev[number_elem];
    int next_elem = list->next[number_elem];

    
    list->next[prev_elem] = next_elem;
    list->prev[next_elem] = prev_elem;

    
    if (number_elem == list->head)
        list->head = next_elem; 

    if (number_elem == list->tail)
        list->tail = prev_elem;

    list->data[number_elem] = FREE_ELEM;
    list->next[number_elem] = -1;
    list->prev[number_elem] = -1;

    return 0;
}

void Dump_Graph   (struct list_info* list, FILE* graph_write)
{
    fprintf (graph_write, "digraph G {\n");
    fprintf (graph_write, "  rankdir = LR;\n");
    fprintf (graph_write, "  node [shape = Mrecord; style = filled;];\n");
    fprintf (graph_write, "  edge [color = darkblue;];\n\n");

    
    for (size_t i = 0; i < list->size_list; i++)
    {
        
        const char* fillcolor = "#C0FFC0"; 
        const char* color     = "#008000"; 

        if (i == 0) 
        {
            fillcolor = "#FFC0CB"; 
            color     = "#800080"; 
        }
        else if (list->data[i] != -1) 
        {
            fillcolor = "#C0E0FF"; 
            color     = "#0000FF";
        }


        fprintf (graph_write, "  V%zu [label = \"index: %zu | value: %d | {prev: %d | next: %d}\"; fillcolor = \"%s\"; color = \"%s\";];\n",
                 i, i, list->data[i], list->prev[i], list->next[i], fillcolor, color);
    }
    fprintf (graph_write, "\n");

    
    for (size_t i = 0; i < list->size_list - 1; i++)
    {
        fprintf (graph_write, "  V%zu -> V%zu [style=invis];\n", i, i + 1);
    }
    fprintf (graph_write, "\n");
    
    
    if (list->head != 0) {
        fprintf (graph_write, "  V0 -> V%d [dir=both; constraint=false];\n", list->head);
    }

    for (size_t i = 1; i < list->size_list; i++)
    {
        
        if (list->data[i] != -1 && list->next[i] != -1)
        {
            if (i != list->next[list->prev[i]])
            {
                fprintf (graph_write, "  V%zu -> V%d [dir=both; constraint=false; color =\"#bc003e\"];\n", i, list->next[i]);
            }
            
            fprintf (graph_write, "  V%zu -> V%d [dir=both; constraint=false];\n", i, list->next[i]);
            
        }
    }

    fprintf (graph_write, "\n  HEAD [label = \"HEAD\"];\n");
    if (list->head != 0)
        fprintf (graph_write, "  { rank = same; HEAD; V%d; }\n", list->head);

    fprintf (graph_write, "  TAIL [label = \"TAIL\"];\n");
    if (list->tail != 0)
        fprintf (graph_write, "  { rank = same; TAIL; V%d; }\n", list->tail);

    fprintf (graph_write, "}\n");
}

void Dump_To_Html (struct list_info* list, const char* html_filename, const char* png_filename)
{
    FILE* dot_file = fopen ("list_dump.dot", "w");
    if (dot_file != NULL)
    {
        Dump_Graph (list, dot_file);
        fclose (dot_file);
    }


    char command[256];
    snprintf (command, sizeof (command), "dot -Tpng list_dump.dot -o %s", png_filename);
    system (command);


    FILE* html = fopen (html_filename, "w");
    if (html == NULL) return;

    
    fprintf (html, "<pre>\n");
    fprintf (html, "<h3> DUMP <font color=\"#be1fde\">Double linked list</font> </h3>\n");
    fprintf (html, "List { %p }\n\n", (void*)list);
    

    fprintf (html, "       ");
    for (int i = 0; i < list->size_list; i++) 
    {
        fprintf (html, "<font color=\"#bc8500\">%03d        </font>", i);
    }
    fprintf (html, "\n");
    fprintf (html, "\n");
    



    fprintf (html, "<font color=\"#1f0e91\">Data:  </font>");
    for (int i = 0; i < list->size_list; i++) 
    {
                if (list->data[i] == POISON_ELEM)
        {
            fprintf (html, "<font color=\"#bc003e\">%03d        </font>", list->data[i]);    
        }
        else if (list->data[i] == FREE_ELEM)
        {
            fprintf (html, "<font color=\"#00bc7e\">%03d        </font>", list->data[i]);
        }
        else
        {
            fprintf (html, "%03d        ", list->data[i]);
        }
    }
    fprintf (html, "\n");
    fprintf (html, "\n");




    fprintf (html,"<font color=\"#1f0e91\">Next:  </font>");
    for (int i = 0; i < list->size_list; i++) 
    {
        if (list->next[i] == FREE_ELEM)
        {
            fprintf (html, "<font color=\"#00bc7e\">%03d        </font>", list->next[i]);
        }
        else
        {
            fprintf (html, "%03d        ", list->next[i]);
        }
    }
    fprintf (html, "\n");
    fprintf (html, "\n");
    
    



    fprintf (html,"<font color=\"#1f0e91\">Prev:  </font>");
    for (int i = 0; i < list->size_list; i++) 
    {
        if (list->prev[i] == FREE_ELEM)
        {
            fprintf (html, "<font color=\"#00bc7e\">%03d        </font>", list->prev[i]);
        }
        else
        {
            fprintf (html, "%03d        ", list->prev[i]);
        }
    }
    fprintf (html, "\n\n");

    



    fprintf (html, "Image:\n");
    fprintf (html, "<img src=\"%s\" width=\"1600px\">\n", png_filename);
    
    fprintf (html, "</pre>\n");
    fclose (html);

    
    printf ("HTML dump generated: %s\n", html_filename);
}
