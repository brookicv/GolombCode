//////////////////////////////////////////////////////////
//
// GolombCodec.h
// ʵ��Golomb���빦��
//		Golomb
//		Golomb Rice
//		Exponent Golomb
//		Sub Exponent Golomb
// ���ߣ�Brookicv
// ���ڣ�2017/1/10 14:56
//
//////////////////////////////////////////////////////////

#ifndef ICV_GOLOMB_DECODER_H
#define ICV_GOLOMB_DECODER_H

#include "BitStream.h"
#include  "GolombEncoder.h"

//////////////////////////////////////////////////////////////
//
// Golomb ����
//
////////////////////////////////////////////////////////////
class GolombDecoder
{
public:

	GolombDecoder(uint64_t para,GolombType type = GolombType::GOLOMB);

	void setBuffer(uint8_t *buffer, int len);

	BufferState decode(uint64_t& num);


private:
	
	uint64_t rice_golombDecode();
	uint64_t exp_golombDecode();

private:
	BitInputStream bitStream;
	uint64_t k;
	uint64_t m;

	GolombType golombType;
};


GolombDecoder::GolombDecoder(uint64_t para, GolombType type)
{
	golombType = type;
	switch (type)
	{
	case GOLOMB:
		break;

	case GOLOMB_RICE:
		m = para;
		k = static_cast<uint64_t>(log(m) / log(2));
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

void GolombDecoder::setBuffer(uint8_t *buffer, int len)
{
	bitStream.setBuffer(buffer, len);
}

/////////////////////////////////////////////////////////
//
// ����
// ��ÿ�ν���ǰ��Ҫcheck buffer��״̬�����ݲ�ͬ��״̬���������Ƿ����
//
///////////////////////////////////////////////////////
BufferState GolombDecoder::decode(uint64_t& num)
{
	auto state = bitStream.check();

	// buffer�������㹻�����н���
	if (state == BufferState::BUFFER_ENGOUGH)
	{
		switch (golombType)
		{
		case GOLOMB:
			break;

		case GOLOMB_RICE:
			num = rice_golombDecode();
			break;

		case EXP_GOLOMB:
			num = exp_golombDecode();
			break;
		default:
			break;
		}
	}

	return state;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// ʵ�ֲ�ͬ���͵�Golomb����
//
/////////////////////////////////////////////////////////////////////////////////////////

// Rice Golomb
uint64_t GolombDecoder::rice_golombDecode()
{
	bool b;
	uint64_t unary = 0;
	b = bitStream.getBit();
	while (b)
	{
		unary++;
		b = bitStream.getBit();
	}

	std::bitset<64> bits;
	bits.reset();
	for (int i = 0; i < k; i++)
	{
		b = bitStream.getBit();
		bits.set(i, b);
	}

	uint64_t num = unary * m + bits.to_ulong();

	return num;
}

// Exp Golomb
uint64_t GolombDecoder::exp_golombDecode()
{
	bool b;
	uint64_t groupID = 0;

	// group id ��0�ĸ�����ֱ��1
	b = bitStream.getBit();
	while (!b)
	{
		groupID++;
		b = bitStream.getBit();
	}

	// �������ڵ�ƫ����
	// ��ȡgroupID-1��bit
	std::bitset<64> bits;
	bits.reset();
	for (int i = 0; i < groupID; i++)
	{
		b = bitStream.getBit();
		bits.set(i, b);
	}

	uint64_t num = pow(2, groupID) - 1 + bits.to_ulong();
	//�ж��Ƿ���0��exp���������ֱ�ӷ��ؽ��
	if (k != 0)
		num = num - pow(2, k) + 1;
	return num;
}

#endif