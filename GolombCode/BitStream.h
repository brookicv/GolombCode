
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

#ifndef ICV_BIT_STREAM_H
#define ICV_BIT_STREAM_H

#include <bitset>
#include <cstdint>

static const int bit_length = 128;
static const int end_length = 24;

//////////////////////////////////////////////////////
//
// Bits buffer
//	将bytes转化为bit stream时,在该buffer中缓存待处理的bit
// 
/////////////////////////////////////////////////////
struct BitBuffer
{
	std::bitset<bit_length> data; // 使用bitset缓存bit
	int						pos;  // 当前bit的指针
	int						count;// bitset中bit的个数

	// 构造函数
	BitBuffer();

	// 从bitset中取出一个bit
	bool getBit();

	// 从bitset中取出一个byte
	uint8_t getByte();

	// 向bitset中写入一个bit
	void putBit(bool b);

	// 向bitset中写入一个byte
	void putByte(uint8_t b);
};

////////////////////////////////////////////////////
//
// Bytes buffer
//
///////////////////////////////////////////////////
struct ByteBuffer
{
	uint8_t *data;   // Byte数据指针
	uint64_t pos;    // 当期byte的指针
	uint64_t length; // 数据长度

	uint64_t totalLength; // 总的放入到 byte buffer中的字节数

	// 构造函数
	ByteBuffer();

	// 取出一个byte
	uint8_t getByte();

	// 写入一个byte
	void putByte(uint8_t b);

	// 设置byte数组
	void setData(uint8_t *buffer, int len);
};

////////////////////////////////////////////////////////
//
// Bit Output Stream
//	将bit stream转化为byte数组
//  这里也只提供功能，至于byte缓存满的处理放到编码器中处理
//
////////////////////////////////////////////////////////
class BitOutputStream
{
	;
public:
	// 写入一个bit
	void putBit(bool b);

	// 写入多个相同的bit
	void putBit(bool b, int num);

	// 设置数据数组
	void setBuffer(uint8_t *buffer, int len);
	void resetBuffer();

	/*
		判断byte buffer中是可用的bit长度
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
// 将byte数组转换为bitstream，以bit形式读取
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//
// Byte Buffer的状态
//
///////////////////////////////////////////////////
enum BufferState
{
	BUFFER_EMPTY, // buffer empty
	BUFFER_END_SYMBOL, // end_symbol 编码的中止符，已经没有编码的数据
	BUFFER_LACK, // buffer数据不足以完成解码，需要新的buffer
	BUFFER_ENGOUGH // 数据足够，继续解码
};

class BitInputStream
{
public:
	// 读取一个bit
	bool getBit();

	// 设置byte buffer
	void setBuffer(uint8_t *buffer, int len);

	BufferState check();

private:
	BitBuffer bits;
	ByteBuffer bytes;
};

#endif