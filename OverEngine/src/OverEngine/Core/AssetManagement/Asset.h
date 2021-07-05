#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Object.h"
#include "OverEngine/Core/String.h"

namespace OverEngine
{
	class AssetDatabase;

	class Asset : public Object
	{
		OE_CLASS(Asset)
		
	public:
		static Ref<Asset> Load(const String& path);

	protected:
		Asset() = default;

	public:
		virtual void Acquire(Ref<Asset> other) {}
		virtual ~Asset() = default;

		virtual bool IsReference() const = 0;

		const String& GetName() const { return m_Name; }
		inline const String& GetPath() const { return m_Path; }

		inline const uint64_t& GetGuid() const { return m_Guid; }
		void SetGuid(const uint64_t& guid);

		virtual void Reload() {}

	protected:
		String m_Path;
		String m_Name;
		uint64_t m_Guid = 0;

		friend class AssetDatabase;
	};

	class AssetFolder : public Asset
	{
		OE_CLASS(AssetFolder)

	public:
		AssetFolder() = default;
		bool RenameChild(const String& currentName, const String& newName);

		Ref<Asset>& operator[] (const String& name) { return m_Assets[name]; }
		inline const auto& GetAssets() const { return m_Assets; }
		inline auto& GetAssets() { return m_Assets; }

		virtual bool IsReference() const override { return false; }

	private:
		Map<String, Ref<Asset>> m_Assets;

		friend class AssetDatabase;
	};
}
