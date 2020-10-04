#include "pcheader.h"
#include "GUIDGenerator.h"

#include "OverEngine/Core/Random.h"

namespace OverEngine
{
	Guid::Guid(const String& source)
	{
		OE_CORE_ASSERT(source.size() == 36, "Invalid GUID length!");

		uint32_t offset = 0;
		for (uint32_t i = 0; i < 16; i++)
		{
			char left = source[2 * i + offset];
			char right = source[2 * i + 1 + offset];

			if (left == '-')
			{
				offset++;
				left = right;
				right = source[2 * i + 1 + offset];
			}

			uint8_t byte = 0;

			for (uint32_t i = 0; i < 2; i++)
			{
				uint32_t ascii;
				if (i == 0)
					ascii = (uint32_t)left;
				else
					ascii = (uint32_t)right;

				if (ascii >= CHAR_0_ASCII && ascii < CAPITAL_A_ASCII) // Is number
					byte += (uint8_t)(ascii - CHAR_0_ASCII);
				else if (ascii >= SMALL_A_ASCII) // a -> f
					byte += (uint8_t)(ascii - SMALL_A_ASCII + 10);
				else // A -> F
					byte += (uint8_t)(ascii - CAPITAL_A_ASCII + 10);

				if (i == 0)
					byte *= 16;
			}

			data[i] = byte;
		}

		RecalculateString(); // To be always lowercase
	}

	void Guid::RecalculateString()
	{
		std::stringstream ss;

		for (uint32_t i = 0; i < 16; i++)
		{
			const uint8_t& byte = data[i];

			uint32_t rightDigit, leftDigit;
			rightDigit = byte % 16;
			leftDigit = ((byte - rightDigit) / 16) % 16;

			for (uint32_t i = 0; i < 2; i++)
			{
				uint32_t* digitPtr = nullptr;
				if (i == 0)
					digitPtr = &leftDigit;
				else
					digitPtr = &rightDigit;

				if (*digitPtr < 10)
					ss << *digitPtr;
				else
					ss << (char)((SMALL_A_ASCII)+*digitPtr - 10);
			}

			if (i == 3 || i == 5 || i == 7 || i == 9)
				ss << "-";
		}

		string = ss.str();
	}

	void Guid::operator=(const Guid& other)
	{
		for (int32_t i = 0; i < 16; i++)
			data[i] = other.data[i];

		string = other.string;
	}

	bool Guid::operator==(const Guid& other) const
	{
		for (int32_t i = 0; i < 16; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}

	Guid GUIDGenerator::GenerateVersion4()
	{
		Guid guid;

		for (auto& byte : guid.data)
			byte = (uint8_t)(Random::Float() * 0xff);

		guid.data[6] &= 0x0f;
		guid.data[6] |= 0x40;

		guid.data[8] &= 0x3f;
		guid.data[8] |= 0x80;

		guid.RecalculateString();
		return guid;
	}
}
