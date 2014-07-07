#define list_entry(node, type, member)    ((type *)((U8 *)(node) - (U32)(&((type *)0)->member)))

#define OFFSET(TYPE,NUMBER)      (int)(&((TYPE *)0)->NUMBER)

#define double_list 3
#if (double_list == 1)
typedef struct LIST{
    struct LIST *prev;
    struct LIST *next;
}LIST;


void list_init(LIST *head)
{
    head->prev = head;
    head->next = head;
}

void list_insert(LIST *head,LIST *element)
{
    element->next    = head->next;
    element->prev    = head;
    
    head->next->prev = element;
    head->next       = element; 
}

void list_delect(LIST *head)
{
    head->prev->next = head->next;
    head->next->prev = head->prev;
}

int list_is_empty(LIST *head)
{
    return (head->next == head);
}

#elif (double_list == 2)

typedef struct LIST{
    struct LIST *next;
}LIST;

void list_init(LIST *head)
{
    head->next = NULL;
}

void list_insert(LIST *head, LIST *element )
{
    element->next = NULL;
    head->next    = element;
}

int list_is_empty(LIST *head)
{
    return (head->next == NULL);
}

#else

void list_init(char a )
{
    a = 1;
}


void list_insert(char a )
{
    a = 1;
}

int list_is_empty(char a)
{
    return (a==1);
}

#endif
