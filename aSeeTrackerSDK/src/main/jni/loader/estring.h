// estring.h: interface for the estring class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ESTRING_H__332BC79F_DD00_4C68_A46A_4E334EDD0621__INCLUDED_
#define _ESTRING_H__332BC79F_DD00_4C68_A46A_4E334EDD0621__INCLUDED_
#ifndef _MSC_VER
#include <stdarg.h>
#endif
#include <utility>
#include <list>
#include <string>
#include <sstream>

template <typename T, typename Iter>
T join(const T &sep, Iter first, Iter end)
{
	T to;
	if (first != end)
	{
		std::stringstream ss;
		Iter &it = first;
		ss << *it;
		//s = ss.str();
		for (++it; it != end; ++it)
		{
			//s += sep;
			ss << sep;
			//std::stringstream ss;
			ss << *it;
			//s += ss.str();
		}

		to = ss.str();
	}
	
	return to;
}
// 字符串类型(std::string, estring, char *等)的调用这个函数，这个性能高
template <typename T, typename Iter>
T join_string(const T &sep, Iter first, Iter end)
{
	T s;
	if (first != end)
	{
		Iter &it = first;
		s = *it;
		for (++it; it != end; ++it)
			s += sep + *it;
	}
	
	return s;
}

class estring : public std::string //basic_string<char>//, std::char_traits<char>, std::allocator<char> >  
{
public:
	enum TrimDirection
	{
		trimNone = 0,	
		trimLeft = 1,	
		trimRight = 2,	
		trimAll = 3		
	};

public:
	estring(const char *pszSrc = NULL);

	estring(const_iterator from, const_iterator to);
	estring(const_pointer from, const_pointer to);

	estring(const estring &str);
	estring(const std::string &str);
	estring(estring &&str)
			:std::string(std::forward<std::string>(str))
	{}
    estring(std::string &&str)
		:std::string(std::forward<std::string>(str))
	{}

	~estring();


	estring &operator=(const char *);
	estring &operator=(const std::string &str);
    estring &operator=(const estring &str);
	estring &operator=(std::string &&str);
    estring &operator=(estring &&str);

    operator const char *() const;

public:
    //Remove leading characters. 
    //If chars is omitted or None, whitespace characters are removed. 
    //If given and not None, chars must be a string; 
    //the characters in the string will be stripped from the beginning of the string this method is called on.
    //estring &ltrim();
    estring &ltrim(const char *chars = NULL);
    estring ltrim_copy(const char *chars = NULL) const;
    //Remove trailing characters. 
    //If chars is omitted or None, whitespace characters are removed. 
    //If given and not None, chars must be a string; 
    //the characters in the string will be stripped from the end of the string this method is called on
    //estring &rtrim();
    estring &rtrim(const char *chars = NULL);
    estring rtrim_copy(const char *chars = NULL) const;
    //Remove leading and trailing characters.
    //If chars is omitted or None, whitespace characters are removed. 
    //If given and not None, chars must be a string; 
    //the characters in the string will be stripped from the both ends of the string this method is called on
    //estring &trim();
    estring &trim(const char *chars = NULL);
    estring trim_copy(const char *chars = NULL) const;

	bool start_with(const char *r, const char *r_end) const;
	bool start_with(const char *r) const;
	bool start_with(const estring &r) const;
	bool end_with(const char *r, const char *r_end) const;
	bool end_with(const char *r) const;
	bool end_with(const estring &r) const;

    estring &sprintf(const char *pszFormat, ...);
	estring &vsprintf(const char *pszFormat, va_list argList);

	estring &capitialize();
	estring &lower();
	estring &upper();
	estring &swapcase();
	bool isalnum() const;
	bool isalpha() const;
	bool isdigit() const;
	bool islower() const;
	bool isupper() const;
	bool istitle() const;
	bool isspace() const;
	template <typename L>
	void split(L &obj) const
	{
		//size_type itFrom(0), itTo(0);
		const char *itFrom = c_str(), *itTo = c_str();
		const char *endThis = c_str() + size();
	
		while ((itTo = findnextspace(itFrom, endThis)) != NULL)
		{
			obj.push_back(estring(itFrom, itTo).trim());
			//std::cout << itFrom << ", " << itTo << " = " << s.c_str() << std::endl;
		
			itFrom = itTo;
		}
	}

    template <typename L, typename Iter>
    void split(L &obj, Iter sepBegin, Iter sepEnd, TrimDirection trimDirection = trimAll) const
	{
		//size_type itFrom(0), itTo(0);
		const char *itFrom = c_str(), *itTo = c_str();
		const char *endThis = c_str() + size();
	
		Iter curSep = sepBegin;
		//while (curSep != sepEnd)
		do
		{
			itTo = findnextsep(itFrom, endThis, sepBegin, sepEnd, curSep);
			//std::cout << itFrom << "," << itTo << std::endl;
			estring s(itFrom, itTo);

			if (trimDirection & trimLeft)
				s.ltrim();
			if (trimDirection & trimRight)
				s.rtrim();
            obj.push_back(s);
			//std::cout << itFrom << ", " << itTo << " = " << s.c_str() << std::endl;
	
			itFrom = itTo + (curSep != sepEnd? curSep->size(): 0);
		}while (itTo != endThis);
	}

    template <typename L>
    void split(L &obj, const char *sep, TrimDirection trimDirection = trimAll) const
	{
		estring strSep(sep);
        split(obj, &strSep, &strSep + 1, trimDirection);
	}

	// 只要sep中任何一个字符出现，即split
	template <typename L>
	void split_c(L &obj, const char *sep, TrimDirection trimDirection = trimAll) const
	{
		std::list<estring> lstSep;
		for(; *sep != '\0'; ++sep)
			lstSep.push_back(estring(sep, sep+1));
		split(obj, lstSep.begin(), lstSep.end(), trimDirection);
	}
	
	template <typename Iter>
	estring join(Iter first, Iter end) const
	{
		return ::join(*this, first, end);
	}

	// 连接字符串类型的叠代器(性能高于join)
	template <typename Iter>
	estring join_s(Iter first, Iter end) const
	{
		return ::join_string(*this, first, end);
	}

public:
	static const estring whitespace;
	static const char *findnextspace(const char *begin, const char *end);

protected:
	template<typename Iter>
	const char *findnextsep(const char *from, const char *to, Iter begin, Iter end, Iter &findedIter) const
	{
		findedIter = end;

		for (const char *it = from; it != to; ++it)
		{
			for (findedIter = begin; findedIter != end; ++findedIter)
			{
				if (findedIter->c_str() != estring("") 
					&& compare(it-c_str(), findedIter->size(), findedIter->c_str()) == 0)
					return it;
			}
		}

		return to;
	}
};

#endif // !defined(_ESTRING_H__332BC79F_DD00_4C68_A46A_4E334EDD0621__INCLUDED_)

