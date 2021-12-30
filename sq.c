//
// An implementation file to meet all of the requirements specified in sq.h
//
//
// Adithya Jose
// U. of Illinois, Chicago
// CS 211: Fall 2021
//

#include <stdio.h>

#include <stdlib.h>

#include "sq.h"

// creates the node struct and it's attributes
struct node {
  struct node * next;
  struct node * prev;
  int inQ;
  int val;
};

typedef struct node node;

// linked-list structure
typedef struct {
  node * front;
  node * back;
  int size;

}
list;

/**
 * SQ is an "incompletely specified type."  
 *
 * The definition of struct service_queue must
 * be (hidden) in an implementation .c file.
 *
 * This has two consequences:
 *
 *	- Different implementations are free to
 *	 	specify the structure as suits
 *		their approach.
 *
 *	- "client" code can only have pointers
 *		to service_queue structures (type
 *		SQ *).  More importantly, it cannot
 *		de-reference the pointers.  This
 *		lets us enforce one of the principles
 *		of ADTs:  the only operations that
 *		can be performed on a service queue
 *		are those specified in the interface
 *		-- i.e., the functions specified below.
 */
struct service_queue {
  list * the_queue;
  node ** arrOfPNodes;
  list * buzzer_bucket;
  int nodesInQ;
  int sizeOfArr;
  int numOfPFNodes;
};

// function that creates a list for a stack and queue structure
list * createList() {

  list * l = malloc(sizeof(list));
  l -> size = 0;
  l -> front = NULL;
  l -> back = NULL;

  return l;

}

/**
 * Function: sq_create()
 * Description: creates and intializes an empty service queue.
 * 	It is returned as an SQ pointer.
 * 
 * RUNTIME REQUIREMENT: O(1)
 */
extern SQ * sq_create() {

  SQ * q = malloc(sizeof(SQ));
  q -> the_queue = createList();
  q -> buzzer_bucket = createList();
  q -> arrOfPNodes = malloc(4 * sizeof(node * ));
  q -> sizeOfArr = 4;
  q -> nodesInQ = -1;
  q -> numOfPFNodes = 0;

  return q;

}

/**
 * Function: sq_free()
 * Description:  see sq.h
 *
 * RUNTIME REQUIREMENT:  O(N_t)  where N_t is the number of buzzer 
 *	IDs that have been used during the lifetime of the
 *	service queue; in general, at any particular instant
 *	the actual queue length may be less than N_t.
 *
 *	[See discussion of in header file]
 *
 * RUNTIME ACHIEVED:  ???
 *
 */
void sq_free(SQ * q) {
  for (int i = 0; i < q -> numOfPFNodes; ++i) {
    free(q -> arrOfPNodes[i]);
  }

  free(q -> the_queue);
  free(q -> buzzer_bucket);
  free(q -> arrOfPNodes);
  free(q);

}

/**
 * Function: sq_display()
 * Description:  prints the buzzer IDs currently
 *    in the queue from front to back.
 *
 * RUNTIME REQUIREMENT:  O(N)  (where N is the current queue
 *		length).
 */
extern void sq_display(SQ * q) {
  node * current = q -> the_queue -> front;

  while (current != NULL) {
    printf("%d ", current -> val);
    current = current -> next;
  }

  printf("\n");

}

/**
 * Function: sq_length()
 * Description:  returns the current number of
 *    entries in the queue.
 *
 * RUNTIME REQUIREMENT:  O(1)
 */
extern int sq_length(SQ * q) {
  return q -> nodesInQ + 1;
}

// function to link a node to a list
void linkNode(SQ * q, node * Node, int type) {
  list * Ltype = q -> the_queue;

  if (type == 1)
    Ltype = q -> buzzer_bucket;

  if (Ltype -> front == NULL) {
    Ltype -> front = Node;
    Ltype -> back = Node;
    return;
  }

  Node -> prev = Ltype -> back;
  Ltype -> back -> next = Node;
  Node -> next = NULL;
  Ltype -> back = Node;

}

// resizes a pointer to the nodes array
void reSize(SQ * q) {

  node ** newNode = malloc(q -> sizeOfArr * 2 * sizeof(node * ));

  for (int i = 0; i < q -> sizeOfArr; ++i) {
    newNode[i] = q -> arrOfPNodes[i];
  }

  q -> sizeOfArr *= 2;
  free(q -> arrOfPNodes);
  q -> arrOfPNodes = newNode;
}

node * addNode(SQ * q, int type) {

  node * newNode = malloc(sizeof(node));
  newNode -> next = NULL;
  newNode -> prev = NULL;
  newNode -> inQ = 0;
  newNode -> val = -1;
  linkNode(q, newNode, type);

  return newNode;
}

/**
* Function: sq_give_buzzer()
* Description:  This is the "enqueue" operation.  For us
*    a "buzzer" is represented by an integer (starting
*    from zero).  The function selects an available buzzer 
*    and places a new entry at the end of the service queue 
*    with the selected buzer-ID. 
*    This buzzer ID is returned.
*    The assigned buzzer-ID is a non-negative integer 
*    with the following properties:
*
*       (1) the buzzer (really it's ID) is not currently 
*         taken -- i.e., not in the queue.  (It
*         may have been in the queue at some previous
*         time -- i.e., buzzer can be re-used).
*	  This makes sense:  you can't give the same
*	  buzzer to two people!
*
*       (2) If there are buzzers that can be re-used, one
*         of those buzzers is used.  A re-usable buzzer is 
*	  a buzzer that _was_ in the queue at some previous
*	  time, but currently is not.
*
*       (3) if there are no previously-used buzzers, the smallest 
*         possible buzzer-ID is used (retrieved from inventory).  
*	  Properties in this situation (where N is the current
*	  queue length):
*
*		- The largest buzzer-ID used so far is N-1
*
*		- All buzzer-IDs in {0..N-1} are in the queue
*			(in some order).
*
*		- The next buzzer-ID (from the basement) is N.
*
*    In other words, you can always get more buzzers (from
*    the basement or something), but you don't fetch an
*    additional buzzer unless you have to.
*    you don't order new buzzers 
*
* Comments/Reminders:
*
*	Rule (3) implies that when we start from an empty queue,
*	the first buzzer-ID will be 0 (zero).
*
*	Rule (2) does NOT require that the _minimum_ reuseable 
*	buzzer-ID be used.  If there are multiple reuseable buzzers, 
*	any one of them will do.
*	
*	Note the following property:  if there are no re-useable 
*	buzzers, the queue contains all buzzers in {0..N-1} where
*       N is the current queue length (of course, the buzzer IDs 
*	may be in any order.)
*
* RUNTIME REQUIREMENT:  O(1)  ON AVERAGE or "AMORTIZED"  
		In other words, if there have been M calls to 
*		sq_give_buzzer, the total time taken for those 
*		M calls is O(M).
*
*		An individual call may therefore not be O(1) so long
*		as when taken as a whole they average constant time.
*
*		(Hopefully this reminds you of an idea we employed in
*		the array-based implementation of the stack ADT).
*/
extern int sq_give_buzzer(SQ * q) {
  int value = 0;

  if (q -> buzzer_bucket -> front != NULL) {
    node * buzzer = q -> buzzer_bucket -> back;
    buzzer -> inQ = 1;
    q -> buzzer_bucket -> back = q -> buzzer_bucket -> back -> prev;
    value = buzzer -> val;
    linkNode(q, buzzer, 0);

    if (q -> buzzer_bucket -> front == buzzer)
      q -> buzzer_bucket -> front = NULL;
  } else {

    if (q -> nodesInQ + 1 >= q -> sizeOfArr) {
      reSize(q);
    }

    node * newNode = addNode(q, 0);

    newNode -> val = q -> numOfPFNodes;
    newNode -> inQ = 1;
    q -> arrOfPNodes[newNode -> val] = newNode;
    value = q -> numOfPFNodes++;
  }
  q -> nodesInQ++;
  return value;
}

/**
 * function: sq_seat()
 * description:  if the queue is non-empty, it removes the first 
 *	 entry from (front of queue) and returns the 
 *	 buzzer ID.
 *	 Note that the returned buzzer can now be re-used.
 *
 *	 If the queue is empty (nobody to seat), -1 is returned to
 *	 indicate this fact.
 *
 * RUNTIME REQUIREMENT:  O(1)
 */
extern int sq_seat(SQ * q) {
  if (q -> the_queue -> front == NULL)
    return -1;

  node * deleted = q -> the_queue -> front;
  deleted -> inQ = 0;

  if (deleted -> next != NULL)
    deleted -> next -> prev = NULL;
  else if (deleted -> prev != NULL)
    deleted -> prev -> next = NULL;

  q -> the_queue -> front = q -> the_queue -> front -> next;
  linkNode(q, deleted, 1);
  q -> nodesInQ--;
  node * curr = q -> buzzer_bucket -> front;

  return q -> buzzer_bucket -> back -> val;
}

/**
 * function: sq_kick_out()
 *
 * description:  Some times buzzer holders cause trouble and
 *		a bouncer needs to take back their buzzer and
 *		tell them to get lost.
 *
 *		Specifially:
 *
 *		If the buzzer given by the 2nd parameter is 
 *		in the queue, the buzzer is removed (and the
 *		buzzer can now be re-used) and 1 (one) is
 *		returned (indicating success).
 *
 *		If the buzzer isn't actually currently in the
 *		queue, the queue is unchanged and 0 is returned
 *		(indicating failure).
 *
 * RUNTIME REQUIREMENT:  O(1)
 */
extern int sq_kick_out(SQ * q, int buzzer) {

  if (q -> numOfPFNodes <= buzzer || q -> arrOfPNodes[buzzer] -> inQ == 0)
    return 0;

  node * current = q -> arrOfPNodes[buzzer];

  if (q -> arrOfPNodes[buzzer] -> prev != NULL || q -> arrOfPNodes[buzzer] -> next != NULL) {
    if (q -> arrOfPNodes[buzzer] -> prev == NULL) {
      q -> the_queue -> front = q -> the_queue -> front -> next;
      q -> the_queue -> front -> prev = NULL;
    } else if (q -> arrOfPNodes[buzzer] -> next == NULL) {
      q -> the_queue -> back = q -> the_queue -> back -> prev;
      q -> the_queue -> back -> next = NULL;
    } else {
      q -> arrOfPNodes[buzzer] -> prev -> next = q -> arrOfPNodes[buzzer] -> next;
      q -> arrOfPNodes[buzzer] -> next -> prev = q -> arrOfPNodes[buzzer] -> prev;
    }
  } else if (q -> arrOfPNodes[buzzer] -> prev == NULL && q -> arrOfPNodes[buzzer] -> next == NULL) {
    q -> the_queue -> front = NULL;
    q -> the_queue -> back = NULL;
  }

  q -> arrOfPNodes[buzzer] -> inQ = 0;
  linkNode(q, q -> arrOfPNodes[buzzer], 1);
  q -> nodesInQ--;

  return 1;

}

/**
 * function:  sq_take_bribe()
 * description:  some people just don't think the rules of everyday
 *		life apply to them!  They always want to be at
 *		the front of the line and don't mind bribing
 *		a bouncer to get there.
 *
 *	        In terms of the function:
 *
 *		  - if the given buzzer is in the queue, it is 
 *		    moved from its current position to the front
 *		    of the queue.  1 is returned indicating success
 *		    of the operation.
 *		  - if the buzzer is not in the queue, the queue 
 *		    is unchanged and 0 is returned (operation failed).
 *
 * RUNTIME REQUIREMENT:  O(1)
 */
extern int sq_take_bribe(SQ * q, int buzzer) {
  if (q -> numOfPFNodes <= buzzer || q -> nodesInQ == -1 || q -> arrOfPNodes[buzzer] -> inQ == 0)
    return 0;
  if (q -> the_queue -> front == q -> arrOfPNodes[buzzer])
    return 1;
  node * node = q -> arrOfPNodes[buzzer];
  if (q -> arrOfPNodes[buzzer] -> next == NULL) {
    node -> prev -> next = NULL;
    q -> the_queue -> back = node -> prev;
    q -> the_queue -> front -> prev = node;
    node -> next = q -> the_queue -> front;
    q -> the_queue -> front = node;
    node -> prev = NULL;
  } else {
    node -> prev -> next = node -> next;
    node -> next -> prev = node -> prev;
    node -> next = q -> the_queue -> front;
    q -> the_queue -> front -> prev = node;
    q -> the_queue -> front = node;

  }
  return 1;

}