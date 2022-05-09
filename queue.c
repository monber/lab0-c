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
    struct list_head *cur = l->next;
    while (cur != l) {
        element_t *tmp = container_of(cur, element_t, list);
        cur = cur->next;
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
    while (ptrL != head && ptrL->next != head) {
        struct list_head *nextL = ptrL->next;
        element_t *e1 = container_of(ptrL, element_t, list);
        element_t *e2 = container_of(nextL, element_t, list);
        if (strcmp(e1->value, e2->value) == 0) {
            // char *str = e1->value; //when free(e1->val) str would cause
            // problems
            while (strcmp(e1->value, e2->value) == 0) {
                nextL = nextL->next;
                free(e2->value);
                free(e2);
                if (nextL == head)
                    break;
                else
                    e2 = container_of(nextL, element_t, list);
            }
            free(e1->value);
            free(e1);
            prevL->next = nextL;
            nextL->prev = prevL;
            // prevL = prevL->next;
            ptrL = nextL;
        } else {
            prevL->next = ptrL;
            ptrL->prev = prevL;
            prevL = prevL->next;
            ptrL = ptrL->next;
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
    while (ptrL != head && ptrL->next != head) {
        prevL->next = ptrL->next;
        ptrL->next->prev = prevL;
        ptrL->next = ptrL->next->next;
        ptrL->next->prev = ptrL;
        prevL->next->next = ptrL;
        ptrL->prev = prevL->next;
        // renew 2 pointer
        prevL = ptrL;
        ptrL = ptrL->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head)
        return;
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
    if (l1)
        *cur = l1;
    else
        *cur = l2;
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
    if (!head || head->next == head)
        return;
    struct list_head *headL = head->next;
    head->prev->next = NULL;
    headL = mergesort(headL);
    mergefinal(head, headL);
}
