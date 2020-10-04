#pragma once

namespace OverEngine
{
	struct Guid
	{
		Guid() = default;
		Guid(const String& source);

		inline const String& ToString() const { return string; }
		void Guid::RecalculateString();
		void operator =(const Guid& other);
		bool operator ==(const Guid& other) const;

		String string;
		uint8_t data[16];
	};

	class GUIDGenerator
	{
	public:
		static Guid GenerateVersion4();
	};
}
