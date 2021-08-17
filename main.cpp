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

);

int main()
{
	return Tester::Run();
}
