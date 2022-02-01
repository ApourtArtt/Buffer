#include "Buffer.h"
#include "Tester.h"
#include <stdint.h>

int main() { return Tester::Run(); }

// Buffer::From

TEST("Buffer::ParseOneByte", "work",
	for (char i = -128; i < 127; i++)
	{
		EXPECT("Buffer::From::ParseOneByte<char>(" + std::to_string(i) + ") to be " + std::to_string(i), Buffer::From::ParseOneByte<char>(&i) == i);
	}
);

TEST("Buffer::From::ParseTwoBytes", "work",
	EXPECT("Buffer::From::ParseTwoBytes<short>(\"\\x00\\x00\") to be 0", Buffer::From::ParseTwoBytes<short>("\x00\x00") == 0);
	EXPECT("Buffer::From::ParseTwoBytes<short>(\"\\xFF\\xFF\") to be -1", Buffer::From::ParseTwoBytes<short>("\xFF\xFF") == -1);
	EXPECT("Buffer::From::ParseTwoBytes<short>(\"\\x00\\x80\") to be -32768", Buffer::From::ParseTwoBytes<short>("\x00\x80") == -32768);
	EXPECT("Buffer::From::ParseTwoBytes<short>(\"\\xFF\\x7F\") to be 32767", Buffer::From::ParseTwoBytes<short>("\xFF\x7F") == 32767);

	EXPECT("Buffer::From::ParseTwoBytes<unsigned short>(\"\\x00\\x00\") to be 0", Buffer::From::ParseTwoBytes<unsigned short>("\x00\x00") == 0);
	EXPECT("Buffer::From::ParseTwoBytes<unsigned short>(\"\\xFF\\xFF\") to be 65535", Buffer::From::ParseTwoBytes<unsigned short>("\xFF\xFF") == 65535);
	EXPECT("Buffer::From::ParseTwoBytes<unsigned short>(\"\\x00\\x80\") to be 32768", Buffer::From::ParseTwoBytes<unsigned short>("\x00\x80") == 32768);
	EXPECT("Buffer::From::ParseTwoBytes<unsigned short>(\"\\xFF\\x7F\") to be 32767", Buffer::From::ParseTwoBytes<unsigned short>("\xFF\x7F") == 32767);
);

TEST("Buffer::From::ParseFourBytes", "work",
	EXPECT("Buffer::From::ParseFourBytes<int32_t>(\"\\x00\\x00\\x00\\x00\") to be 0", Buffer::From::ParseFourBytes<int32_t>("\x00\x00\x00\x00") == 0);
	EXPECT("Buffer::From::ParseFourBytes<int32_t>(\"\\xFF\\xFF\\xFF\\xFF\") to be -1", Buffer::From::ParseFourBytes<int32_t>("\xFF\xFF\xFF\xFF") == -1);
	EXPECT("Buffer::From::ParseFourBytes<int32_t>(\"\\x00\\x00\\x00\\x80\") to be -2147483648", Buffer::From::ParseFourBytes<int32_t>("\x00\x00\x00\x80") == -2147483648);
	EXPECT("Buffer::From::ParseFourBytes<int32_t>(\"\\xFF\\xFF\\xFF\\x7F\") to be 2147483647", Buffer::From::ParseFourBytes<int32_t>("\xFF\xFF\xFF\x7F") == 2147483647);

	EXPECT("Buffer::From::ParseFourBytes<uint32_t>(\"\\x00\\x00\\x00\\x00\") to be 0", Buffer::From::ParseFourBytes<uint32_t>("\x00\x00\x00\x00") == 0);
	EXPECT("Buffer::From::ParseFourBytes<uint32_t>(\"\\xFF\\xFF\\xFF\\xFF\") to be 4294967295", Buffer::From::ParseFourBytes<uint32_t>("\xFF\xFF\xFF\xFF") == 4294967295);
	EXPECT("Buffer::From::ParseFourBytes<uint32_t>(\"\\x00\\x00\\x00\\x80\") to be 2147483648", Buffer::From::ParseFourBytes<uint32_t>("\x00\x00\x00\x80") == 2147483648);
	EXPECT("Buffer::From::ParseFourBytes<uint32_t>(\"\\xFF\\xFF\\xFF\\x7F\") to be 2147483647", Buffer::From::ParseFourBytes<uint32_t>("\xFF\xFF\xFF\x7F") == 2147483647);
);

TEST("Buffer::From::ParseString", "work",
	const char* buffer = "hello\0world";

	EXPECT("Buffer::From::ParseString(\"hello\\0world\") to be \"hello\"", Buffer::From::ParseString(buffer) == "hello");
	EXPECT("Buffer::From::ParseString(\"hello\\0world\" + 5) to be \"\"", Buffer::From::ParseString(buffer + 5) == "\x00");
	EXPECT("Buffer::From::ParseString(\"hello\\0world\" + 6) to be \"world\"", Buffer::From::ParseString(buffer + 6) == "world");
);

TEST("Buffer::From::ParseStringWithSize", "work",
	const char* buffer = "hello\0world";

	EXPECT("Buffer::From::ParseStringWithSize(\"hello\\0world\", 3) to be \"hel\"", Buffer::From::ParseStringWithSize(buffer, 3) == "hel");
	EXPECT("Buffer::From::ParseStringWithSize(\"hello\\0world\", 5) to be \"hello\"", Buffer::From::ParseStringWithSize(buffer, 5) == "hello");
	EXPECT("Buffer::From::ParseStringWithSize(\"hello\\0world\", 11) to be \"hello\\0world\"", Buffer::From::ParseStringWithSize(buffer, 11) == std::string("hello\0world", 11));
);

TEST("Buffer::From::ParseStringWithSizeOneByte", "work",
	const char* buffer = "\x0Bhello\0world";

	EXPECT("Buffer::From::ParseStringWithSizeOneByte(\"\\x0Bhello\\0world\") to be \"hello\\0world\"", Buffer::From::ParseStringWithSizeOneByte(buffer) == std::string("hello\0world", 11));
);

std::vector<std::string> res1 = { "hello", "world!" };
std::vector<int8_t> res2 = { 0x12, 0x34, 0x56 };
std::vector<std::vector<std::string>> res3 = { { "hello", "world!"}, { "how", " are", "you?" }};
TEST("Buffer::From::ParseVectorWithSizeOneByte", "work", 
	const char* buffer1 = "\x2\x5hello\x6world!";
	EXPECT("Buffer::From::ParseVectorWithSizeOneByte<std::string>(\"\\x2\\x5hello\\x6world!\") to be { \"hello\", \"world!\" }", Buffer::From::ParseVectorWithSizeOneByte<std::string>(buffer1) == res1);

	const char* buffer2 = "\x3\x12\x34\x56";
	EXPECT("Buffer::From::ParseVectorWithSizeOneByte<int8_t>(\"\\x3\\x12\\x34\\x56\") to be { 0x12, 0x34, 0x56 }", Buffer::From::ParseVectorWithSizeOneByte<int8_t>(buffer2) == res2);

	const char* buffer3 = "\x2\x2\x5hello\x6world!\x3\x3how\x4 are\x4you?";
	EXPECT("Buffer::From::ParseVectorWithSizeOneByte<std::vector<std::string>>(\"\\x2\\x2\\x5hello\\x6world!\\x3\\x3how\\x4 are\\x4you?\"", Buffer::From::ParseVectorWithSizeOneByte<std::vector<std::string>>(buffer3) == res3);
);

std::vector<std::tuple<std::string, int8_t>> res4 = { { "hello", 0x12 }, { "world!", 0x34 }};
TEST("Buffer::From::ParseVectorOfTupleWithSizeOneByte", "work",
	const char* buffer1 = "\x2\x5hello\x12\x6world!\x34";
	EXPECT("Buffer::From::ParseVectorOfTupleWithSizeOneByte<std::string, int8_t>(\"\\x2\\x5hello\\x12\\x6world!\\x34\") to be { { \"hello\", 0x12 }, { \"world!\", 0x34 } }", (Buffer::From::ParseVectorOfTupleWithSizeOneByte<std::string, int8_t>)(buffer1) == res4);
);



BENCH("Buffer::From::ParseOneByte\t\t", "be fast",
	const char* buffer = "\x25";

	START_BENCH;
	Buffer::From::ParseOneByte<unsigned char>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseTwoBytes\t\t", "be fast",
	const char* buffer = "\x00\x80";

	START_BENCH;
	Buffer::From::ParseTwoBytes<unsigned char>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseFourBytes\t\t", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

	START_BENCH;
	Buffer::From::ParseFourBytes<int32_t>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseString\t\t", "be fast",
	const char* buffer = "hello";

	START_BENCH;
	Buffer::From::ParseString(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseStringWithSize\t", "be fast",
	const char* buffer = "hello";

	START_BENCH;
	Buffer::From::ParseStringWithSize(buffer, 5);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseStringWithSizeOneByte\t", "be fast",
	const char* buffer = "5hello";

	START_BENCH;
	Buffer::From::ParseStringWithSizeOneByte(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseVectorWithSizeOneByte\t", "be fast",
	const char* buffer1 = "\x2\x5hello\x6world!";

	START_BENCH;
	Buffer::From::ParseVectorWithSizeOneByte<std::string>(buffer1);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseVectorOfTupleWithSizeOneByte\t", "be fast",
	const char* buffer1 = "\x2\x5hello\x12\x6world!\x34";

	START_BENCH;
	(Buffer::From::ParseVectorOfTupleWithSizeOneByte<std::string, int8_t>)(buffer1);
	STOP_BENCH;
);



// Buffer::To


