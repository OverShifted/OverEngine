#pragma once

namespace OverEngine
{
	struct Guid
	{
		Guid() = default;
		Guid(const String& source);

		String ToString(bool upperCase = false) const;
		void operator =(const Guid& other);
		bool operator ==(const Guid& other);

		uint8_t data[16];
	};

	class GUIDGenerator
	{
	public:
		static Guid GenerateVersion4();
	};
}
