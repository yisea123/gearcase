// TaskResultQueue.cpp: implementation of the TaskResultQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ModbusQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModbusQueue::CModbusQueue()
{
	InitializeCriticalSection(&m_csQueue);
	m_pHead = NULL;
	m_pTail = NULL;
	m_nSize = 0;
}

CModbusQueue::~CModbusQueue()
{
	while(m_pHead)
	{
		ModbusPacket *p = m_pHead;
		m_pHead = m_pHead->next;
		delete p;
	}
	DeleteCriticalSection(&m_csQueue);
}

void CModbusQueue::Push(ModbusPacket * packet)
{
	if(packet == NULL) return;
	EnterCriticalSection(&m_csQueue);
	if(m_pHead)
	{
		m_pTail->next = packet;
		m_pTail = packet;
	}
	else
	{
		m_pHead = packet;
		m_pTail = m_pHead;
	}
	m_nSize++;
	LeaveCriticalSection(&m_csQueue);
}

ModbusPacket *CModbusQueue::Pop() 
{
	ModbusPacket *packet = NULL;
	EnterCriticalSection(&m_csQueue);
	if(m_pHead) 
	{
		packet = m_pHead;
		m_pHead = m_pHead->next;
		m_nSize--;
	}
	LeaveCriticalSection(&m_csQueue);
	return packet;
}

ModbusPacket *CModbusQueue::Top() 
{
	ModbusPacket *packet = NULL;
	EnterCriticalSection(&m_csQueue);
	if(m_pHead) 
	{
		packet = m_pHead;
	}
	LeaveCriticalSection(&m_csQueue);
	return packet;
}

void CModbusQueue::DeleteAll()
{
	EnterCriticalSection(&m_csQueue);
	while(m_pHead)
	{
		ModbusPacket *p = m_pHead;
		m_pHead = m_pHead->next;
		delete p;
	}
	m_pHead = NULL;
	m_pTail = NULL;
	m_nSize = 0;
	LeaveCriticalSection(&m_csQueue);
}

int CModbusQueue::EditTail(int nSize, byte *data)
{
	int nRet = 1;
	EnterCriticalSection(&m_csQueue);
	if(m_pTail)
	{
		if(nSize && data)
		{
			m_pTail->nRecvSize = nSize;
			memcpy(m_pTail->ucRecvBuf, data, nSize);
		}
		nRet = 0;
		m_pTail->next = NULL;
	}
	LeaveCriticalSection(&m_csQueue);
	return nRet;
}

int CModbusQueue::GetCount()
{
	int iCnt = 0;
	EnterCriticalSection(&m_csQueue);
	iCnt = m_nSize;
	LeaveCriticalSection(&m_csQueue);
	return iCnt;
}