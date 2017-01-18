///////////////////////////////////////////////////////////
// 
// BitBuffer
//		该buffer中缓存待处理的bit，并且只提供基本的get/put功能，更
//		详细的判断处理由调用者处理
//
// ByteBuffer
//		缓存待处理的byte，只提供基本的get/put功能，一些判断由调用者处理
//
// BitOutputStream
//		写入bit stream，并将bits转化为byte写入到byte数组中
//
// BitInputStream
//		将byte数组转化为bit stream，实现对字节数组的按位读取
//
// 作者	:	Brookicv
// 日期	:	2017/1/9 16:20
//
//////////////////////////////////////////////////////////

#include "BitStream.h"
#include <iostream>

///////////////////////////////////////////////////
// 
// Bit Buffer实现
//
//////////////////////////////////////////////////

// 构造函数
BitBuffer::BitBuffer()
{
	data.set();
	data.flip();
	pos = 0;
	count = 0;
}

// 从bitset中取出一个bit
// 返回bitset中最低位的bit
bool BitBuffer::getBit()
{
	bool b = data[0];
	count--;
	data >>= 1;
	return b;
}

// 从bitset中取一个字节
uint8_t BitBuffer::getByte()
{
	uint8_t b = static_cast<uint8_t>(data.to_ulong());
	count -= 8;
	pos -= 8;
	data >>= 8;
	return b;
}

// 向bitset中写入一个bit
void BitBuffer::putBit(bool b)
{
	data.set(pos, b);
	pos++;
	count++;
}

// 向bitset中写入一个byte
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
// Byte Buffer实现
//
//////////////////////////////////////////////////

// 构造函数
ByteBuffer::ByteBuffer()
{
	data = nullptr;
	pos = 0;
	length = 0;
	totalLength = 0;
}

// 取出一个byte
uint8_t ByteBuffer::getByte()
{
	auto b = data[pos];
	pos++;
	return b;
}

// 写入一个byte
void ByteBuffer::putByte(uint8_t b)
{
	data[pos] = b;
	pos++;
	totalLength++;
}

// 设置byte数组
void ByteBuffer::setData(uint8_t *buffer, int len)
{
	data = buffer;
	length = len;
}

///////////////////////////////////////////////////
//
// BitOutputStream实现
//
//////////////////////////////////////////////////

// 设置缓存数组
void BitOutputStream::setBuffer(uint8_t *buffer, int len)
{
	bytes.setData(buffer, len);
}

void BitOutputStream::resetBuffer()
{
	bytes.pos = 0;
}

// 写入一个bit
// 成功，返回true
void BitOutputStream::putBit(bool b)
{
	// bit缓存中是否足够一个byte，并且 byte buffer中有空间
	while(bits.count >= 8 && bytes.pos < bytes.length)
	{
		auto b = bits.getByte();
		bytes.putByte(b);
	}

	// 如果 byte buffer中空间不足，则直接放在bit buffer中。
	bits.putBit(b);
}

// 写入多个相同的bit
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
	
	// 写入8 bytes 0
	for (int i = 0; i < 8; i++)
		bytes.putByte(0x00);

	// 是否是8的倍数
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
// BitInputStream 实现
//
//////////////////////////////////////////////

// 设置数据数组
void BitInputStream::setBuffer(uint8_t *buffer, int len)
{
	bytes.data = buffer;
	bytes.length = len;
}

// 获取一个bit
// get bit时不做判断，在每次解码前都要进行check得到buffer的状态
// 一旦解码开始，就意味着buffer中的数据是足够的
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

// 检测buffer的状态
// 在每次解码开始前调用
BufferState BitInputStream::check()
{
	// buffer中已无数据
	if (bits.count <= 0 && bytes.pos >= bytes.length)
		return BufferState::BUFFER_EMPTY;

	// buffer中还有数据，分为两种情况：不足64bits和有64bits
	auto count = (bytes.length - bytes.pos) * 8 + bits.count;

	// buffer中的数据足够64位
	if (count >= 64)
	{
		// bit buffer中数据就有64bits
		if (bits.count >= 64)
		{
			if (bits.data.none()) // 64 bits 0
				return BufferState::BUFFER_END_SYMBOL; // 编码中止符
			else
				return BufferState::BUFFER_ENGOUGH; // 数据足够继续解码
		}
		// bit buffer中的数据不足64bit
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
	// buffer中数据不足64位，不进行解码，
	// 将byte buffer中的数据取出放在bit buffer后，返回BUFFER_LACK
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

