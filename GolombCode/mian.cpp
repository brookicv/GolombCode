
#include "GolombEncoder.h"
#include "GolombDecoder.h"

#include <iostream>
#include <random>
#include <fstream>

using namespace std;

static const int length = 50000;
static const int  max = 20;

int main()
{
	uint8_t buffer[1024];
	int m = 8;

	
	///////////////////////////////////////////////////////
	//
	// 随机生成测试数据
	//
	//////////////////////////////////////////////////////
	//ofstream codeOfs;
	//codeOfs.open("code.txt");

	//uint64_t *nums = new uint64_t[length];

	//std::random_device rd;
	//for (int n = 0; n < length; ++n)
	//{
	//	nums[n] = rd() % max;
	//	codeOfs << nums[n] << endl;
	//}
	//codeOfs.close();
		
	////////////////////////////////////////////////////
	//
	// Encode
	//
	///////////////////////////////////////////////////
	//GolombEncoder encoder(m);
	//encoder.setBuffer(buffer, 1024);

	//ofstream ofs;
	//ofs.open("golomb.gl", ios::binary);

	//for (int i = 0; i < length; i++)
	//{
	//	auto b = encoder.encode(nums[i]);
	//	if (!b)
	//	{
	//		cout << "Lack of buffer space,write the data to file" << endl;
	//		cout << "reset buffer" << endl;
	//		ofs.write((const char*)buffer, encoder.getToalCodeLength());

	//		encoder.resetBuffer();
	//		break;
	//	}
	//}
	//encoder.close();

	//ofs.write((const char*)buffer, encoder.getToalCodeLength());

	//ofs.close();

	//cout << "Golomb finished coding" << endl;

	////////////////////////////////////////////////////////////////////
	//
	// Decode
	//
	////////////////////////////////////////////////////////////////////
	/*ifstream ifs;
	ifs.open("golomb.gl", ios::binary);

	memset(buffer, 0, 1024);

	ifs.read((char*)buffer, 664);

	ofstream encodeOfs;
	encodeOfs.open("encode.txt");

	GolombDecoder decoder(m);
	decoder.setBuffer(buffer, 1024);
	uint64_t num;
	auto state = decoder.decode(num);

	int index = 0;
	while (state != BufferState::BUFFER_END_SYMBOL)
	{
		encodeOfs << num << endl;
		state = decoder.decode(num);

		index++;
	}

	ifs.close();
	encodeOfs.close();

	cout << "decode finished" << endl;*/

	// 2阶exp_golomb

	GolombEncoder expGolomb(0, GolombType::EXP_GOLOMB);
	expGolomb.setBuffer(buffer, 1024);

	expGolomb.encode(5);
	expGolomb.encode(9);
	expGolomb.encode(16);
	expGolomb.encode(27);
	
	// 结束编码
	expGolomb.close();

	//解码
	GolombDecoder expDecoder(0, GolombType::EXP_GOLOMB);
	expDecoder.setBuffer(buffer, 1024);

	uint64_t a;
	expDecoder.decode(a);
	cout << a << endl;

	expDecoder.decode(a);
	cout << a << endl;

	expDecoder.decode(a);
	cout << a << endl;

	expDecoder.decode(a);
	cout << a << endl;

	getchar();
	return 0;
}