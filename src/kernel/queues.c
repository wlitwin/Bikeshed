/*
** SCCS ID:	@(#)queues.c	1.1	4/5/12
**
** File:	queues.c
**
** Author:	4003-506 class of 20113
**
** Contributor:
**
** Description:	Queue module
*/

#define	__KERNEL__20113__

#include "defs.h"
#include "types.h"

#include "pcbs.h"
#include "stacks.h"
#include "semaphores.h"
#include "c_io.h"

#include "memory/kmalloc.h"

/*
** PRIVATE DEFINITIONS
*/

// Number of Qnodes to allocate
//
// need one per PCB, one per Stack, and a few extra

#define	N_QNODES	(N_PCBS + N_STACKS + MAX_SEMAPHORES + 5)

// Number of queues to allocate

#define	N_QUEUES	10 + MAX_SEMAPHORES

/*
** PRIVATE DATA TYPES
*/

// Queue node

typedef struct qnode {
	struct qnode *prev;
	struct qnode *next;
	void *data;
	Key key;
} Qnode;

// A queue

struct queue {
	Qnode *head;
	Qnode *tail;
	int (*compare)(Key, Key);
};

// We define this so that we don't get the "fake" Queue definitions

#define	COMPILING_QUEUE_MODULE
#include "queues.h"

/*
** PRIVATE GLOBAL VARIABLES
*/

// Qnodes

// Unlike PCBs and stacks, we maintain just a simple linked list of
// available Qnodes.

// Queues

// Again, we maintain just a simple linked list of Queues.  The
// list is maintained via the Qnode *head field of the Queue entries.

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** _qnode_alloc()
**
** allocate a qnode, returning a pointer to it or NULL
*/

static Qnode *_qnode_alloc( void ) {
	Qnode* new = (Qnode *)__kmalloc(sizeof(Qnode));

	return( new );
}

/*
** _qnode_dealloc(qnode)
**
** deallocate a qnode, putting it into the list of available qnodes
*/

static Status _qnode_dealloc( Qnode *qn ) {

	// Sanity check here?  What happens if qn is NULL?
	if( qn == NULL ) {
		return BAD_PARAM;
	}

	__kfree(qn);

	return SUCCESS;
}

/*
** _que_alloc()
**
** allocate a queue, returning a pointer to it or NULL
*/

static Queue *_que_alloc( void ) {
	Queue* new = (Queue *)__kmalloc(sizeof(Queue));

	return( new );
}

/*
** _que_dealloc(queue)
**
** deallocate a queue, putting it into the list of available queues
*/

static Status _que_dealloc( Queue *que ) {

	// Sanity check here?  What if que is NULL?
	if( que == NULL ){
		return BAD_PARAM;
	}

	__kfree(que);

	return SUCCESS;
}

/*
** _q_delete_node(queue,qnode)
**
** remove the specified entry from a queue
**
** returns the status of the removal
*/

static Status _q_delete_node( Queue *que, Qnode *qn ) {

	if( que == NULL || qn == NULL ) {
		return( BAD_PARAM );
	}

	if( qn->prev == NULL ) {
		que->head = qn->next;
	} else {
		qn->prev->next = qn->next;
	}

	if( qn->next == NULL ) {
		que->tail = qn->prev;
	} else {
		qn->next->prev = qn->prev;
	}

	return _qnode_dealloc( qn );
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _comp_ascend_int(key1,key2)
**
** compare the two keys as signed integer values
**
** returns the status of the comparison:
**      < 0     key1 < key2
**      = 0     key1 == key2
**      > 0     key1 > key2
*/

int _comp_ascend_int( Key old, Key new ) {
	if( old.i < new.i ) return( -1);
	else if( old.i == new.i ) return( 0 );
	else return( 1 );
}

/*
** _comp_ascend_uint(key1,key2)
**
** compare the two keys as unsigned integer values
**
** returns the status of the comparison:
**      < 0     key1 < key2
**      = 0     key1 == key2
**      > 0     key1 > key2
*/

int _comp_ascend_uint( Key old, Key new ) {
	if( old.u < new.u ) return( -1);
	else if( old.u == new.u ) return( 0 );
	else return( 1 );
}

/*
** _q_init( void )
**
** initialize the queue module
*/

void _q_init( void ) {

	// init qnodes

	// init queues

	// report that we have finished

	c_puts( " queues" );

}

/*
** _q_status(queue)
**
** evaluate the status of a queue
**
** returns the status
*/

Status _q_status( Queue *que ) {

	if( que == NULL ) {
		return( BAD_PARAM );
	}

	if( que->head == NULL ) {
		return( EMPTY_QUEUE );
	}

	return( NOT_EMPTY_QUEUE );

}

/*
** _q_peek(queue,key)
**
** peek into the specified queue, returning the first Key through
** the second parameter
**
** returns the status of the peek attempt
*/

Status _q_peek( Queue *que, Key *key ) {

	if( que == NULL || key == NULL ) {
		return( BAD_PARAM );
	}

	if( _q_empty(que) ) {
		return( EMPTY_QUEUE );
	}

	*key = que->head->key;
	return( SUCCESS );
}

/*
** _q_alloc(queue,compare)
**
** allocate a queue which uses the specified comparison routine,
** returning a pointer to the queue through the first parameter
**
** returns the status of the allocation attempt
*/

Status _q_alloc( Queue **que, int (*compare)(Key,Key) ) {
	Queue *new;

	if( que == NULL ) {
		return( BAD_PARAM );
	}

	new = _que_alloc();

	if( new == NULL ) {
		return( NO_QUEUES );
	}

	new->head = new->tail = NULL;
	new->compare = compare;

	*que = new;
	return( SUCCESS );
}


/*
** _q_dealloc(queue,compare)
**
** deallocates a queue and gives it back to the queue system to be used again.
**
** returns the status of the deallocation attempt
*/

Status _q_dealloc( Queue * que ) {
	_que_dealloc(que);
	return SUCCESS;
}

/*
** _q_insert(queue,data,key)
**
** insert the supplied data value into a queue, using the supplied
** key for ordering according to the queue's ordering rule
**
** returns the status of the insertion attempt
*/

Status _q_insert( Queue *que, void *data, Key key ) {
	Qnode *qn;
	Qnode *curr;
	
	if( que == NULL ) {
		return( BAD_PARAM );
	}

	qn = _qnode_alloc();
	if( qn == NULL ) {
		return( ALLOC_FAILED );
	}

	qn->key = key;
	qn->data = data;

	if( que->compare != NULL ) {

		// find insertion location
	
		curr = que->head;
		while( curr && (*que->compare)(curr->key,key) <= 0 ) {
			curr = curr->next;
		}

	// curr != NULL : add before curr and return
		if( curr != NULL ) {
			qn->prev = curr->prev;
			qn->next = curr;
			if( curr->prev == NULL ) {
				que->head = qn;
			} else {
				curr->prev->next = qn;
			}
			curr->prev = qn;
			return( SUCCESS );
		}

	}

	// add to end
	qn->prev = que->tail;
	qn->next = NULL;

	if( que->tail == NULL ) {
		que->head = qn;
	} else {
		que->tail->next = qn;
	}
	
	que->tail = qn;
	
	return( SUCCESS );

}

/*
** _q_remove(queue,data)
**
** remove the first element from the queue, returning the pointer to
** it through the second parameter
**
** returns the status of the removal attempt
*/

Status _q_remove( Queue *que, void **data ) {
	Qnode *qn;

	if( que == NULL ) {
		return( BAD_PARAM );
	}

	if( _q_empty(que) ) {
		return( EMPTY_QUEUE );
	}

	qn = que->head;
	que->head = qn->next;
	if( que->head == NULL ) {
		que->tail = NULL;
	} else {
		que->head->prev = NULL;
	}

	*data = qn->data;
	;

	return _qnode_dealloc( qn );
}

/*
** _q_remove_by_key(queue,data,key)
**
** remove the first element in the queue which has the supplied key,
** returning the pointer to it through the second parameter
**
** returns the status of the removal attempt
*/

Status _q_remove_by_key( Queue *que, void **data, Key key ) {
	Qnode *qn;

	if( que == NULL ) {
		return( BAD_PARAM );
	}

	if( _q_empty(que) ) {
		return( EMPTY_QUEUE );
	}

	qn = que->head;
	while( qn && qn->key.u != key.u ) {
		qn = qn->next;
	}

	if( qn == NULL ) {
		return( NOT_FOUND );
	}

	*data = qn->data;

	return( _q_delete_node(que,qn) );

}


/*
** _q_get_by_key(queue,data,key)
**
** gets the first element in the queue which has the supplied key,
** returning the pointer to it through the second parameter
**
** returns the status of the get attempt
*/

Status _q_get_by_key( Queue *que, void **data, Key key ) {
	Qnode *qn;

	if( que == NULL ) {
		return( BAD_PARAM );
	}

	if( _q_empty(que) ) {
		return( EMPTY_QUEUE );
	}

	qn = que->head;
	while( qn && qn->key.u != key.u ) {
		qn = qn->next;
	}

	if( qn == NULL ) {
		return( NOT_FOUND );
	}

	*data = qn->data;

	return( SUCCESS );

}


/*
** _q_remove_selected(queue,data,compare,lookfor)
**
** scan a queue looking for a particular entry (specified by the lookfor
** parameter) using the supplied comparison function, removing the node,
** and returning the pointer to the node through the second parameter
**
** returns the status of the removal attempt
*/

Status _q_remove_selected( Queue *que, void **data,
			    int (*compare)(void*,void*),
			    void *lookfor ) {
	Qnode *qn;

	if( que == NULL ) {
		return( BAD_PARAM );
	}


	if( _q_empty(que) ) {
		return( EMPTY_QUEUE );
	}

	qn = que->head;
	while( qn && compare(qn->data,lookfor) != 0 ) {
		qn = qn->next;
	}

	if( qn == NULL ) {
		return( NOT_FOUND );
	}

	*data = qn->data;

	return( _q_delete_node(que,qn) );

}


/*
** _q_dump(which,queue)
**
** dump the contents of the specified queue to the console
*/

void _q_dump( char *which, Queue *queue ) {
	Qnode *tmp;


	c_printf( "%s: ", which );
	if( queue == NULL ) {
		c_puts( "NULL???" );
		return;
	}

	c_printf( "head %08x tail %08x comp %08x\n",
		  queue->head, queue->tail, queue->compare );
	
	if( !_q_empty(queue) ) {
		c_puts( " keys: " );
		for( tmp = queue->head; tmp != NULL; tmp = tmp->next ) {
			c_printf( " [%x]", tmp->key.u );
		}
		c_puts( "\n" );
	}

}
