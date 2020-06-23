#pragma once
#include <exception>

namespace OverEngine
{
	namespace Structs
	{
		template<typename T>
		class List
		{
		public:
			List()
				: m_firstElement(nullptr), m_Length(0)
			{
			}

			void Append(const T& item)
			{
				if (m_Length == 0)
				{
					m_firstElement = new Element<T>(item);
				}
				else
				{
					T* l = malloc(sizeof(T));
					memcpy(l, static_cast<const void*>(item), sizeof(T));
					GetNthElementPointer(-1)->next = new Element<T>(l);
				}
				m_Length++;
			}

			void Remove(int idx)
			{
				if (idx == 0)
				{
					Element<T>* e = GetNthElementPointer(idx);
					m_firstElement = e->next;
					m_Length--;
					return e->value;
				}
				else // there is something before that
				{
					Element<T>* e_before = GetNthElementPointer(idx-1);
					Element<T>* e = e_before->next;
					Element<T>* e_after = e->next;
					if (e_after == nullptr) // Last element
						e_before->next = nullptr;
					else
						e_before->next = e_after;
					m_Length--;
					return e->value;
				}
			}

			T& operator[] (int idx)
			{
				return *GetNthElementPointer(idx)->value;
			}
		private:
			template<typename _T>
			struct Element
			{
				Element(_T* val) : value(val), next(nullptr) {}
				Element<_T>* next;
				_T* value;
			};

			Element<T>* m_firstElement;
			int m_Length;

			Element<T>* GetNthElementPointer(int idx)
			{
				if (idx < 0)
					idx = m_Length + idx;
				Element<T>* current = m_firstElement;
				for (int i = 0; i < idx; i++)
				{
					current = current->next;
				}
				return current;
			}
		};
	}
}