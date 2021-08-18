#include "Buffer.h"
#include "Tester.h"

TEST("ParseOneByte", "work",
	char* buffer = new char[256];

	int index = 0;
	for (char i = -128; i < 127; i++)
	{
		buffer[index] = i;
		EXPECT("ParseOneByte(" + std::to_string(i) + ") to be " + std::to_string(i), ParseOneByte<unsigned char>(buffer, index) == static_cast<unsigned char>(i));
		index++;
	}
);

BENCH("ParseOneByte", "be fast",
	const char* buffer = "\x25\x2f";

START_BENCH;
	ParseOneByte<unsigned char>(buffer, 1);
STOP_BENCH;
);

TEST("ParseTwoBytes", "work",
	const char* buffer = "\x16\x25\x2f\xff";

EXPECT("ParseTwoBytes(\"\\x16\\x25\") to be 9494", ParseTwoBytes<short>(buffer, 0) == 9494);
EXPECT("ParseTwoBytesLittleEndian(\"\\x16\\x25\") to be 9494", ParseTwoBytesLittleEndian<short>(buffer, 0) == 9494);
EXPECT("ParseTwoBytesBigEndian(\"\\x16\\x25\") to be 5669", ParseTwoBytesBigEndian<short>(buffer, 0) == 5669);

EXPECT("ParseTwoBytes(\"\\x25\\x2F\") to be 12069", ParseTwoBytes<short>(buffer, 1) == 12069);
EXPECT("ParseTwoBytesLittleEndian(\"\\x25\\x2F\") to be 12069", ParseTwoBytesLittleEndian<short>(buffer, 1) == 12069);
EXPECT("ParseTwoBytesBigEndian(\"\\x25\\x2F\") to be 9519", ParseTwoBytesBigEndian<short>(buffer, 1) == 9519);

EXPECT("ParseTwoBytes(\"\\x2F\\xFF\") to be 65327", ParseTwoBytes<unsigned short>(buffer, 2) == 65327);
EXPECT("ParseTwoBytesLittleEndian(\"\\x2F\\xFF\") to be 65327", ParseTwoBytesLittleEndian<unsigned short>(buffer, 2) == 65327);
EXPECT("ParseTwoBytes(\"\\x2F\\xFF\") to be -209", ParseTwoBytes<short>(buffer, 2) == -209);
EXPECT("ParseTwoBytesLittleEndian(\"\\x2F\\xFF\") to be -209", ParseTwoBytesLittleEndian<short>(buffer, 2) == -209);
EXPECT("ParseTwoBytesBigEndian(\"\\x2F\\xFF\") to be 12287", ParseTwoBytesBigEndian<short>(buffer, 2) == 12287);
);

BENCH("ParseTwoBytes", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

START_BENCH;
	ParseTwoBytes<short>(buffer, 1);
STOP_BENCH;
);

TEST("ParseFourBytes", "work",
	const char* buffer = "\x16\x25\x2f\xff";

EXPECT("ParseFourBytes(\"\\x16\\x25\\x2F\\xFF\") to be 4281279766", ParseFourBytes<int>(buffer, 0) == -13687530);
EXPECT("ParseFourBytesLittleEndian(\"\\x16\\x25\\x2F\\xFF\") to be 4281279766", ParseFourBytesLittleEndian<unsigned int>(buffer, 0) == 4281279766);
EXPECT("ParseFourBytesBigEndian(\"\\x16\\x25\\x2F\\xFF\") to be 371535871", ParseFourBytesBigEndian<int>(buffer, 0) == 371535871);
);

BENCH("ParseFourBytes", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

START_BENCH;
ParseFourBytes<int>(buffer, 0);
STOP_BENCH;
);

TEST("ParseString", "work",
	const char* buffer = "hello\0world";

EXPECT("ParseString(\"hello\\0world\", 0) to be \"hello\"", ParseString(buffer, 0) == "hello");
EXPECT("ParseString(\"hello\\0world\", 1) to be \"ello\"", ParseString(buffer, 1) == "ello");
EXPECT("ParseString(\"hello\\0world\", 5) to be \"\"", ParseString(buffer, 5) == "\x00");
EXPECT("ParseString(\"hello\\0world\", 6) to be \"world\"", ParseString(buffer, 6) == "world");
);

BENCH("ParseString   ", "be fast",
	const char* buffer = "hello\0world";

START_BENCH;
ParseString(buffer, 0);
STOP_BENCH;
);

TEST("ParseStringWithSize", "work",
	const char* buffer = "hello\0world";

EXPECT("ParseStringWithSize(\"hello\\0world\", 0, 11) to be \"hello\\0world\"", ParseStringWithSize(buffer, 0, 11) == std::string("hello\0world", 11));
EXPECT("ParseStringWithSize(\"hello\\0world\", 1, 10) to be \"ello\"", ParseStringWithSize(buffer, 1, 10) == std::string("ello\0world", 10));
EXPECT("ParseStringWithSize(\"hello\\0world\", 5, 1) to be \"\"", ParseStringWithSize(buffer, 5, 1) == std::string("\0", 1));
EXPECT("ParseStringWithSize(\"hello\\0world\", 6, 5) to be \"world\"", ParseStringWithSize(buffer, 6, 5) == "world");
EXPECT("ParseStringWithSize(\"hello\\0world\", 6, 5) to be \"world\"", ParseStringWithSize(buffer, 1, 0) == "");
);

BENCH("ParseStringWithSize", "be fast",
	const char* buffer = "hello\0world";

START_BENCH;
ParseStringWithSize(buffer, 0, 11);
STOP_BENCH;
);

int main()
{
	return Tester::Run();
}
