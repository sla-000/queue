/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
// Ryabinin (c) 2012
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#include <string.h>

#include <queue_settings.h>

#include "queue.h"
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Pointer increment
static BOOLX Inc(Queue_t * queue, BOOLX end);
/// Get elements number
static QueueTypeNum_t Size(Queue_t * queue);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Push(Queue_t * queue, const void * pval);
static BOOLX PopFront(Queue_t * queue);

#define Front(queue, pval)					(Peek((queue), (pval), 0))

static BOOLX Peek(Queue_t * queue, void * pval, QueueTypeNum_t num);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Take(const Queue_t * queue);
static void Give(const Queue_t * queue);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX ReadPriv(Queue_t * queue, QueuePrivate_t * priv);
static BOOLX WritePriv(Queue_t * queue, QueuePrivate_t * priv);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static void Init(Queue_t * queue);

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#define INIT(qq) \
	do{ \
		if( ! qq->Private.Inited ) Init(qq); \
	}while(0)

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

QueueTypeNum_t QueueSize(Queue_t * queue)
{
	queue_assert(queue);

	INIT(queue);

	if (!Take(queue))
	{
		return 0;
	}

	QueueTypeNum_t rez;

	rez = Size(queue);

	Give(queue);

	return rez;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

BOOLX QueuePush(Queue_t * queue, const void * pval)
{
	queue_assert(queue);
	queue_assert(pval);

	INIT(queue);

	if( queue->RO )
	{
		return FALSE;
	}

	if (!Take(queue))
	{
		return FALSE;
	}

	BOOLX rez = Push(queue, pval);

	Give(queue);

	return rez;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

BOOLX QueuePopFront(Queue_t * queue)
{
	queue_assert(queue);

	INIT(queue);

	if (!Take(queue))
	{
		return FALSE;
	}

	BOOLX rez = PopFront(queue);

	Give(queue);

	return rez;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

BOOLX QueuePeek(Queue_t * queue, void * pval, QueueTypeNum_t num)
{
	queue_assert(queue);

	INIT(queue);

	if (!Take(queue))
	{
		return FALSE;
	}

	BOOLX rez = Peek(queue, pval, num);

	Give(queue);

	return rez;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

QueueTypeNum_t QueueGet(Queue_t * queue, void * pval, QueueTypeNum_t num)
{
	queue_assert(queue);
	queue_assert(pval);

	INIT(queue);

	if (!Take(queue))
	{
		return FALSE;
	}

	QueueTypeNum_t n = 0;

	for (QueueTypeNum_t q = 0; q < num; ++q)
	{
		u8 * ppp = (u8 *)((QueueTypeAddr_t)pval + queue->Sett->ElemSize * q);

		if (Front(queue, ppp))
		{
			PopFront(queue);
			++n;
		}
		else
		{
			break;
		}
	}

	Give(queue);

	return n;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

QueueTypeNum_t QueueSet(Queue_t * queue, const void * pval, QueueTypeNum_t num)
{
	queue_assert(queue);
	queue_assert(pval);

	INIT(queue);

	if (!Take(queue))
	{
		return FALSE;
	}

	QueueTypeNum_t n = 0;

	for (QueueTypeNum_t q = 0; q < num; ++q)
	{
		if (Push(queue, ((u8 *)pval) + queue->Sett->ElemSize * q))
		{
			++n;
		}
		else
		{
			break;
		}
	}

	Give(queue);

	return n;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

BOOLX QueueClear( Queue_t * queue )
{
	QueuePrivate_t priv;

	priv.End = 0;
	priv.Start = 0;
	priv.Full = FALSE;

	return WritePriv(queue, &priv);
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

void QueueReadOnly( Queue_t * queue, BOOLX enable )
{
	queue->RO = enable;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

//BOOLX QueueBack(Queue_t * queue, void * pval)
//{
//}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

//BOOLX QueuePopBack(Queue_t * queue)
//{
//}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Inc(Queue_t * queue, BOOLX end)
{
	QueueTypeNum_t * val;

	QueuePrivate_t priv;

	if( ! ReadPriv(queue, &priv) )
	{
		return FALSE;
	}

	if (end)
	{
		val = &priv.End;
	}
	else
	{
		val = &priv.Start;
	}

	(*val)++;
	if ((*val) >= queue->Sett->BufElem)
	{
		(*val) = 0;
	}

	if (end)
	{
		if (priv.End == priv.Start)
		{
			priv.Full = TRUE;
		}
	}
	else
	{
		priv.Full = FALSE;
	}

	if( ! WritePriv(queue, &priv) )
	{
		return FALSE;
	}

	return TRUE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static QueueTypeNum_t Size(Queue_t * queue)
{
	QueueTypeNum_t rez;

	QueuePrivate_t priv;

	if( ! ReadPriv(queue, &priv) )
	{
		return 0;
	}

	if (priv.End == priv.Start)
	{
		if (priv.Full)
		{
			rez = queue->Sett->BufElem;
		}
		else
		{
			rez = 0;
		}
	}
	else if (priv.End < priv.Start)
	{
		rez = priv.End + queue->Sett->BufElem - priv.Start;
	}
	else
	{
		rez = priv.End - priv.Start;
	}

	return rez;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Push(Queue_t * queue, const void * pval)
{
	queue_assert(queue->Sett->Iface.Write);

	QueuePrivate_t priv;

	if( ! ReadPriv(queue, &priv) )
	{
		return FALSE;
	}

	if (priv.Full && queue->Sett->OverWrite)
	{
		if( ! Inc(queue, FALSE) )
		{
			return FALSE;
		}
	}

	if( ! ReadPriv(queue, &priv) )
	{
		return FALSE;
	}

	if (! priv.Full)
	{
		if( ! queue->Sett->Iface.Write(queue->Sett->ElemSize * priv.End, pval) )
		{
			return FALSE;
		}

		if( ! Inc(queue, TRUE) )
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX PopFront(Queue_t * queue)
{
	if (Size(queue))
	{
		if( ! Inc(queue, FALSE) )
		{
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Peek(Queue_t * queue, void * pval, QueueTypeNum_t num)
{
	queue_assert(queue->Sett->Iface.Read);

	QueuePrivate_t priv;

	if( ! ReadPriv(queue, &priv) )
		return FALSE;

	QueueTypeNum_t size = Size(queue);

	if( num >= size )
		return FALSE;

	if( num >= queue->Sett->BufElem )
		return FALSE;

	if (size)
	{
		QueueTypeNum_t index = priv.Start + num;

		if( index >= queue->Sett->BufElem )
			index -= queue->Sett->BufElem;

		if( ! queue->Sett->Iface.Read(queue->Sett->ElemSize * index, pval) )
			return FALSE;

		return TRUE;
	}
	else
	{ // пусто
		return FALSE;
	}
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX Take(const Queue_t * queue)
{
	if (queue->Sett->Sem.Take)
	{
		if( queue->Sett->Sem.Take() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static void Give(const Queue_t * queue)
{
	if (queue->Sett->Sem.Give)
	{
		queue->Sett->Sem.Give();
	}
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX ReadPriv(Queue_t * queue, QueuePrivate_t * priv)
{
	queue_assert(queue->Sett->Iface.ReadPrivate);

	BOOLX rez = queue->Sett->Iface.ReadPrivate(priv);

	if( !rez || // не смог считать
		priv->Start >= queue->Sett->BufElem || // некорректное значение Start
		priv->End >= queue->Sett->BufElem || //некорректное значение End
		(priv->Full != TRUE && priv->Full != FALSE)) //некорректное значение Full
	{
		priv->Start = 0;
		priv->End = 0;
		priv->Full = FALSE;

		WritePriv(queue, priv);

		return FALSE;
	}

	return TRUE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX WritePriv(Queue_t * queue, QueuePrivate_t * priv)
{
	queue_assert(queue->Sett->Iface.WritePrivate);

	if( ! queue->Sett->Iface.WritePrivate(priv) )
	{
		return FALSE;
	}

	return TRUE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static void Init(Queue_t * queue)
{
	queue_assert(queue);

	if (!Take(queue))
	{
		return;
	}

	QueuePrivate_t priv;

	ReadPriv(queue, &priv);

	queue->Private.Inited = TRUE;

	Give(queue);
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#ifdef UNIT_TEST
#ifdef QUEUE_TEST

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

typedef struct {
	float fl;
	char * text;
	u8 qq;
	int ii;
} Test_t;

#define TEST_NUM										5
#define TEST_SIZE										sizeof(Test_t)

#define PRIV_SIZE										sizeof(QueuePrivate_t)

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static BOOLX sem;

static BOOLX SemTake(void)
{
	if( sem )
	{
		assert(!"Sem already taken");
		return FALSE;
	}
	else
	{
		sem = TRUE;
		return TRUE;
	}
}
static void SemGive(void)
{
	sem = FALSE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Смещение приватных данных
#define ADDR_PRIVATE								0
/// Смещение данных
#define ADDR_DATA									(ADDR_PRIVATE + PRIV_SIZE)

/// Наша память для хранения
static u8 Buf[PRIV_SIZE + TEST_SIZE * TEST_NUM];

static BOOLX Write(QueueTypeAddr_t offset, const void * pdata)
{
	memcpy( &Buf[ADDR_DATA + offset], pdata, TEST_SIZE );

	return TRUE;
}
static BOOLX Read(QueueTypeAddr_t offset, void * pdata)
{
	memcpy( pdata, &Buf[ADDR_DATA + offset], TEST_SIZE );

	return TRUE;
}

static BOOLX WritePrivate(const QueuePrivate_t * priv)
{
	memcpy( &Buf[ADDR_PRIVATE], priv, PRIV_SIZE );

	return TRUE;
}
static BOOLX ReadPrivate(QueuePrivate_t * priv)
{
	memcpy( priv, &Buf[ADDR_PRIVATE], PRIV_SIZE );

	return TRUE;
}

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

static QueueSett_t Sett =
{
	TEST_NUM,
	TEST_SIZE,
	{ SemTake, SemGive,},
	{ Write, Read, WritePrivate, ReadPrivate, },
	false,
};

static Queue_t Queue =
{
	.Sett = (QueueSett_t *)&Sett,
};

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

int main(void)
{
	Test_t z;

	z.fl = 0;
	z.text = NULL;
	z.qq = 0xAE;

	assert( QueueClear(&Queue) );

	z.fl = 5;
	assert( QueuePush(&Queue, &z) );

	assert( QueueSize(&Queue) == 1 );

	z.fl = 10;
	assert( QueuePush(&Queue, &z) );

	assert( QueueSize(&Queue) == 2 );

	z.fl = 15;
	assert( QueuePush(&Queue, &z) );

	z.fl = 20;
	assert( QueuePush(&Queue, &z) );

	z.fl = 25;
	assert( QueuePush(&Queue, &z) );

	assert( QueueSize(&Queue) == 5 );

	z.fl = 30;
	assert( ! QueuePush(&Queue, &z) );

	assert( QueueSize(&Queue) == 5 );

	z.fl = 35;
	assert( ! QueuePush(&Queue, &z) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 5 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 4 );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 10 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 15 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 20 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 1 );

	z.fl = 50;
	assert( QueuePush(&Queue, &z) );
	z.fl = 55;
	assert( QueuePush(&Queue, &z) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 25 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 50 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 55 );
	assert( QueuePopFront(&Queue) );

	assert( !QueueFront(&Queue, &z) );
	assert( !QueuePopFront(&Queue) );

	//--------------------------------------

	assert( QueueClear(&Queue) );

	static const Test_t qwe[5] = {
		{ 4, "1", 1, 123, },
		{ 8, "2", 2, 234, },
		{ 16, "3", 3, 345, },
		{ 32, "4", 4, 456, },
		{ 64, "5", 5, 567, },
	};
	Test_t zxc[5] = {0};

	assert( QueueSet(&Queue, qwe, 5) == 5 );

	assert( QueueGet(&Queue, zxc, 2) == 2 );
	assert( zxc[0].fl == 4 );
	assert( zxc[1].fl == 8 );

	z.fl = 128;
	assert( QueuePush(&Queue, &z) );

	assert( QueueGet(&Queue, zxc, 3) == 3 );
	assert( zxc[0].fl == 16 );
	assert( zxc[1].fl == 32 );
	assert( zxc[2].fl == 64 );

	assert( QueueGet(&Queue, zxc, 1) == 1 );
	assert( zxc[0].fl == 128 );

	assert( QueueGet(&Queue, zxc, 1) == 0 );

	//--------------------------------------

	Sett.OverWrite = TRUE;

	z.text = NULL;
	z.qq = 0xF1;

	assert( QueueClear(&Queue) );

	z.fl = 1;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 1 );
	z.fl = 2;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 1 );
	z.fl = 3;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 1 );
	z.fl = 4;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 1 );
	z.fl = 5;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 1 );
	z.fl = 6;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 2 );
	z.fl = 7;
	assert( QueuePush(&Queue, &z) );
	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 3 );

	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 4 );

	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 5 );

	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 6 );

	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 7 );

	assert( QueuePopFront(&Queue) );

	assert( ! QueueFront(&Queue, &z) );

	assert( ! QueuePopFront(&Queue) );

	//--------------------------------------

	Sett.OverWrite = TRUE;

	z.text = NULL;
	z.qq = 0xF1;

	assert( QueueClear(&Queue) );

	for(u32 q = 0; q < 1000; ++q)
	{
		z.fl = q;
		assert( QueuePush(&Queue, &z) );
	}

	assert( QueueSize(&Queue) == 5 );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 995 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 996 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 997 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 2 );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 998 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 999 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 0 );

	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 0 );

	//--------------------------------------

	Sett.OverWrite = FALSE;

	z.text = NULL;
	z.qq = 0xF1;

	assert( QueueClear(&Queue) );

	for(u32 q = 100; q < 1000; ++q)
	{
		z.fl = q;
		if( q < 105 )
		{
			assert( QueuePush(&Queue, &z) );
		}
		else
		{
			assert( !QueuePush(&Queue, &z) );
		}
	}

	assert( QueueSize(&Queue) == 5 );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 100 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 101 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 3 );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 102 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 103 );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 104 );
	assert( QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 0 );

	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );

	assert( QueueSize(&Queue) == 0 );

	//--------------------------------------

	Sett.OverWrite = TRUE;

	z.text = NULL;
	z.qq = 0xF1;

	assert( QueueClear(&Queue) );

	assert( !QueueFront(&Queue, &z) );
	assert( !QueueFront(&Queue, &z) );
	assert( !QueueFront(&Queue, &z) );

	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );
	assert( !QueuePopFront(&Queue) );

	//--------------------------------------

	Sett.OverWrite = TRUE;

	z.text = NULL;
	z.qq = 0xF1;

	assert( QueueClear(&Queue) );

	z.fl = 1;
	assert( QueuePush(&Queue, &z) );
	z.fl = 2;
	assert( QueuePush(&Queue, &z) );
	z.fl = 3;
	assert( QueuePush(&Queue, &z) );

	assert( QueuePeek(&Queue, &z, 2) );
	assert( z.fl == 3 );
	assert( !QueuePeek(&Queue, &z, 3) );

	z.fl = 4;
	assert( QueuePush(&Queue, &z) );
	z.fl = 5;
	assert( QueuePush(&Queue, &z) );
	z.fl = 6;
	assert( QueuePush(&Queue, &z) );

	assert( QueuePeek(&Queue, &z, 0) );
	assert( z.fl == 2 );
	assert( QueuePeek(&Queue, &z, 1) );
	assert( z.fl == 3 );
	assert( QueuePeek(&Queue, &z, 2) );
	assert( z.fl == 4 );
	assert( QueuePeek(&Queue, &z, 3) );
	assert( z.fl == 5 );
	assert( QueuePeek(&Queue, &z, 4) );
	assert( z.fl == 6 );

	z.fl = 7;
	assert( QueuePush(&Queue, &z) );
	z.fl = 8;
	assert( QueuePush(&Queue, &z) );

	assert( QueuePopFront(&Queue) );
	assert( QueuePopFront(&Queue) );

	assert( QueueFront(&Queue, &z) );
	assert( z.fl == 6 );

	assert( QueuePeek(&Queue, &z, 1) );
	assert( z.fl == 7 );
	assert( QueuePeek(&Queue, &z, 2) );
	assert( z.fl == 8 );
	assert( !QueuePeek(&Queue, &z, 3) );

	//--------------------------------------

	assert( QueueClear(&Queue) );

	z.ii = 1;
	assert( QueuePush(&Queue, &z) );
	z.ii = 2;
	assert( QueuePush(&Queue, &z) );
	z.ii = 3;
	assert( QueuePush(&Queue, &z) );

	assert( QueueFront(&Queue, &z) );
	assert( z.ii == 1 );

	assert( QueueClear(&Queue) );

	assert( !QueueFront(&Queue, &z) );

	//--------------------------------------

	assert( QueueClear(&Queue) );

	z.ii = 1;
	assert( QueuePush(&Queue, &z) );
	z.ii = 2;
	assert( QueuePush(&Queue, &z) );

	QueueReadOnly(&Queue, TRUE);

	z.ii = 3;
	assert( !QueuePush(&Queue, &z) );

	assert( QueueFront(&Queue, &z) );
	assert( QueuePopFront(&Queue) );
	assert( z.ii == 1 );

	assert( QueueFront(&Queue, &z) );
	assert( QueuePopFront(&Queue) );
	assert( z.ii == 2 );

	assert( !QueueFront(&Queue, &z) );
	assert( !QueuePopFront(&Queue) );

	QueueReadOnly(&Queue, FALSE);

	z.ii = 3;
	assert( QueuePush(&Queue, &z) );

	assert( QueueFront(&Queue, &z) );
	assert( QueuePopFront(&Queue) );
	assert( z.ii == 3 );

	assert( !QueueFront(&Queue, &z) );
	assert( !QueuePopFront(&Queue) );

	//--------------------------------------
	//--------------------------------------
	//--------------------------------------
	//--------------------------------------

	printf("OK\n");
}
#endif
#endif

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
