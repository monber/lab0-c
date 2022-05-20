#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

static struct list_head *merge(struct list_head *l1, struct list_head *l2);

static struct list_head *mergesort(struct list_head *head);

static void mergefinal(struct list_head *head, struct list_head *sortHead);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (new)
        INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *ptrE, *nextE;
    list_for_each_entry_safe (ptrE, nextE, l, list)
        q_release_element(ptrE);
    free(l);
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *newE = (element_t *) malloc(sizeof(element_t));
    if (!newE)
        return false;
    size_t sSize = strlen(s) + 1;
    newE->value = (char *) malloc(sSize);
    if (!newE->value) {
        free(newE);
        return false;
    }
    strncpy(newE->value, s, sSize);
    list_add(&newE->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *newE = (element_t *) malloc(sizeof(element_t));
    if (!newE)
        return false;
    size_t sSize = strlen(s) + 1;
    newE->value = (char *) malloc(sSize);
    if (!newE->value) {
        free(newE);
        return false;
    }
    strncpy(newE->value, s, sSize);
    list_add_tail(&newE->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *delL = head->next;
    list_del(delL);
    element_t *delE = container_of(delL, element_t, list);
    if (delE->value && sp) {
        strncpy(sp, delE->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return delE;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *delL = head->prev;
    list_del(delL);
    element_t *delE = container_of(delL, element_t, list);
    if (delE->value && sp) {
        strncpy(sp, delE->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
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
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next, *fast = slow->next;
    while (slow != head && fast != head) {
        slow = slow->next;
        fast = fast->next;
        if (fast != head)
            fast = fast->next;
    }
    list_del(slow);
    q_release_element(container_of(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    else if (list_empty(head))
        return true;
    element_t *ptr, *next;
    bool check, checkpre = false;
    list_for_each_entry_safe (ptr, next, head, list) {
        if (&next->list != head && strcmp(ptr->value, next->value) == 0)
            check = true;
        else
            check = false;

        if (check | checkpre) {
            list_del(&ptr->list);
            q_release_element(ptr);
        }
        checkpre = check;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *ptrL = head->next, *nextL = ptrL->next;
    for (; ptrL != head && nextL != head;
         ptrL = ptrL->next, nextL = ptrL->next) {
        list_move(ptrL, nextL);
        // renew 2 pointer
        // ptrL = ptrL->next;
        // nextL = ptrL->next;
    }
}

/* Reverse queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *ptrL, *nextL;
    list_for_each_safe (ptrL, nextL, head)
        list_move(ptrL, head);
}

static struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *ret = NULL, **cur = &ret;
    element_t *E1 = container_of(l1, element_t, list);
    element_t *E2 = container_of(l2, element_t, list);
    for (; l1 && l2; cur = &(*cur)->next) {
        if (strcmp(E1->value, E2->value) < 0) {
            *cur = l1;
            l1 = l1->next;
            E1 = container_of(l1, element_t, list);
        } else {
            *cur = l2;
            l2 = l2->next;
            E2 = container_of(l2, element_t, list);
        }
    }
    if (!l1)
        *cur = l2;
    else
        *cur = l1;
    return ret;
}

static struct list_head *mergesort(struct list_head *head)
{
    if (head->next == NULL)
        return head;
    struct list_head *fast = head->next, *slow = head, *l, *r;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    r = mergesort(slow->next);
    slow->next = NULL;
    l = mergesort(head);
    return merge(l, r);
}

static void mergefinal(struct list_head *head, struct list_head *sortHead)
{
    struct list_head *prev = sortHead, *cur;
    for (cur = sortHead->next; cur; cur = cur->next) {
        prev->next = cur;
        cur->prev = prev;
        prev = prev->next;
    }
    head->next = sortHead;
    sortHead->prev = head;
    head->prev = prev;
    prev->next = head;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    // transform doubly list into singly list
    struct list_head *headL = head->next;
    head->prev->next = NULL;
    headL = mergesort(headL);
    mergefinal(head, headL);
}
