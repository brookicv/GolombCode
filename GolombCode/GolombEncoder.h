
#ifndef GOLOMB_ENCODER_H
#define GOLOMB_ENCODER_H

#include "BitStream.h"

enum GolombType
{
	GOLOMB, 
	GOLOMB_RICE, // Rice����
	EXP_GOLOMB   // ָ��Golomb����
};

class GolombEncoder
{
public:

	GolombEncoder(uint64_t m_,GolombType golombType = GolombType::GOLOMB);

	/*
		���룬����ǰ���ȼ���������Ԫ�ĳ���L����������泤�Ȳ���L bits����byte������
	��ʣ���bit��BitBuffer�л��棬Ȼ�󷵻�false��ָ�����治���Դ�ű�������Ԫ��
	*/
	bool encode(uint64_t num);

	/*
		����������̡���bit buffer�е�����flush��byte buffer�У�Ȼ�����64bits��0�����ж�
		totalCodeLength�Ƿ���8�ı�����������ǣ��������0.
	*/
	bool close();

	/*
		buffer�����Ժ󣬴�������buffer���ݣ�д���ļ�������...��������buffer������ʹ�á�
	*/
	void resetBuffer();

	/*
		���ⲿ�趨buffer
	*/
	void setBuffer(uint8_t *buffer, uint64_t len);

	// �����Ѿ�������ֽڳ��ȣ�����Ϊ8�ı���
	uint64_t getToalCodeLength()
	{
		return bitStream.getTotalCodeLength();
	}

private:
	uint64_t rice_golombEncode(uint64_t num);
	uint64_t exp_golombEncode(uint64_t num);

private:

	BitOutputStream bitStream;

	GolombType type; // Golomb���������,common��
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

	return bitStream.freeLength() >= len; // �ռ��㹻����ű�������Ԫ�򷵻�true�����򷵻�false
}

bool GolombEncoder::close()
{
	return bitStream.flush();
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// ��ͬGolomb�����ʵ�� 
//
//////////////////////////////////////////////////////////////////////////////////////////

// Golomb Rice
uint64_t GolombEncoder::rice_golombEncode(uint64_t num)
{
	uint64_t q = num >> k;
	uint64_t r = num & (m - 1);

	auto len = q + 1 + k; // �������Ԫ�ĳ���

	/*
	�����жϻ����Ƿ�Ϊ����ֱ��������ţ�����Ļ����浽bit buffer��
	*/
	bitStream.putBit(1, q);
	bitStream.putBit(0);

	for (int i = 0; i < k; i++)
	{
		bitStream.putBit(static_cast<bool>(r & 0x01));
		r >>= 1;
	}

	return len; // ���ر������Ԫ�ĳ���
}

// Exp Golomb
uint64_t GolombEncoder::exp_golombEncode(uint64_t num)
{
	if (k != 0)
		num = num + pow(2, k) - 1;

	// 0��exp-golom����
	auto m = static_cast<int>(log(num + 1) / log(2)); // ǰ׺��Ҳ�������
	auto info = static_cast<int>(num + 1 - pow(2, m)); // ���ڵ�ƫ����

	auto len = 2 * m + 1; // �������Ԫ�ĳ���

	// д��m��0
	bitStream.putBit(0, m);
	// д��1
	bitStream.putBit(1);

	// д��info m-1 bits
	for (int i = 0; i < m ; i++)
	{
		bitStream.putBit(static_cast<bool>(info & 0x01));
		info >>= 1;
	}

	return len;
}

#endif