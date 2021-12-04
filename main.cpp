#include "Buffer.h"
#include "Tester.h"

TEST("Buffer::ParseOneByte", "work",
	char* buffer = new char[256];

int index = 0;
for (char i = -128; i < 127; i++)
{
	buffer[index] = i;
	EXPECT("Buffer::ParseOneByte(" + std::to_string(i) + ") to be " + std::to_string(i), Buffer::ParseOneByte<unsigned char>(buffer, index) == static_cast<unsigned char>(i));
	index++;
}
);

BENCH("Buffer::ParseOneByte", "be fast",
	const char* buffer = "\x25\x2f";

START_BENCH;
Buffer::ParseOneByte<unsigned char>(buffer, 1);
STOP_BENCH;
);

TEST("Buffer::ParseTwoBytes", "work",
	const char* buffer = "\x16\x25\x2f\xff";

EXPECT("Buffer::ParseTwoBytes(\"\\x16\\x25\") to be 9494", Buffer::ParseTwoBytes<short>(buffer, 0) == 9494);
EXPECT("Buffer::ParseTwoBytesLittleEndian(\"\\x16\\x25\") to be 9494", Buffer::ParseTwoBytesLittleEndian<short>(buffer, 0) == 9494);
EXPECT("Buffer::ParseTwoBytesBigEndian(\"\\x16\\x25\") to be 5669", Buffer::ParseTwoBytesBigEndian<short>(buffer, 0) == 5669);

EXPECT("Buffer::ParseTwoBytes(\"\\x25\\x2F\") to be 12069", Buffer::ParseTwoBytes<short>(buffer, 1) == 12069);
EXPECT("Buffer::ParseTwoBytesLittleEndian(\"\\x25\\x2F\") to be 12069", Buffer::ParseTwoBytesLittleEndian<short>(buffer, 1) == 12069);
EXPECT("Buffer::ParseTwoBytesBigEndian(\"\\x25\\x2F\") to be 9519", Buffer::ParseTwoBytesBigEndian<short>(buffer, 1) == 9519);

EXPECT("Buffer::ParseTwoBytes(\"\\x2F\\xFF\") to be 65327", Buffer::ParseTwoBytes<unsigned short>(buffer, 2) == 65327);
EXPECT("Buffer::ParseTwoBytesLittleEndian(\"\\x2F\\xFF\") to be 65327", Buffer::ParseTwoBytesLittleEndian<unsigned short>(buffer, 2) == 65327);
EXPECT("Buffer::ParseTwoBytes(\"\\x2F\\xFF\") to be -209", Buffer::ParseTwoBytes<short>(buffer, 2) == -209);
EXPECT("Buffer::ParseTwoBytesLittleEndian(\"\\x2F\\xFF\") to be -209", Buffer::ParseTwoBytesLittleEndian<short>(buffer, 2) == -209);
EXPECT("Buffer::ParseTwoBytesBigEndian(\"\\x2F\\xFF\") to be 12287", Buffer::ParseTwoBytesBigEndian<short>(buffer, 2) == 12287);
);

BENCH("Buffer::ParseTwoBytes", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

START_BENCH;
Buffer::ParseTwoBytes<short>(buffer, 1);
STOP_BENCH;
);

TEST("Buffer::ParseFourBytes", "work",
	const char* buffer = "\x16\x25\x2f\xff";

EXPECT("Buffer::ParseFourBytes(\"\\x16\\x25\\x2F\\xFF\") to be 4281279766", Buffer::ParseFourBytes<int>(buffer, 0) == -13687530);
EXPECT("Buffer::ParseFourBytesLittleEndian(\"\\x16\\x25\\x2F\\xFF\") to be 4281279766", Buffer::ParseFourBytesLittleEndian<unsigned int>(buffer, 0) == 4281279766);
EXPECT("Buffer::ParseFourBytesBigEndian(\"\\x16\\x25\\x2F\\xFF\") to be 371535871", Buffer::ParseFourBytesBigEndian<int>(buffer, 0) == 371535871);
);

BENCH("Buffer::ParseFourBytes", "be fast",
	const char* buffer = "\x16\x25\x2f\xff";

START_BENCH;
Buffer::ParseFourBytes<int>(buffer, 0);
STOP_BENCH;
);

TEST("Buffer::ParseString", "work",
	const char* buffer = "hello\0world";

EXPECT("Buffer::ParseString(\"hello\\0world\", 0) to be \"hello\"", Buffer::ParseString(buffer, 0) == "hello");
EXPECT("Buffer::ParseString(\"hello\\0world\", 1) to be \"ello\"", Buffer::ParseString(buffer, 1) == "ello");
EXPECT("Buffer::ParseString(\"hello\\0world\", 5) to be \"\"", Buffer::ParseString(buffer, 5) == "\x00");
EXPECT("Buffer::ParseString(\"hello\\0world\", 6) to be \"world\"", Buffer::ParseString(buffer, 6) == "world");
);

BENCH("Buffer::ParseString   ", "be fast",
	const char* buffer = "hello\0world";

START_BENCH;
Buffer::ParseString(buffer, 0);
STOP_BENCH;
);

TEST("Buffer::ParseStringWithSize", "work",
	const char* buffer = "hello\0world";

EXPECT("Buffer::ParseStringWithSize(\"hello\\0world\", 0, 11) to be \"hello\\0world\"", Buffer::ParseStringWithSize(buffer, 0, 11) == std::string("hello\0world", 11));
EXPECT("Buffer::ParseStringWithSize(\"hello\\0world\", 1, 10) to be \"ello\"", Buffer::ParseStringWithSize(buffer, 1, 10) == std::string("ello\0world", 10));
EXPECT("Buffer::ParseStringWithSize(\"hello\\0world\", 5, 1) to be \"\"", Buffer::ParseStringWithSize(buffer, 5, 1) == std::string("\0", 1));
EXPECT("Buffer::ParseStringWithSize(\"hello\\0world\", 6, 5) to be \"world\"", Buffer::ParseStringWithSize(buffer, 6, 5) == "world");
EXPECT("Buffer::ParseStringWithSize(\"hello\\0world\", 6, 5) to be \"world\"", Buffer::ParseStringWithSize(buffer, 1, 0) == "");
);

BENCH("Buffer::ParseStringWithSize", "be fast",
	const char* buffer = "hello\0world";

START_BENCH;
Buffer::ParseStringWithSize(buffer, 0, 11);
STOP_BENCH;
);

class Dummy : public Buffer::Bufferizable
{
public:
	Dummy(int A, int B, std::string C) : a(A), b(B), c(C) {}

	char* Bufferize() override
	{
		char* buffer = Buffer::InitializeBuffer(sizeof a + sizeof b + 1 + c.size());
		auto t = Buffer::ParseFourBytesLittleEndian(b);
		Buffer::Concat(buffer, Buffer::ParseFourBytesLittleEndian(a), 4);
		Buffer::Concat(buffer + 4, Buffer::ParseFourBytesLittleEndian(b), 4);
		Buffer::Concat(buffer + 8, Buffer::ParseFourBytesLittleEndian(c.size()), 4);
		Buffer::Concat(buffer + 12, c.c_str(), c.size());
		return buffer;
	}

	void Unbufferize(const char* Buffer) override
	{
		a = Buffer::ParseFourBytesLittleEndian<int>(Buffer, 0);
		b = Buffer::ParseFourBytesLittleEndian<int>(Buffer, 4);
		auto size = Buffer::ParseFourBytesLittleEndian<int>(Buffer, 8);
		c = Buffer::ParseStringWithSize(Buffer, 12, size);
	}

	int a;
	int b;
	std::string c;
};

TEST("Bufferizable::Bufferize", "Handle simple class",
	Dummy dummy(0, 0, ""); // 0x5 0x1e240 hello
const char* buffer = "\x5\x00\x00\x00\x40\xe2\x01\x00\x6\x00\x00\x00hellob";
dummy.Unbufferize(buffer);
EXPECT("a to be 5", dummy.a == 5);
EXPECT("b to be 123456", dummy.b == 123456);
EXPECT("c to be \"hellob\"", dummy.c == "hellob");
const char* new_buffer = dummy.Bufferize();
EXPECT("buffer and new_buffer to be identical", memcmp(buffer, new_buffer, 18) == 0);
)

int main()
{
	return Tester::Run();
}
