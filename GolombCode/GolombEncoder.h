
#ifndef GOLOMB_ENCODER_H
#define GOLOMB_ENCODER_H

#include "BitStream.h"

enum GolombType
{
	GOLOMB, 
	GOLOMB_RICE, // Rice编码
	EXP_GOLOMB   // 指数Golomb编码
};

class GolombEncoder
{
public:

	GolombEncoder(uint64_t m_,GolombType golombType = GolombType::GOLOMB);

	/*
		编码，编码前首先计算编码后码元的长度L。如果，缓存长度不足L bits。则将byte填充完后
	，剩余的bit在BitBuffer中缓存，然后返回false，指出缓存不足以存放编码后的码元。
	*/
	bool encode(uint64_t num);

	/*
		结束编码过程。将bit buffer中的数据flush到byte buffer中，然后填充64bits的0，并判断
		totalCodeLength是否是8的倍数，如果不是，则继续补0.
	*/
	bool close();

	/*
		buffer满了以后，处理满的buffer数据（写入文件、发送...），重置buffer，重新使用。
	*/
	void resetBuffer();

	/*
		从外部设定buffer
	*/
	void setBuffer(uint8_t *buffer, uint64_t len);

	// 返回已经编码的字节长度，必须为8的倍数
	uint64_t getToalCodeLength()
	{
		return bitStream.getTotalCodeLength();
	}

private:
	uint64_t rice_golombEncode(uint64_t num);
	uint64_t exp_golombEncode(uint64_t num);

private:

	BitOutputStream bitStream;

	GolombType type; // Golomb编码的类型,common、
	uint64_t m;
	uint64_t k;
};


GolombEncoder::GolombEncoder(uint64_t para, GolombType golombType)
{
	type = golombType;

	switch (golombType)
	{
	case GOLOMB:
		m = para;
		k = log(m) / log(2);
		break;
	case GOLOMB_RICE:
		break;
	case EXP_GOLOMB:
		k = para;
		m = -1;
		break;
	default:
		k = -1;
		m = -1;
		break;
	}

}

void GolombEncoder::setBuffer(uint8_t *buffer, uint64_t len)
{
	bitStream.setBuffer(buffer, len);
}

void GolombEncoder::resetBuffer()
{
	bitStream.resetBuffer();
}

bool GolombEncoder::encode(uint64_t num)
{
	uint64_t len = 0;
	switch (type)
	{
	case GOLOMB:
		break;

	case GOLOMB_RICE:
		len = rice_golombEncode(num);
		break;

	case EXP_GOLOMB:
		len = exp_golombEncode(num);
		break;
	default:
		break;
	}

	return bitStream.freeLength() >= len; // 空间足够，存放编码后的码元则返回true；否则返回false
}

bool GolombEncoder::close()
{
	return bitStream.flush();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// 不同Golomb编码的实现 
//
//////////////////////////////////////////////////////////////////////////////////////////

// Golomb Rice
uint64_t GolombEncoder::rice_golombEncode(uint64_t num)
{
	uint64_t q = num >> k;
	uint64_t r = num & (m - 1);

	auto len = q + 1 + k; // 编码后码元的长度

	/*
	不会判断缓存是否为满，直接向里面放，不足的话缓存到bit buffer中
	*/
	bitStream.putBit(1, q);
	bitStream.putBit(0);

	for (int i = 0; i < k; i++)
	{
		bitStream.putBit(static_cast<bool>(r & 0x01));
		r >>= 1;
	}

	return len; // 返回编码后码元的长度
}

// Exp Golomb
uint64_t GolombEncoder::exp_golombEncode(uint64_t num)
{
	if (k != 0)
		num = num + pow(2, k) - 1;

	// 0阶exp-golom编码
	auto m = static_cast<int>(log(num + 1) / log(2)); // 前缀，也即是组号
	auto info = static_cast<int>(num + 1 - pow(2, m)); // 组内的偏移量

	auto len = 2 * m + 1; // 编码后码元的长度

	// 写入m个0
	bitStream.putBit(0, m);
	// 写入1
	bitStream.putBit(1);

	// 写入info m-1 bits
	for (int i = 0; i < m ; i++)
	{
		bitStream.putBit(static_cast<bool>(info & 0x01));
		info >>= 1;
	}

	return len;
}

#endif