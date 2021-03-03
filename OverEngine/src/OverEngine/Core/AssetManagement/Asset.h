#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Object.h"
#include "OverEngine/Core/String.h"

namespace OverEngine
{
	struct AssetPath
	{
		Vector<String> PathNodes;
		bool Absolute = true;

		AssetPath() = default;

		AssetPath(const String& path)
		{
			Absolute = path[0] == '/';
			StringUtils::Split(path, "/",  PathNodes);
		}

		operator String() const
		{
			String str;
			if (Absolute)
				str += '/';

			StringUtils::Join(PathNodes, "/", str);
			return str;
		}
	};

	class AssetDatabase;

	class Asset : public Object
	{
		OE_CLASS(Asset)
		
	public:
		static Ref<Asset> Load(const String& path);

	protected:
		Asset() = default;

	public:
		virtual ~Asset() = default;

		virtual bool IsRefrence() const = 0;

		const String& GetName() const;
		inline const AssetPath& GetPath() const { return m_Path; }

		inline const uint64_t& GetGuid() const { return m_Guid; }
		void SetGuid(const uint64_t& guid);

		virtual void Reload() {}

	protected:
		AssetPath m_Path;
		uint64_t m_Guid = 0;

		friend class AssetDatabase;
	};

	class AssetFolder : public Asset
	{
		OE_CLASS(AssetFolder)

	public:
		AssetFolder() = default;
		bool RenameChild(const String& currentName, const String& newName);

		Ref<Asset> operator[] (const String& name);
		inline const auto& GetAssets() const { return m_Assets; }
		inline auto& GetAssets() { return m_Assets; }

		virtual bool IsRefrence() const override { return false; }

		struct AssetMappingKey
		{
			enum AssetMappingKeyType_
			{
				AssetMappingKeyType_None = 0,
				AssetMappingKeyType_Folder = 1,
				AssetMappingKeyType_NonFolder = 2,
			};

			uint8_t Type = 0;
			String Name;

			bool operator< (const AssetMappingKey& other) const
			{
				String myNameLower = Name;
				std::transform(myNameLower.begin(), myNameLower.end(), myNameLower.begin(), [](unsigned char c){ return std::tolower(c); });

				String otherNameLower = other.Name;
				std::transform(otherNameLower.begin(), otherNameLower.end(), otherNameLower.begin(), [](unsigned char c){ return std::tolower(c); });

				if (Type == other.Type)
					return myNameLower < otherNameLower;

				return Type < other.Type;
			}

			bool operator> (const AssetMappingKey& other) const
			{
				String myNameLower = Name;
				std::transform(myNameLower.begin(), myNameLower.end(), myNameLower.begin(), [](unsigned char c){ return std::tolower(c); });

				String otherNameLower = other.Name;
				std::transform(otherNameLower.begin(), otherNameLower.end(), otherNameLower.begin(), [](unsigned char c){ return std::tolower(c); });

				if (Type == other.Type)
					return myNameLower > otherNameLower;

				return Type > other.Type;
			}

			bool operator== (const AssetMappingKey& other) const
			{
				return Type == other.Type && Name == other.Name;
			}

			bool operator!= (const AssetMappingKey& other) const
			{
				return !operator==(other);
			}

			bool operator== (const String& other) const
			{
				return Name == other;
			}

			bool operator!= (const String& other) const
			{
				return !operator==(other);
			}
		};

	private:
		Map<AssetMappingKey, Ref<Asset>> m_Assets;

		friend class AssetDatabase;
	};
}

namespace std
{
	template <>
	struct hash<OverEngine::AssetFolder::AssetMappingKey>
	{
		size_t operator() (const OverEngine::AssetFolder::AssetMappingKey& v)
		{
			return std::hash<OverEngine::String>()(v.Name) + v.Type;
		}
	};
}
