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

#include "BitStream.h"
#include <iostream>

///////////////////////////////////////////////////
// 
// Bit Bufferʵ��
//
//////////////////////////////////////////////////

// ���캯��
BitBuffer::BitBuffer()
{
	data.set();
	data.flip();
	pos = 0;
	count = 0;
}

// ��bitset��ȡ��һ��bit
// ����bitset�����λ��bit
bool BitBuffer::getBit()
{
	bool b = data[0];
	count--;
	data >>= 1;
	return b;
}

// ��bitset��ȡһ���ֽ�
uint8_t BitBuffer::getByte()
{
	uint8_t b = static_cast<uint8_t>(data.to_ulong());
	count -= 8;
	pos -= 8;
	data >>= 8;
	return b;
}

// ��bitset��д��һ��bit
void BitBuffer::putBit(bool b)
{
	data.set(pos, b);
	pos++;
	count++;
}

// ��bitset��д��һ��byte
void BitBuffer::putByte(uint8_t b)
{
	/*data <<= 8;
	data |= b;
	pos += 8;
	count += 8;*/

	uint64_t tmp = (b << count);
	data |= tmp;
	count += 8;
	pos += 8;
}

///////////////////////////////////////////////////
// 
// Byte Bufferʵ��
//
//////////////////////////////////////////////////

// ���캯��
ByteBuffer::ByteBuffer()
{
	data = nullptr;
	pos = 0;
	length = 0;
	totalLength = 0;
}

// ȡ��һ��byte
uint8_t ByteBuffer::getByte()
{
	auto b = data[pos];
	pos++;
	return b;
}

// д��һ��byte
void ByteBuffer::putByte(uint8_t b)
{
	data[pos] = b;
	pos++;
	totalLength++;
}

// ����byte����
void ByteBuffer::setData(uint8_t *buffer, int len)
{
	data = buffer;
	length = len;
}

///////////////////////////////////////////////////
//
// BitOutputStreamʵ��
//
//////////////////////////////////////////////////

// ���û�������
void BitOutputStream::setBuffer(uint8_t *buffer, int len)
{
	bytes.setData(buffer, len);
}

void BitOutputStream::resetBuffer()
{
	bytes.pos = 0;
}

// д��һ��bit
// �ɹ�������true
void BitOutputStream::putBit(bool b)
{
	// bit�������Ƿ��㹻һ��byte������ byte buffer���пռ�
	while(bits.count >= 8 && bytes.pos < bytes.length)
	{
		auto b = bits.getByte();
		bytes.putByte(b);
	}

	// ��� byte buffer�пռ䲻�㣬��ֱ�ӷ���bit buffer�С�
	bits.putBit(b);
}

// д������ͬ��bit
void BitOutputStream::putBit(bool b, int num)
{
	for (int i = 0; i < num; i++)
		putBit(b);
}

uint64_t BitOutputStream::freeLength()
{
	return (bytes.length - bytes.pos) * 8 - bits.count;
}

bool BitOutputStream::flush()
{
	if (bits.count > 0)
	{
		auto b = bits.getByte();
		bytes.putByte(b);
	}
	
	// д��8 bytes 0
	for (int i = 0; i < 8; i++)
		bytes.putByte(0x00);

	// �Ƿ���8�ı���
	auto remainder = bytes.totalLength % 8;
	if (remainder != 0)
	{
		for (int i = remainder; i < 8; i++)
			bytes.putByte(0x00);
	}
	std::cout << bytes.totalLength << std::endl;
	return true;
}

///////////////////////////////////////////////
//
// BitInputStream ʵ��
//
//////////////////////////////////////////////

// ������������
void BitInputStream::setBuffer(uint8_t *buffer, int len)
{
	bytes.data = buffer;
	bytes.length = len;
}

// ��ȡһ��bit
// get bitʱ�����жϣ���ÿ�ν���ǰ��Ҫ����check�õ�buffer��״̬
// һ�����뿪ʼ������ζ��buffer�е��������㹻��
bool BitInputStream::getBit()
{
	bool b;
	if (bits.count <= 0)
	{
		auto c = bytes.getByte();
		bits.putByte(c);
	}

	b = bits.getBit();

	return b;
}

// ���buffer��״̬
// ��ÿ�ν��뿪ʼǰ����
BufferState BitInputStream::check()
{
	// buffer����������
	if (bits.count <= 0 && bytes.pos >= bytes.length)
		return BufferState::BUFFER_EMPTY;

	// buffer�л������ݣ���Ϊ�������������64bits����64bits
	auto count = (bytes.length - bytes.pos) * 8 + bits.count;

	// buffer�е������㹻64λ
	if (count >= 64)
	{
		// bit buffer�����ݾ���64bits
		if (bits.count >= 64)
		{
			if (bits.data.none()) // 64 bits 0
				return BufferState::BUFFER_END_SYMBOL; // ������ֹ��
			else
				return BufferState::BUFFER_ENGOUGH; // �����㹻��������
		}
		// bit buffer�е����ݲ���64bit
		else
		{

			if (!bits.data.none())
				return BufferState::BUFFER_ENGOUGH;

			int count = ((64 - bits.count) / 8 + 1);
			int index = 0;
			while (index < count)
			{
				auto b = bytes.data[bytes.pos + index];
				index++;
				if (b != 0)
					return BufferState::BUFFER_ENGOUGH;
			}

			return BUFFER_END_SYMBOL;
		}
	}
	// buffer�����ݲ���64λ�������н��룬
	// ��byte buffer�е�����ȡ������bit buffer�󣬷���BUFFER_LACK
	else
	{
		while (bytes.pos < bytes.length)
		{
			auto b = bytes.getByte();
			bits.putByte(b);
		}
		return BufferState::BUFFER_LACK;
	}
}

