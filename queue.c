#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!new)
        return NULL;
    new->next = new;
    new->prev = new;
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    while (l->next != l) {
        element_t *tmp = container_of(l->next, element_t, list);
        l->next = l->next->next;
        if (tmp->value)
            free(tmp->value);
        free(tmp);
    }
    free(l);
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *insertH = (element_t *) malloc(sizeof(element_t));
    if (!insertH)
        return false;
    size_t sSize = strlen(s) + 1;
    insertH->value = (char *) malloc(sSize);
    if (!insertH->value) {
        free(insertH);
        return false;
    }
    strncpy(insertH->value, s, sSize);
    struct list_head *tmp = head->next;
    head->next = &insertH->list;
    insertH->list.next = tmp;
    insertH->list.prev = head;
    tmp->prev = &insertH->list;

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *insertH = (element_t *) malloc(sizeof(element_t));
    if (!insertH)
        return false;
    size_t sSize = strlen(s) + 1;
    insertH->value = (char *) malloc(sSize);
    if (!insertH->value) {
        free(insertH);
        return false;
    }
    strncpy(insertH->value, s, sSize);
    struct list_head *tmp = head->prev;
    head->prev = &insertH->list;
    insertH->list.next = head;
    insertH->list.prev = tmp;
    tmp->next = &insertH->list;

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (q_size(head) == 0)
        return NULL;
    struct list_head *delL = head->next;
    head->next = delL->next;
    (delL->next)->prev = head;
    element_t *delE = container_of(delL, element_t, list);
    if (delE->value && sp) {
        size_t sSize = strlen(delE->value) + 1;
        if (sSize > bufsize) {
            strncpy(sp, delE->value, bufsize);
            sp[bufsize - 1] = '\0';
        } else
            strncpy(sp, delE->value, sSize);
    }

    return delE;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (q_size(head) == 0)
        return NULL;
    struct list_head *delL = head->prev;
    head->prev = delL->prev;
    (delL->prev)->next = head;
    element_t *delE = container_of(delL, element_t, list);
    if (delE->value && sp) {
        size_t sSize = strlen(delE->value) + 1;
        if (sSize > bufsize) {
            strncpy(sp, delE->value, bufsize);
            sp[bufsize - 1] = '\0';
        } else
            strncpy(sp, delE->value, sSize);
    }
    return delE;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head)
        return false;
    struct list_head *slow = head, *fast = head, *prevL;
    do {
        prevL = slow;
        slow = slow->next;
        fast = fast->next;
        if (fast != head)
            fast = fast->next;
    } while (fast != head);
    prevL->next = slow->next;
    slow->next->prev = prevL;
    element_t *delE = container_of(slow, element_t, list);
    if (delE->value)
        free(delE->value);
    free(delE);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (head == NULL)
        return false;
    else if (head->next == head)
        return true;
    struct list_head *ptrL = head->next, *prevL = head;
    while (ptrL->next != head) {
        struct list_head *nextL = ptrL->next;
        element_t *e1 = container_of(ptrL, element_t, list);
        element_t *e2 = container_of(nextL, element_t, list);
        if (strcmp(e1->value, e2->value) == 0) {
            char *str = e1->value;
            free(e1->value);
            free(e1);
            while (strcmp(str, e2->value) == 0) {
                nextL = nextL->next;
                free(e2->value);
                free(e2);
                e2 = container_of(nextL, element_t, list);
            }
            prevL->next = nextL;
        } else {
            prevL->next = nextL;
            prevL = prevL->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || head->next == head)
        return;
    struct list_head *ptrL = head->next, *prevL = head;
    while (ptrL->next != head) {
        struct list_head *tmp = ptrL;
        ptrL = ptrL->next->next;
        prevL->next = tmp->next;
        prevL->next->next = tmp;
        prevL = prevL->next->next;
        prevL->next = ptrL;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *node, *tmp;
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    list_for_each (node, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || head->next == head)
        return;

    // int size = q_size(head);
    struct list_head *ptrL, *endL = head;
    while (endL != head->next) {
        element_t *maxE = container_of(endL->prev, element_t, list);
        for (ptrL = head->next; ptrL != endL; ptrL = ptrL->next) {
            element_t *curE = container_of(ptrL, element_t, list);
            if (strcmp(curE->value, maxE->value) > 0) {
                maxE = curE;
            }
        }
        maxE->list.prev->next = maxE->list.next;
        maxE->list.next->prev = maxE->list.prev;
        struct list_head *tmp = endL->prev;
        endL->prev = &maxE->list;
        maxE->list.next = endL;
        maxE->list.prev = tmp;
        tmp->next = &maxE->list;
        endL = &maxE->list;
    }
}
