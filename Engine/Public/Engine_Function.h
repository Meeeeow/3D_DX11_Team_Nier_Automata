#pragma once
#ifndef __ENGINE_FUNCTION_H__
#define __ENGINE_FUNCTION_H__

namespace Engine {

	// Safe Delete ///////////////////////
	template <typename T>
	void Safe_Delete(T& pPointer)
	{
		if (pPointer != nullptr)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}
	template <typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (pPointer != nullptr)
		{
			delete[] pPointer;
			pPointer = nullptr;
		}
	}
	//////////////////////////////////////

	// Referrence Count Function /////////
	template <typename T>
	unsigned long Safe_AddRef(T& pBase)
	{
		unsigned long dwRefCnt = 0;
		if (pBase != nullptr)
			dwRefCnt = pBase->AddRef();

		return dwRefCnt;
	}
	template <typename T>
	unsigned long Safe_Release(T& pBase)
	{
		unsigned long dwRefCnt = 0;
		if (pBase != nullptr)
		{
			dwRefCnt = pBase->Release();
			if (dwRefCnt == 0)
				pBase = nullptr;
		}
		return dwRefCnt;
	}
	//////////////////////////////////////

	


	// Tag Finder ////////////////////////
	class CTagFinder
	{
	public:
		explicit CTagFinder(const wchar_t* pTag) : m_pTag(pTag) {}
		virtual ~CTagFinder() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !lstrcmp(Pair.first, m_pTag);
		}
	private:
		const wchar_t*		m_pTag = nullptr;
	};
	//////////////////////////////////////

	// String Finder /////////////////////
	class CStringFinder
	{
	public:
		explicit CStringFinder(wstring pTag) : m_pTag(pTag) {}
		virtual ~CStringFinder() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !lstrcmp(Pair.first.c_str(), m_pTag.c_str());
		}
	private:
		wstring		m_pTag = nullptr;
	};
	//////////////////////////////////////
}
#endif // !__ENGINE_FUNCTION_H__