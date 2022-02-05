#include "Buffer.h"
#include "Tester.h"
#include <stdint.h>

int main() { return Tester::Run(); }

// Test data
std::vector<std::string> res1 = { "hello", "world!" };
std::vector<int8_t> res2 = { 0x12, 0x34, 0x56 };
std::vector<std::vector<std::string>> res3 = { { "hello", "world!"}, { "how", " are", "you?" } };
std::vector<std::tuple<std::string, int8_t>> res4 = { { "hello", 0x12 }, { "world!", 0x34 } };

std::pair<char*, size_t> res1Pair = { (char*)"\x2\x5hello\x6world!", 14 };
std::pair<char*, size_t> res2Pair = { (char*)"\x3\x12\x34\x56", 4 };
std::pair<char*, size_t> res3Pair = { (char*)"\x2\x2\x5hello\x6world!\x3\x3how\x4 are\x4you?", 30 };
std::pair<char*, size_t> res4Pair = { (char*)"\x2\x5hello\x12\x6world!\x34", 16 };


// Buffer::From
TEST("Buffer::From::Parse", "work",
	for (char i = -128; i < 127; i++)
		EXPECT("Buffer::From::Parse<char>(" + std::to_string(i) + ") to be " + std::to_string(i), Buffer::From::Parse<char>(&i) == i);

	EXPECT("Buffer::From::Parse<short>(\"\\x00\\x00\") to be 0", Buffer::From::Parse<short>("\x00\x00") == 0);
	EXPECT("Buffer::From::Parse<short>(\"\\xFF\\xFF\") to be -1", Buffer::From::Parse<short>("\xFF\xFF") == -1);
	EXPECT("Buffer::From::Parse<short>(\"\\x00\\x80\") to be -32768", Buffer::From::Parse<short>("\x00\x80") == -32768);
	EXPECT("Buffer::From::Parse<short>(\"\\xFF\\x7F\") to be 32767", Buffer::From::Parse<short>("\xFF\x7F") == 32767);

	EXPECT("Buffer::From::Parse<unsigned short>(\"\\x00\\x00\") to be 0", Buffer::From::Parse<unsigned short>("\x00\x00") == 0);
	EXPECT("Buffer::From::Parse<unsigned short>(\"\\xFF\\xFF\") to be 65535", Buffer::From::Parse<unsigned short>("\xFF\xFF") == 65535);
	EXPECT("Buffer::From::Parse<unsigned short>(\"\\x00\\x80\") to be 32768", Buffer::From::Parse<unsigned short>("\x00\x80") == 32768);
	EXPECT("Buffer::From::Parse<unsigned short>(\"\\xFF\\x7F\") to be 32767", Buffer::From::Parse<unsigned short>("\xFF\x7F") == 32767);

	EXPECT("Buffer::From::Parse<int32_t>(\"\\x00\\x00\\x00\\x00\") to be 0", Buffer::From::Parse<int32_t>("\x00\x00\x00\x00") == 0);
	EXPECT("Buffer::From::Parse<int32_t>(\"\\xFF\\xFF\\xFF\\xFF\") to be -1", Buffer::From::Parse<int32_t>("\xFF\xFF\xFF\xFF") == -1);
	EXPECT("Buffer::From::Parse<int32_t>(\"\\x00\\x00\\x00\\x80\") to be -2147483648", Buffer::From::Parse<int32_t>("\x00\x00\x00\x80") == -2147483648);
	EXPECT("Buffer::From::Parse<int32_t>(\"\\xFF\\xFF\\xFF\\x7F\") to be 2147483647", Buffer::From::Parse<int32_t>("\xFF\xFF\xFF\x7F") == 2147483647);

	EXPECT("Buffer::From::Parse<uint32_t>(\"\\x00\\x00\\x00\\x00\") to be 0", Buffer::From::Parse<uint32_t>("\x00\x00\x00\x00") == 0);
	EXPECT("Buffer::From::Parse<uint32_t>(\"\\xFF\\xFF\\xFF\\xFF\") to be 4294967295", Buffer::From::Parse<uint32_t>("\xFF\xFF\xFF\xFF") == 4294967295);
	EXPECT("Buffer::From::Parse<uint32_t>(\"\\x00\\x00\\x00\\x80\") to be 2147483648", Buffer::From::Parse<uint32_t>("\x00\x00\x00\x80") == 2147483648);
	EXPECT("Buffer::From::Parse<uint32_t>(\"\\xFF\\xFF\\xFF\\x7F\") to be 2147483647", Buffer::From::Parse<uint32_t>("\xFF\xFF\xFF\x7F") == 2147483647);

	EXPECT("Buffer::From::Parse(\"\\x0Bhello\\0world\") to be \"hello\\0world\"", Buffer::From::Parse<std::string>("\x0Bhello\0world") == std::string("hello\0world", 11));
);

TEST("Buffer::From::ParseVector", "work", 
	const char* buffer1 = "\x2\x5hello\x6world!";
	EXPECT("Buffer::From::ParseVector<std::string>(\"\\x2\\x5hello\\x6world!\") to be { \"hello\", \"world!\" }", Buffer::From::ParseVector<std::string>(buffer1) == res1);

	const char* buffer2 = "\x3\x12\x34\x56";
	EXPECT("Buffer::From::ParseVector<int8_t>(\"\\x3\\x12\\x34\\x56\") to be { 0x12, 0x34, 0x56 }", Buffer::From::ParseVector<int8_t>(buffer2) == res2);

	const char* buffer3 = "\x2\x2\x5hello\x6world!\x3\x3how\x4 are\x4you?";
	EXPECT("Buffer::From::ParseVector<std::vector<std::string>>(\"\\x2\\x2\\x5hello\\x6world!\\x3\\x3how\\x4 are\\x4you?\"", Buffer::From::ParseVector<std::vector<std::string>>(buffer3) == res3);
);

TEST("Buffer::From::ParseVectorOfTuple", "work",
	const char* buffer1 = "\x2\x5hello\x12\x6world!\x34";
	EXPECT("Buffer::From::ParseVectorOfTuple<std::string, int8_t>(\"\\x2\\x5hello\\x12\\x6world!\\x34\") to be { { \"hello\", 0x12 }, { \"world!\", 0x34 } }", (Buffer::From::ParseVectorOfTuple<std::string, int8_t>)(buffer1) == res4);
);


BENCH("Buffer::From::Parse<int8_t>\t\t\t", "be fast",
	const char* buffer = "\x16";

	START_BENCH;
	Buffer::From::Parse<int8_t>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::Parse<int16_t>\t\t\t", "be fast",
	const char* buffer = "\x16\x25";

	START_BENCH;
	Buffer::From::Parse<int16_t>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::Parse<int32_t>\t\t\t", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

	START_BENCH;
	Buffer::From::Parse<int32_t>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::Parse<std::string>\t\t\t", "be fast",
	const char* buffer = "hello";

	START_BENCH;
	Buffer::From::Parse<std::string>(buffer);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseVector\t\t\t", "be fast",
	START_BENCH;
	Buffer::From::ParseVector<std::string>(res1Pair.first);
	STOP_BENCH;
);

BENCH("Buffer::From::ParseVectorOfTuple\t\t\t", "be fast",
	START_BENCH;
	(Buffer::From::ParseVectorOfTuple<std::string, int8_t>)(res4Pair.first);
	STOP_BENCH;
);


// Buffer::To
TEST("Buffer::To::Parse", "work",
	for (char i = -128; i < 127; i++)
		EXPECT("Buffer::To::Parse(" + std::to_string(i) + ") to be " + std::to_string(i),Buffer::To::Parse(i)[0] == i);

	EXPECT("Buffer::To::Parse(0) to be \"\\x00\\x00\"", std::string(Buffer::To::Parse((short)0), 2) == std::string("\x00\x00", 2));
	EXPECT("Buffer::To::Parse(-1) to be \"\\xFF\\xFF\"", std::string(Buffer::To::Parse((short)-1), 2) == std::string("\xFF\xFF", 2));
	EXPECT("Buffer::To::Parse(-32768) to be \"\\x00\\x80\"", std::string(Buffer::To::Parse((short)-32768), 2) == std::string("\x00\x80", 2));
	EXPECT("Buffer::To::Parse(32767) to be \"\\xFF\\x7F\"", std::string(Buffer::To::Parse((short)32767), 2) == std::string("\xFF\x7F", 2));

	EXPECT("Buffer::To::Parse(0) to be \"\\x00\\x00\"", std::string(Buffer::To::Parse((unsigned short)0), 2) == std::string("\x00\x00", 2));
	EXPECT("Buffer::To::Parse(65535) to be \"\\xFF\\xFF\"", std::string(Buffer::To::Parse((unsigned short)65535), 2) == std::string("\xFF\xFF", 2));
	EXPECT("Buffer::To::Parse(32768) to be \"\\x00\\x80\"", std::string(Buffer::To::Parse((unsigned short)32768), 2) == std::string("\x00\x80", 2));
	EXPECT("Buffer::To::Parse(32767) to be \"\\xFF\\x7F\"", std::string(Buffer::To::Parse((unsigned short)32767), 2) == std::string("\xFF\x7F", 2));

	EXPECT("Buffer::To::Parse(0) to be \"\\x00\\x00\\x00\\x00\"", std::string(Buffer::To::Parse((uint32_t)0), 4) == std::string("\x00\x00\x00\x00", 4));
	EXPECT("Buffer::To::Parse(-1) to be \"\\xFF\\xFF\\xFF\\xFF\"", std::string(Buffer::To::Parse((uint32_t)-1), 4) == std::string("\xFF\xFF\xFF\xFF", 4));
	EXPECT("Buffer::To::Parse(-2147483648) to be \"\\x00\\x00\\x00\\x80\"", std::string(Buffer::To::Parse((uint32_t)-2147483648), 4) == std::string("\x00\x00\x00\x80", 4));
	EXPECT("Buffer::To::Parse(2147483647) to be \"\\xFF\\xFF\\xFF\\x7F\"", std::string(Buffer::To::Parse((uint32_t)2147483647), 4) == std::string("\xFF\xFF\xFF\x7F", 4));

	EXPECT("Buffer::To::Parse(0) to be \"\\x00\\x00\\x00\\x00\"", std::string(Buffer::To::Parse((uint32_t)0), 4) == std::string("\x00\x00\x00\x00", 4));
	EXPECT("Buffer::To::Parse(4294967295) to be \"\\xFF\\xFF\\xFF\\xFF\"", std::string(Buffer::To::Parse((uint32_t)4294967295), 4) == std::string("\xFF\xFF\xFF\xFF", 4));
	EXPECT("Buffer::To::Parse(2147483648) to be \"\\x00\\x00\\x00\\x80\"", std::string(Buffer::To::Parse((uint32_t)2147483648), 4) == std::string("\x00\x00\x00\x80", 4));
	EXPECT("Buffer::To::Parse(2147483647) to be \"\\xFF\\xFF\\xFF\\x7F\"", std::string(Buffer::To::Parse((uint32_t)2147483647), 4) == std::string("\xFF\xFF\xFF\x7F", 4));
);

TEST("Buffer::To::ParseVector", "work",
	auto vec1 = Buffer::To::ParseVector(res1);
	auto current1 = std::string(vec1.first, vec1.second);
	auto expected1 = std::string(res1Pair.first, res1Pair.second);
	EXPECT("Buffer::To::ParseVector({ \"hello\", \"world!\" }) to be \"\\x2\\x5hello\\x6world!\"", current1 == expected1);

	auto vec2 = Buffer::To::ParseVector(res2);
	auto current2 = std::string(vec2.first, vec2.second);
	auto expected2 = std::string(res2Pair.first, res2Pair.second);
	EXPECT("Buffer::To::ParseVector({ 0x12, 0x34, 0x56 }) to be \"\\x3\\x12\\x34\\x56\"", current2 == expected2);


	auto vec3 = Buffer::To::ParseVector(res3);
	auto current3 = std::string(vec3.first, vec3.second);
	auto expected3 = std::string(res3Pair.first, res3Pair.second);
	EXPECT("Buffer::To::ParseVector({ { \"hello\", \"world!\"}, { \"how\", \" are\", \"you ? \" }}) to be \"\\x2\\x2\\x5hello\\x6world!\\x3\\x3how\\x4 are\\x4you?\"", current3 == expected3);


	auto vec4 = Buffer::To::ParseVector(res4);
	auto current4 = std::string(vec4.first, vec4.second);
	auto expected4 = std::string(res4Pair.first, res4Pair.second);
	EXPECT("Buffer::To::ParseVector({ { \"hello\", 0x12 }, { \"world!\", 0x34 } }) to be \"\\x2\\x5hello\\x12\\x6world!\\x34\"", current4 == expected4);
);


BENCH("Buffer::To::Parse (1 byte)\t\t\t", "be fast",
	char data = 5;

	START_BENCH;
	Buffer::To::Parse(data);
	STOP_BENCH;
);

BENCH("Buffer::To::Parse (2 bytes)\t\t\t", "be fast",
	unsigned short data = 34544;

	START_BENCH;
	Buffer::To::Parse(data);
	STOP_BENCH;
);

BENCH("Buffer::To::Parse (4 bytes)\t\t\t", "be fast",
	int32_t data = 464564385;

	START_BENCH;
	Buffer::To::Parse(data);
	STOP_BENCH;
);

BENCH("Buffer::To::Parse (std::string)\t\t\t", "be fast",
	std::string buffer = "hello";

	START_BENCH;
	Buffer::To::Parse(buffer);
	STOP_BENCH;
);

BENCH("Buffer::To::ParseVector\t\t\t\t", "be fast",
	START_BENCH;
	Buffer::To::ParseVector(res1);
	STOP_BENCH;
);

BENCH("Buffer::To::ParseVector (tuple)\t\t\t", "be fast",
	START_BENCH;
	Buffer::To::ParseVector(res4);
	STOP_BENCH;
);