
///////////////////////////////////////////////////////////
// 
// BitBuffer
//		��buffer�л���������bit������ֻ�ṩ������get/put���ܣ���
//		��ϸ���жϴ����ɵ����ߴ���
//
// ByteBuffer
//		����������byte��ֻ�ṩ������get/put���ܣ�һЩ�ж��ɵ����ߴ���
//
// BitOutputStream
//		д��bit stream������bitsת��Ϊbyteд�뵽byte������
//
// BitInputStream
//		��byte����ת��Ϊbit stream��ʵ�ֶ��ֽ�����İ�λ��ȡ
//
// ����	:	Brookicv
// ����	:	2017/1/9 16:20
//
//////////////////////////////////////////////////////////

#ifndef ICV_BIT_STREAM_H
#define ICV_BIT_STREAM_H

#include <bitset>
#include <cstdint>

static const int bit_length = 128;
static const int end_length = 24;

//////////////////////////////////////////////////////
//
// Bits buffer
//	��bytesת��Ϊbit streamʱ,�ڸ�buffer�л���������bit
// 
/////////////////////////////////////////////////////
struct BitBuffer
{
	std::bitset<bit_length> data; // ʹ��bitset����bit
	int						pos;  // ��ǰbit��ָ��
	int						count;// bitset��bit�ĸ���

	// ���캯��
	BitBuffer();

	// ��bitset��ȡ��һ��bit
	bool getBit();

	// ��bitset��ȡ��һ��byte
	uint8_t getByte();

	// ��bitset��д��һ��bit
	void putBit(bool b);

	// ��bitset��д��һ��byte
	void putByte(uint8_t b);
};

////////////////////////////////////////////////////
//
// Bytes buffer
//
///////////////////////////////////////////////////
struct ByteBuffer
{
	uint8_t *data;   // Byte����ָ��
	uint64_t pos;    // ����byte��ָ��
	uint64_t length; // ���ݳ���

	uint64_t totalLength; // �ܵķ��뵽 byte buffer�е��ֽ���

	// ���캯��
	ByteBuffer();

	// ȡ��һ��byte
	uint8_t getByte();

	// д��һ��byte
	void putByte(uint8_t b);

	// ����byte����
	void setData(uint8_t *buffer, int len);
};

////////////////////////////////////////////////////////
//
// Bit Output Stream
//	��bit streamת��Ϊbyte����
//  ����Ҳֻ�ṩ���ܣ�����byte�������Ĵ���ŵ��������д���
//
////////////////////////////////////////////////////////
class BitOutputStream
{
	;
public:
	// д��һ��bit
	void putBit(bool b);

	// д������ͬ��bit
	void putBit(bool b, int num);

	// ������������
	void setBuffer(uint8_t *buffer, int len);
	void resetBuffer();

	/*
		�ж�byte buffer���ǿ��õ�bit����
	*/
	uint64_t freeLength();

	// Flush bit buffer to byte buffer
	bool flush();

	uint64_t getTotalCodeLength()
	{
		return bytes.pos;
	}

private:
	BitBuffer bits;
	ByteBuffer bytes;
};



//////////////////////////////////////////////////////////
//
// Bit Input Stream
// ��byte����ת��Ϊbitstream����bit��ʽ��ȡ
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//
// Byte Buffer��״̬
//
///////////////////////////////////////////////////
enum BufferState
{
	BUFFER_EMPTY, // buffer empty
	BUFFER_END_SYMBOL, // end_symbol �������ֹ�����Ѿ�û�б��������
	BUFFER_LACK, // buffer���ݲ�������ɽ��룬��Ҫ�µ�buffer
	BUFFER_ENGOUGH // �����㹻����������
};

class BitInputStream
{
public:
	// ��ȡһ��bit
	bool getBit();

	// ����byte buffer
	void setBuffer(uint8_t *buffer, int len);

	BufferState check();

private:
	BitBuffer bits;
	ByteBuffer bytes;
};

#endif