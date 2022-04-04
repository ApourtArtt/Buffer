#include "Buffer.h"
#include "Tester.h"
#include <stdint.h>

int main() { return Tester::Run(); }

TEST("Construct a Buffer from int8_t", "work",
	for (int8_t i = -128;; i++)
	{
		Buffer::Buffer buf(i);
		EXPECT("buf.GetSize() to be 1", buf.GetSize() == 1);
		EXPECT("buf.GetData() to be " + std::to_string(i), buf.GetData()[0] == i);
		EXPECT("Buffer::GetArguments() to be " + std::to_string(i), Buffer::Buffer::GetArguments<int8_t>(buf.GetData()) == i);
		auto argAndSize = Buffer::Buffer::GetArgumentsAndSize<int8_t>(buf.GetData());
		EXPECT("Buffer::GetArgumentsAndSize() to be " + std::to_string(i), argAndSize.first == i && argAndSize.second == 1);

		if (i == 127) // overflows
			break;
	}
);

BENCH("Construct a Buffer from int8_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((int8_t)-5);
	STOP_BENCH;
);

TEST("Construct a Buffer from uint8_t", "work",
	for (uint8_t i = 0;; i++)
	{
		Buffer::Buffer buf(i);
		EXPECT("buf.GetSize() to be 1", buf.GetSize() == 1);
		EXPECT("buf.GetData() to be " + std::to_string(i), static_cast<uint8_t>(buf.GetData()[0]) == i);
		EXPECT("Buffer::GetArguments() to be " + std::to_string(i), Buffer::Buffer::GetArguments<uint8_t>(buf.GetData()) == i);
		auto argAndSize = Buffer::Buffer::GetArgumentsAndSize<uint8_t>(buf.GetData());
		EXPECT("Buffer::GetArgumentsAndSize() to be " + std::to_string(i), argAndSize.first == i && argAndSize.second == 1);

		if (i == 255) // overflows
			break;
	}
);

BENCH("Construct a Buffer from uint8_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((uint8_t)250);
	STOP_BENCH;
);

TEST("Construct a Buffer from int16_t", "work",
	Buffer::Buffer buf1((int16_t)0);
	EXPECT("buf1.GetSize() to be 2", buf1.GetSize() == 2);
	EXPECT("buf1.GetData() to be \"\\x00\\x00\"", std::string(buf1.GetData(), 2) == std::string("\x00\x00", 2));
	EXPECT("Buffer::GetArguments() to be 0", Buffer::Buffer::GetArguments<int16_t>(buf1.GetData()) == 0);
	auto argAndSize1 = Buffer::Buffer::GetArgumentsAndSize<int16_t>(buf1.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 0", argAndSize1.first == 0 && argAndSize1.second == 2);

	Buffer::Buffer buf2((int16_t)-1);
	EXPECT("buf2.GetSize() to be 2", buf2.GetSize() == 2);
	EXPECT("buf2.GetData() to be \"\\xFF\\xFF\"", std::string(buf2.GetData(), 2) == std::string("\xFF\xFF", 2));
	EXPECT("Buffer::GetArguments() to be -1", Buffer::Buffer::GetArguments<int16_t>(buf2.GetData()) == -1);
	auto argAndSize2 = Buffer::Buffer::GetArgumentsAndSize<int16_t>(buf2.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be -1", argAndSize2.first == -1 && argAndSize2.second == 2);

	Buffer::Buffer buf3((int16_t)-32768);
	EXPECT("buf3.GetSize() to be 2", buf3.GetSize() == 2);
	EXPECT("buf3.GetData() to be \"\\x00\\x80\"", std::string(buf3.GetData(), 2) == std::string("\x00\x80", 2));
	EXPECT("Buffer::GetArguments() to be -32768", Buffer::Buffer::GetArguments<int16_t>(buf3.GetData()) == -32768);
	auto argAndSize3 = Buffer::Buffer::GetArgumentsAndSize<int16_t>(buf3.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be -32768", argAndSize3.first == -32768 && argAndSize3.second == 2);

	Buffer::Buffer buf4((int16_t)32767);
	EXPECT("buf4.GetSize() to be 2", buf4.GetSize() == 2);
	EXPECT("buf4.GetData() to be \"\\xFF\\x7F\"", std::string(buf4.GetData(), 2) == std::string("\xFF\x7F", 2));
	EXPECT("Buffer::GetArguments() to be 32767", Buffer::Buffer::GetArguments<int16_t>(buf4.GetData()) == 32767);
	auto argAndSize4 = Buffer::Buffer::GetArgumentsAndSize<int16_t>(buf4.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 32767", argAndSize4.first == 32767 && argAndSize4.second == 2);
);

BENCH("Construct a Buffer from int16_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((int16_t)-31523);
	STOP_BENCH;
);

TEST("Construct a Buffer from uint16_t", "work",
	Buffer::Buffer buf1((uint16_t)0);
	EXPECT("buf1.GetSize() to be 2", buf1.GetSize() == 2);
	EXPECT("buf1.GetData() to be \"\\x00\\x00\"", std::string(buf1.GetData(), 2) == std::string("\x00\x00", 2));
	EXPECT("Buffer::GetArguments() to be 0", Buffer::Buffer::GetArguments<uint16_t>(buf1.GetData()) == 0);
	auto argAndSize1 = Buffer::Buffer::GetArgumentsAndSize<uint16_t>(buf1.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 0", argAndSize1.first == 0 && argAndSize1.second == 2);

	Buffer::Buffer buf2((uint16_t)65535);
	EXPECT("buf2.GetSize() to be 2", buf2.GetSize() == 2);
	EXPECT("buf2.GetData() to be \"\\xFF\\xFF\"", std::string(buf2.GetData(), 2) == std::string("\xFF\xFF", 2));
	EXPECT("Buffer::GetArguments() to be 65535", Buffer::Buffer::GetArguments<uint16_t>(buf2.GetData()) == 65535);
	auto argAndSize2 = Buffer::Buffer::GetArgumentsAndSize<uint16_t>(buf2.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 65535", argAndSize2.first == 65535 && argAndSize2.second == 2);

	Buffer::Buffer buf3((uint16_t)-32768);
	EXPECT("buf3.GetSize() to be 2", buf3.GetSize() == 2);
	EXPECT("buf3.GetData() to be \"\\x00\\x80\"", std::string(buf3.GetData(), 2) == std::string("\x00\x80", 2));
	EXPECT("Buffer::GetArguments() to be 32768", Buffer::Buffer::GetArguments<uint16_t>(buf3.GetData()) == 32768);
	auto argAndSize3 = Buffer::Buffer::GetArgumentsAndSize<uint16_t>(buf3.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 32768", argAndSize3.first == 32768 && argAndSize3.second == 2);

	Buffer::Buffer buf4((uint16_t)32767);
	EXPECT("buf4.GetSize() to be 2", buf4.GetSize() == 2);
	EXPECT("buf4.GetData() to be \"\\xFF\\x7F\"", std::string(buf4.GetData(), 2) == std::string("\xFF\x7F", 2));
	EXPECT("Buffer::GetArguments() to be 32767", Buffer::Buffer::GetArguments<uint16_t>(buf4.GetData()) == 32767);
	auto argAndSize4 = Buffer::Buffer::GetArgumentsAndSize<uint16_t>(buf4.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 32767", argAndSize4.first == 32767 && argAndSize4.second == 2);
);

BENCH("Construct a Buffer from uint16_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((uint16_t)45321);
	STOP_BENCH;
);

TEST("Construct a Buffer from int32_t", "work",
	Buffer::Buffer buf1((int32_t)0);
	EXPECT("buf1.GetSize() to be 4", buf1.GetSize() == 4);
	EXPECT("buf1.GetData() to be \"\\x00\\x00\\x00\\x00\"", std::string(buf1.GetData(), 4) == std::string("\x00\x00\x00\x00", 4));
	EXPECT("Buffer::GetArguments() to be 0", Buffer::Buffer::GetArguments<int32_t>(buf1.GetData()) == 0);
	auto argAndSize1 = Buffer::Buffer::GetArgumentsAndSize<int32_t>(buf1.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 0", argAndSize1.first == 0 && argAndSize1.second == 4);

	Buffer::Buffer buf2((int32_t)-1);
	EXPECT("buf2.GetSize() to be 4", buf2.GetSize() == 4);
	EXPECT("buf2.GetData() to be \"\\xFF\\xFF\\xFF\\xFF\"", std::string(buf2.GetData(), 4) == std::string("\xFF\xFF\xFF\xFF", 4));
	EXPECT("Buffer::GetArguments() to be -1", Buffer::Buffer::GetArguments<int32_t>(buf2.GetData()) == -1);
	auto argAndSize2 = Buffer::Buffer::GetArgumentsAndSize<int32_t>(buf2.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be -1", argAndSize2.first == -1 && argAndSize2.second == 4);

	Buffer::Buffer buf3((int32_t)-2147483648);
	EXPECT("buf3.GetSize() to be 4", buf3.GetSize() == 4);
	EXPECT("buf3.GetData() to be \"\\x00\\x00\\x00\\x80\"", std::string(buf3.GetData(), 4) == std::string("\x00\x00\x00\x80", 4));
	EXPECT("Buffer::GetArguments() to be -2147483648", Buffer::Buffer::GetArguments<int32_t>(buf3.GetData()) == -2147483648);
	auto argAndSize3 = Buffer::Buffer::GetArgumentsAndSize<int32_t>(buf3.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be -2147483648", argAndSize3.first == -2147483648 && argAndSize3.second == 4);

	Buffer::Buffer buf4((int32_t)2147483647);
	EXPECT("buf4.GetSize() to be 4", buf4.GetSize() == 4);
	EXPECT("buf4.GetData() to be \"\\xFF\\xFF\\xFF\\x7F\"", std::string(buf4.GetData(), 4) == std::string("\xFF\xFF\xFF\x7F", 4));
	EXPECT("Buffer::GetArguments() to be 2147483647", Buffer::Buffer::GetArguments<int32_t>(buf4.GetData()) == 2147483647);
	auto argAndSize4 = Buffer::Buffer::GetArgumentsAndSize<int32_t>(buf4.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 2147483647", argAndSize4.first == 2147483647 && argAndSize4.second == 4);
);

BENCH("Construct a Buffer from int32_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((int32_t)-4532541);
	STOP_BENCH;
);

TEST("Construct a Buffer from int32_t", "work",
	Buffer::Buffer buf1((uint32_t)0);
	EXPECT("buf1.GetSize() to be 4", buf1.GetSize() == 4);
	EXPECT("buf1.GetData() to be \"\\x00\\x00\\x00\\x00\"", std::string(buf1.GetData(), 4) == std::string("\x00\x00\x00\x00", 4));
	EXPECT("Buffer::GetArguments() to be 0", Buffer::Buffer::GetArguments<uint32_t>(buf1.GetData()) == 0);
	auto argAndSize1 = Buffer::Buffer::GetArgumentsAndSize<uint32_t>(buf1.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 0", argAndSize1.first == 0 && argAndSize1.second == 4);

	Buffer::Buffer buf2((uint32_t)4294967295);
	EXPECT("buf2.GetSize() to be 4", buf2.GetSize() == 4);
	EXPECT("buf2.GetData() to be \"\\xFF\\xFF\\xFF\\xFF\"", std::string(buf2.GetData(), 4) == std::string("\xFF\xFF\xFF\xFF", 4));
	EXPECT("Buffer::GetArguments() to be 4294967295", Buffer::Buffer::GetArguments<uint32_t>(buf2.GetData()) == 4294967295);
	auto argAndSize2 = Buffer::Buffer::GetArgumentsAndSize<uint32_t>(buf2.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 4294967295", argAndSize2.first == 4294967295 && argAndSize2.second == 4);

	Buffer::Buffer buf3((uint32_t)2147483648);
	EXPECT("buf3.GetSize() to be 4", buf3.GetSize() == 4);
	EXPECT("buf3.GetData() to be \"\\x00\\x00\\x00\\x80\"", std::string(buf3.GetData(), 4) == std::string("\x00\x00\x00\x80", 4));
	EXPECT("Buffer::GetArguments() to be 2147483648", Buffer::Buffer::GetArguments<uint32_t>(buf3.GetData()) == 2147483648);
	auto argAndSize3 = Buffer::Buffer::GetArgumentsAndSize<uint32_t>(buf3.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 2147483648", argAndSize3.first == 2147483648 && argAndSize3.second == 4);

	Buffer::Buffer buf4((uint32_t)2147483647);
	EXPECT("buf4.GetSize() to be 4", buf4.GetSize() == 4);
	EXPECT("buf4.GetData() to be \"\\xFF\\xFF\\xFF\\x7F\"", std::string(buf4.GetData(), 4) == std::string("\xFF\xFF\xFF\x7F", 4));
	EXPECT("Buffer::GetArguments() to be 2147483647", Buffer::Buffer::GetArguments<uint32_t>(buf4.GetData()) == 2147483647);
	auto argAndSize4 = Buffer::Buffer::GetArgumentsAndSize<uint32_t>(buf4.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be 2147483647", argAndSize4.first == 2147483647 && argAndSize4.second == 4);
);

BENCH("Construct a Buffer from uint32_t", "be fast",
	START_BENCH;
	Buffer::Buffer buf((uint32_t)4532541);
	STOP_BENCH;
);

TEST("Construct a Buffer from std::string", "work",
	std::string target("hello guys!");
	Buffer::Buffer buf(target);
	EXPECT("buf.GetSize() to be 12", buf.GetSize() == 12);
	EXPECT("buf.GetData() to be \"\\x0Bhello guys!\"", std::string(buf.GetData(), 12) == std::string("\x0B") + target);
	EXPECT("Buffer::GetArguments() to be \"hello guys!\"", Buffer::Buffer::GetArguments<std::string>(buf.GetData()) == target)
	auto argAndSize = Buffer::Buffer::GetArgumentsAndSize<std::string>(buf.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be \"hello guys!\"", argAndSize.first == target && argAndSize.second == 12);
);

BENCH("Construct a Buffer from std::string", "be fast",
	std::string target("hello guys!");
	START_BENCH;
	Buffer::Buffer buf(target);
	STOP_BENCH;
);

std::tuple<int8_t, int16_t> tup_res1 = { 0x12, 0x0080 };
TEST("Construct a Buffer from std::tuple<int8_t, int16_t>", "work",
	Buffer::Buffer buf(tup_res1);
	EXPECT("buf.GetSize() to be 3", buf.GetSize() == 3);
	EXPECT("buf.GetData() to be \"\\x12\\x80\\x00\"", std::string(buf.GetData(), 3) == std::string("\x12\x80\x00", 3));
	EXPECT("Buffer::GetArguments() to be { 0x12, 0x8000 }", (Buffer::Buffer::GetArguments<std::tuple<int8_t, int16_t>>(buf.GetData())) == tup_res1);
	auto argAndSize = (Buffer::Buffer::GetArgumentsAndSize<std::tuple<int8_t, int16_t>>(buf.GetData()));
	EXPECT("Buffer::GetArgumentsAndSize() to be { 0x12, 0x8000 }", argAndSize.first == tup_res1 && argAndSize.second == 3);
);

BENCH("Construct a Buffer from std::tuple<int8_t, int16_t>", "be fast",
	START_BENCH;
	Buffer::Buffer buf(tup_res1);
	STOP_BENCH;
);

std::tuple<int8_t, std::string> tup_res2 = { 0x12, "hello"};
TEST("Construct a Buffer from std::tuple<int8_t, std::string>", "work",
	Buffer::Buffer buf(tup_res2);
	EXPECT("buf.GetSize() to be 7", buf.GetSize() == 7);
	EXPECT("buf.GetData() to be \"\\x12\\x05hello\"", std::string(buf.GetData(), 7) == std::string("\x12\x05hello", 7));
	EXPECT("Buffer::GetArguments() to be { 0x12, \"hello\" }", (Buffer::Buffer::GetArguments<std::tuple<int8_t, std::string>>(buf.GetData())) == tup_res2);
	auto argAndSize = (Buffer::Buffer::GetArgumentsAndSize<std::tuple<int8_t, std::string>>(buf.GetData()));
	EXPECT("Buffer::GetArgumentsAndSize() to be { 0x12, \"hello\" }", argAndSize.first == tup_res2 && argAndSize.second == 7);
);

BENCH("Construct a Buffer from std::tuple<int8_t, std::string>", "be fast",
	START_BENCH;
	Buffer::Buffer buf(tup_res2);
	STOP_BENCH;
);

std::vector<int8_t> vec_res1 = { 0x12, 0x34, 0x56 };
TEST("Construct a Buffer from std::vector<int8_t>", "work",
	Buffer::Buffer buf(vec_res1);
	EXPECT("buf.GetSize() to be 4", buf.GetSize() == 4);
	EXPECT("buf.GetData() to be \"\\x03\\x12\\x34\\x56\"", std::string(buf.GetData(), 4) == std::string("\x03\x12\x34\x56", 4));
	EXPECT("Buffer::GetArguments() to be { 0x12, 0x34, 0x56 }", Buffer::Buffer::GetArguments<std::vector<int8_t>>(buf.GetData()) == vec_res1);
	auto argAndSize = Buffer::Buffer::GetArgumentsAndSize<std::vector<int8_t>>(buf.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be { 0x12, 0x34, 0x56 }", argAndSize.first == vec_res1 && argAndSize.second == 4);
);

BENCH("Construct a Buffer from std::vector<int8_t>", "be fast",
	START_BENCH;
	Buffer::Buffer buf(vec_res1);
	STOP_BENCH;
);

std::vector<std::string> vec_res2 = { "hello", "guys", "!"};
TEST("Construct a Buffer from std::vector<std::string>", "work",
	Buffer::Buffer buf(vec_res2);
	EXPECT("buf.GetSize() to be 14", buf.GetSize() == 14);
	EXPECT("buf.GetData() to be \"\\x03\\x05hello\\x04guys\\x01!\"", std::string(buf.GetData(), 14) == std::string("\x03\x05hello\x04guys\x01!", 14));
	EXPECT("Buffer::GetArguments() to be { \"hello\", \"guys\", \"!\" }", Buffer::Buffer::GetArguments<std::vector<std::string>>(buf.GetData()) == vec_res2);
	auto argAndSize = Buffer::Buffer::GetArgumentsAndSize<std::vector<std::string>>(buf.GetData());
	EXPECT("Buffer::GetArgumentsAndSize() to be { \"hello\", \"guys\", \"!\" }", argAndSize.first == vec_res2 && argAndSize.second == 14);
);

BENCH("Construct a Buffer from std::vector<std::string>", "be fast",
	START_BENCH;
	Buffer::Buffer buf(vec_res2);
	STOP_BENCH;
);

std::vector<std::tuple<int8_t, std::string>> vec_res3 =
{
	{ 0x12, "hello" },
	{ 0x34, "guys" },
	{ 0x56, "!" },
};
TEST(("Construct a Buffer from std::vector<std::tuple<int8_t, std::string>>"), "work",
	Buffer::Buffer buf(vec_res3);
	EXPECT("buf.GetSize() to be 17", buf.GetSize() == 17);
	EXPECT("buf.GetData() to be \"\\x03\\x12\\x05hello\\x34\\x04guys\\x56\\x01!\"", std::string(buf.GetData(), 17) == std::string("\x03\x12\x05hello\x34\x04guys\x56\x01!", 17));
	EXPECT("Buffer::GetArguments() to be { { 0x12, \"hello\" }, { 0x34, \"guys\" }, { 0x56, \"!\" } }", (Buffer::Buffer::GetArguments<std::vector<std::tuple<int8_t, std::string>>>(buf.GetData())) == vec_res3);
	auto argAndSize = (Buffer::Buffer::GetArgumentsAndSize<std::vector<std::tuple<int8_t, std::string>>>(buf.GetData()));
	EXPECT("Buffer::GetArgumentsAndSize() to be { { 0x12, \"hello\" }, { 0x34, \"guys\" }, { 0x56, \"!\" } }", argAndSize.first == vec_res3 && argAndSize.second == 17);
);

BENCH(("Construct a Buffer from std::vector<std::tuple<int8_t, std::string>>"), "be fast",
	START_BENCH;
	Buffer::Buffer buf(vec_res3);
	STOP_BENCH;
);
