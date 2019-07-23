#ifndef _MODBUS_QUEUE_H_
#define _MODBUS_QUEUE_H_

//size = sizeof(addr) + sizeof(code) + sizeof(data) + sizeof(crc)
struct ModbusPacket
{
	int nSize;
	byte data[256];
	DWORD dwStart;
	DWORD dwDelay;
	int nRecvSize;
	byte ucRecvBuf[256];
	ModbusPacket *next;
};

class CModbusQueue  
{
public:
	CModbusQueue();
	virtual ~CModbusQueue();
	
public:
	void Push(ModbusPacket * packet);
	ModbusPacket *Pop();
	ModbusPacket *Top();
	void DeleteAll();
	int EditTail(int nSize, byte *data);
	int GetCount();
	
private:
	CRITICAL_SECTION m_csQueue;
	ModbusPacket *m_pHead;
	ModbusPacket *m_pTail;
	int m_nSize;
};

#endif // !defined(_MODBUS_QUEUE_H_)
