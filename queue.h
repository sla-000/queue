/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
// sla_r@rambler.ru (c) 2013
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#pragma once
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#include <stdint.h>

#include <types.h>
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#define QUEUE_VER_MAJOR 		    		                1
#define QUEUE_VER_MINOR     		    		            0
#define QUEUE_VER_BUGFIX                                    1
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Private data
typedef struct {
    QueueTypeNum_t Start;		///< Index of first element. In empty queue == 0
	QueueTypeNum_t End;			///< Index of last element. In empty queue == 0. When queue is full (Full == TRUE) Start equals End
							    ///< End points where next element will be added */

    BOOL8 Full;			///< Full flag
} QueuePrivate_t;

/// Settings
typedef struct {
    QueueTypeNum_t BufElem;		///< Number of elements in user buffer
    QueueTypeSize_t ElemSize;		///< Size of one element, eg sizeof()

    /// Semaphores for multithreaded environment
    struct {
        BOOLX (*Take)(void);	///< Take semaphore (blocking, waiting function), NULL if not used
        void (*Give)(void);		///< Give semaphore, NULL if not used
    } Sem;

    /// User implemented interface. Here is can be whatever you need - FRAM interface, perfocards, vinils )
    struct {
        BOOLX (*Write)(QueueTypeAddr_t offset, const void * pdata);		///< Write one element with offset, size is ElemSize
        BOOLX (*Read)(QueueTypeAddr_t offset, void * pdata);		    ///< Read one element with offset, size is ElemSize

        BOOLX (*WritePrivate)(const QueuePrivate_t * priv);		    ///< Write private data to whatever you want
        BOOLX (*ReadPrivate)(QueuePrivate_t * priv);				///< Read private data from your source
    } Iface;

    BOOL8 OverWrite;			///< TRUE if you want old data overwrite when add new data with QueuePush()
} QueueSett_t;

/// Control structure
typedef struct {
	union {
		QueueSett_t * Sett;				///< Settings
		const QueueSett_t * ConstSett;	///< Const settings
	};

    BOOL8 RO;				///< Disable write to queue

    /// Private
    struct {
    	BOOL8 Inited;       ///< Inited flag
    } Private;
} Queue_t;

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Clear queue
extern BOOLX QueueClear( Queue_t * queue );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Get current size (number of elements)
extern QueueTypeSize_t QueueSize( Queue_t * queue );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Push data to the end of queue
extern BOOLX QueuePush( Queue_t * queue, const void * val );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Remove first (oldest) data from queue
extern BOOLX QueuePopFront( Queue_t * queue );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Get first data
#define QueueFront(queue, pval)			QueuePeek((queue), (pval), 0)

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/**
 * Get any data, from pointed position
 * @param num Index: 0-first (oldest) data; 1-second; (QueueSize() - 1) - last added (most fresh) data
 */
extern BOOLX QueuePeek(Queue_t * queue, void * pval, QueueTypeNum_t num);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Get last added data 
/// @todo QueueBack()
extern BOOLX QueueBack(Queue_t * queue, void * pval);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Delete from queue last added data
/// @todo QueuePopBack()
extern BOOLX QueuePopBack(Queue_t * queue);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/**
 * Read-only mode control
 *
 * @param enable TRUE - read0only, FALSE - write enabled
 */
extern void QueueReadOnly( Queue_t * queue, BOOLX enable );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Get number of data
/**
 * @param val Where to put data
 * @param num How much elements to get
 * @return How much elements was get
 */
extern QueueTypeSize_t QueueGet( Queue_t * queue, void * pval, QueueTypeSize_t num );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Add number of data
/**
 * @param val Where to get data
 * @param num How much elements to set
 * @return How much elements was set
 */
extern QueueTypeSize_t QueueSet( Queue_t * queue, const void * pval, QueueTypeSize_t num );

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
