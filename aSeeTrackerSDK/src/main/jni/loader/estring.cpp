#ifdef WIN32
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <iostream>
#include <stdio.h>		//sprintf
#include <stdlib.h>
#include <stdarg.h>

#ifndef _MSC_VER
#include <cstddef>
#endif

#include <algorithm>
#include <string.h>		//strlen
#include "estring.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//static size_t max(size_t a, size_t b)
//{
//    return (a > b ? a : b);
//}
//static size_t min(size_t a, size_t b)  
//{
//    return (a < b ? a : b);
//}

const estring estring::whitespace = "\t\n\x0b\x0c\r ";

estring::estring(const char *pszStr/* = NULL*/)
	:std::string(pszStr == NULL? "": pszStr)
{
    //setstr(pszStr);
}

estring::estring(const std::string &str)
	:std::string(str)
{
    //setstr(str.c_str());
}

estring::estring(const estring &str)
	:std::string(dynamic_cast<const std::string &>(str))
{
	//setstr(str.c_str());
}

estring::estring(const_iterator from, const_iterator to)
	:std::string(from, to)
{
}
estring::estring(const_pointer from, const_pointer to)
	:std::string(from, to)
{
}

estring::~estring()
{

}

estring &estring::operator=(const char *s)
{
	dynamic_cast<std::string &>(*this) = (s == nullptr? "": s);

	return *this;
}

estring &estring::operator=(const std::string &str)
{
	dynamic_cast<std::string &>(*this) = dynamic_cast<const std::string &>(str);

    return *this;
}
estring &estring::operator=(const estring &str)
{
    //if (this != &str)
    //    setstr(str.c_str());
	dynamic_cast<std::string &>(*this) = dynamic_cast<const std::string &>(str);

    return *this;
}

estring &estring::operator=(std::string &&str)
{
	dynamic_cast<std::string &>(*this) = std::forward<std::string>(str);
	return *this;
}
estring &estring::operator=(estring &&str)
{
	dynamic_cast<std::string &>(*this) = static_cast<std::string &&>(str);
	return *this;
}

estring::operator const char *() const
{
	return c_str();
}

//Remove leading characters. 
//If chars is omitted or None, whitespace characters are removed. 
//If given and not None, chars must be a string; 
//the characters in the string will be stripped from the beginning of the string this method is called on.
//estring &estring::ltrim()
//{
//	iterator it = begin();
//    for (; it != end() && ::isspace((unsigned char)*it); ++it)
//        ;
//    
//    erase(begin(), it);
//
//    return *this;
//}
//
estring &estring::ltrim(const char *chars/* = NULL*/)
{
    estring ws(chars);
    if (ws == "")
        ws = whitespace;

	iterator it = begin();
    for (; it != end() && std::find(ws.begin(), ws.end(), *it) != ws.end(); ++it)
    {
        ;
    }
    
    erase(begin(), it);

    return *this;
}

estring estring::ltrim_copy(const char *chars/* = NULL*/) const
{
    return estring(*this).ltrim(chars);
}
//Remove trailing characters. 
//If chars is omitted or None, whitespace characters are removed. 
//If given and not None, chars must be a string; 
//the characters in the string will be stripped from the end of the string this method is called on
//estring &estring::rtrim()
//{
//    reverse_iterator it = rbegin();
//    for(; it != rend() && ::isspace((unsigned char)*it); ++it)
//        ;
//
//	if (it != rbegin())	
//    	erase(end() - (it - rbegin()));
//
//    return *this;
//}
//
estring &estring::rtrim(const char *chars/* = NULL*/)
{
    estring ws(chars);
    if (ws == "")
        ws = whitespace;

    reverse_iterator it = rbegin();
    for (; it != rend() && std::find(ws.begin(), ws.end(), *it) != ws.end(); ++it)
    {
        ;
    }
    
	if (it != rbegin())	
    	erase(end() - (it - rbegin()), end());

    return *this;
}
estring estring::rtrim_copy(const char *chars/* = NULL*/) const
{
    return estring(*this).rtrim(chars);
}
//Remove leading and trailing characters.
//If chars is omitted or None, whitespace characters are removed. 
//If given and not None, chars must be a string; 
//the characters in the string will be stripped from the both ends of the string this method is called on
//estring &estring::trim()
//{
    //ltrim();
    //return rtrim();
//}
estring &estring::trim(const char *chars/* = NULL*/)
{
    ltrim(chars);
    return rtrim(chars);
}
estring estring::trim_copy(const char *chars/* = NULL*/) const
{
    return estring(*this).trim(chars);
}

bool estring::start_with(const char *r, const char *r_end) const
{
	const char *s = c_str();
	for(; *s && r < r_end && *s == *r; ++s, ++r)
	{}

	return r == r_end;
}

bool estring::start_with(const char *r) const
{
	const char *s = c_str();
	for(; *s && *r && *s == *r; ++s, ++r)
	{}

	return *r == '\0';
}
bool estring::start_with(const estring &r) const
{
	return start_with(r.c_str());
}

bool estring::end_with(const char *r, const char *r_end) const
{
	const char *s_begin = c_str();
	const char *s = s_begin + size() - 1;

	for (--r_end; r_end >= r && s >= s_begin && *r_end == *s; --r_end, --s)
	{}

	return r > r_end;
}
bool estring::end_with(const char *r) const
{
	const char *r_end = r + strlen(r);
	return end_with(r, r_end);
}
bool estring::end_with(const estring &r) const
{
	return end_with(r.c_str(), r.c_str() + r.size());
}

//void estring::setstr(const char *pszStr)
//{
//    if (pszStr != NULL)
//        assign(pszStr);
//    else
//        assign("");
//}

estring &estring::capitialize()
{
	if (size() > 0)
	{
		estring::iterator first = begin();
		*first = ::toupper(*first);
	}

    return *this;
}

estring &estring::lower()
{
	//for (estring::iterator it = begin(); it != end(); ++it)
	for (unsigned char *it = (unsigned char *)c_str(); *it; ++it)
	{
        if (::isupper(*it) != 0)
			*it = ::tolower(*it);
	}
	
	//std::cout << this->c_str() << std::endl;
	return *this;
}

estring &estring::upper()
{
	for (unsigned char *it = (unsigned char *)c_str(); *it; ++it)
	{
        if (::islower(*it) != 0)
			*it = ::toupper(*it);
	}
	
	//std::cout << this->c_str() << std::endl;
	return *this;
}

estring &estring::swapcase()
{
	for (unsigned char *it = (unsigned char *)c_str(); *it; ++it)
	{
		if (::islower(*it) != 0)
			*it = ::toupper(*it);
		else if (::isupper(*it) != 0)
			*it = ::tolower(*it);
	}
	
	//std::cout << this->c_str() << std::endl;
	return *this;
}

bool estring::isalnum() const
{
	//for (estring::const_iterator it = begin(); it != end(); ++it)
	for (const unsigned char *it = (const unsigned char *)c_str(); *it; ++it)
	{
        if (!(::isalnum(*it)))//digit(*it) || ::isalpha(*it)))
        	return false;
	}
	
    return true;
}

bool estring::isalpha() const
{
	for (const unsigned char *it = (const unsigned char *)c_str(); *it; ++it)
	{
        if (::isalpha(*it) == false)
        	return false;
	}
    return true;
}

bool estring::isdigit() const
{
	for (const unsigned char *it = (const unsigned char *)c_str(); *it; ++it)
	{
        if (::isdigit(*it) == false)
        	return false;
	}
    return true;
}

bool estring::islower() const
{
	for (const unsigned char *it = (const unsigned char *)c_str(); *it; ++it)
	{
        if (::islower(*it) == false)
        	return false;
	}
    return true;
}

bool estring::isupper() const
{
	for (const unsigned char *it = (const unsigned char *)c_str(); *it; ++it)
	{
        if (::isupper(*it) == false)
        	return false;
	}
    return true;
}

bool estring::istitle() const
{
	const char *from = c_str(), *to = c_str();
	const char *endTo = c_str() + size();
	while ((to = findnextspace(from, endTo)) != NULL)
	{
		estring s(from, to);
		s.ltrim();
		//std::cout << s.c_str() << "\t" << s.at(0) << std::endl;
		if (!::isupper(*(unsigned char *)&s.at(0)))
			return false;
			
		from = to;
	}
	
    return true;
}

bool estring::isspace() const
{
	for (const_iterator it = begin(); it != end(); ++it)
	{
        //if (::isspace((unsigned char)*it) == false)
		if (std::find(whitespace.begin(), whitespace.end(), *it) == whitespace.end())
        	return false;
	}
    return true;
}

const char *estring::findnextspace(const char *from, const char *to)
{
	bool striped(false);
	for (const char *it = from; it != to; ++it)
	{
		//if (::isspace((unsigned char)at(it)) == false)
        if (std::find(whitespace.begin(), whitespace.end(), *it) == whitespace.end())
			striped = true;
		else 
		{
			if (striped)
				return it;
		}
	}
	
	return striped == true? to: NULL;
}

////template<typename I>
//int do_div(unsigned long long &n, int base)
//{
//	int rem;
//	rem = (int)(n % base);
//	n = n / base;
//	return rem;
//}
///**
// * simple_strtoul - convert a string to an unsigned long
// * @cp: The start of the string
// * @endp: A pointer to the end of the parsed string will be placed here
// * @base: The number base to use
// */
//unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
//{
//	unsigned long result = 0,value;
//
//	if (!base) {
//		base = 10;
//		if (*cp == '0') {
//			base = 8;
//			cp++;
//			if ((toupper(*cp) == 'X') && isxdigit(cp[1])) {
//				cp++;
//				base = 16;
//			}
//		}
//	} else if (base == 16) {
//		if (cp[0] == '0' && toupper(cp[1]) == 'X')
//			cp += 2;
//	}
//	while (isxdigit(*cp) &&
//	       (value = isdigit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base) {
//		result = result*base + value;
//		cp++;
//	}
//	if (endp)
//		*endp = (char *)cp;
//	return result;
//}
//
//
///**
// * simple_strtol - convert a string to a signed long
// * @cp: The start of the string
// * @endp: A pointer to the end of the parsed string will be placed here
// * @base: The number base to use
// */
//long simple_strtol(const char *cp,char **endp,unsigned int base)
//{
//	if(*cp=='-')
//		return -1 * simple_strtoul(cp+1,endp,base);
//	return simple_strtoul(cp,endp,base);
//}
//
//
///**
// * simple_strtoull - convert a string to an unsigned long long
// * @cp: The start of the string
// * @endp: A pointer to the end of the parsed string will be placed here
// * @base: The number base to use
// */
//unsigned long long simple_strtoull(const char *cp,char **endp,unsigned int base)
//{
//	unsigned long long result = 0,value;
//
//	if (!base) {
//		base = 10;
//		if (*cp == '0') {
//			base = 8;
//			cp++;
//			if ((toupper(*cp) == 'X') && isxdigit(cp[1])) {
//				cp++;
//				base = 16;
//			}
//		}
//	} else if (base == 16) {
//		if (cp[0] == '0' && toupper(cp[1]) == 'X')
//			cp += 2;
//	}
//	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
//	    ? toupper(*cp) : *cp)-'A'+10) < base) {
//		result = result*base + value;
//		cp++;
//	}
//	if (endp)
//		*endp = (char *)cp;
//	return result;
//}
//
///**
// * simple_strtoll - convert a string to a signed long long
// * @cp: The start of the string
// * @endp: A pointer to the end of the parsed string will be placed here
// * @base: The number base to use
// */
//long long simple_strtoll(const char *cp,char **endp,unsigned int base)
//{
//	if(*cp=='-')
//		return -1 * simple_strtoull(cp+1,endp,base);
//	return simple_strtoull(cp,endp,base);
//}
//
//static int skip_atoi(const char **s)
//{
//	int i=0;
//
//	while (isdigit(**s))
//		i = i*10 + *((*s)++) - '0';
//	return i;
//}
//
//#define ZEROPAD	1		/* pad with zero */
//#define SIGN	2		/* unsigned/signed long */
//#define PLUS	4		/* show plus */
//#define SPACE	8		/* space if plus */
//#define LEFT	16		/* left justified */
//#define SPECIAL	32		/* 0x */
//#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
//
//static char * number(char * buf, char * end, unsigned long long num, int base, int size, int precision, int type)
//{
//	char c,sign,tmp[66];
//	const char *digits;
//	static const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
//	static const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//	int i;
//
//	digits = (type & LARGE) ? large_digits : small_digits;
//	if (type & LEFT)
//		type &= ~ZEROPAD;
//	if (base < 2 || base > 36)
//		return NULL;
//	c = (type & ZEROPAD) ? '0' : ' ';
//	sign = 0;
//	if (type & SIGN) {
//		if ((signed long long) num < 0) {
//			sign = '-';
//			num = - (signed long long) num;
//			size--;
//		} else if (type & PLUS) {
//			sign = '+';
//			size--;
//		} else if (type & SPACE) {
//			sign = ' ';
//			size--;
//		}
//	}
//	if (type & SPECIAL) {
//		if (base == 16)
//			size -= 2;
//		else if (base == 8)
//			size--;
//	}
//	i = 0;
//	if (num == 0)
//		tmp[i++]='0';
//	else while (num != 0)
//		tmp[i++] = digits[do_div(num,base)];
//	if (i > precision)
//		precision = i;
//	size -= precision;
//	if (!(type&(ZEROPAD+LEFT))) {
//		while(size-->0) {
//			if (buf < end)
//				*buf = ' ';
//			++buf;
//		}
//	}
//	if (sign) {
//		if (buf < end)
//			*buf = sign;
//		++buf;
//	}
//	if (type & SPECIAL) {
//		if (base==8) {
//			if (buf < end)
//				*buf = '0';
//			++buf;
//		} else if (base==16) {
//			if (buf < end)
//				*buf = '0';
//			++buf;
//			if (buf < end)
//				*buf = digits[33];
//			++buf;
//		}
//	}
//	if (!(type & LEFT)) {
//		while (size-- > 0) {
//			if (buf < end)
//				*buf = c;
//			++buf;
//		}
//	}
//	while (i < precision--) {
//		if (buf < end)
//			*buf = '0';
//		++buf;
//	}
//	while (i-- > 0) {
//		if (buf < end)
//			*buf = tmp[i];
//		++buf;
//	}
//	while (size-- > 0) {
//		if (buf < end)
//			*buf = ' ';
//		++buf;
//	}
//	return buf;
//}
//
//static char *decimal_fraction(char * buf, char * end, double num, const char *fmt_begin, const char *fmt_end)
//{
//	estring fmt(fmt_begin, fmt_end);
//	char sz_str[64] = {0};
//
//	sprintf(sz_str, fmt.c_str(), num);
//
//	size_t sz_str_len = strlen(sz_str);
//	if (end > buf)
//	{
//		size_t cpy_cnt = sz_str_len;
//		if (buf + cpy_cnt > end)
//			cpy_cnt = end - buf;
//		strncpy(buf, sz_str, cpy_cnt);
//	}
//	buf += sz_str_len;
//
//	return buf;
//}
///**
// * vsnprintf - Format a string and place it in a buffer
// * @buf: The buffer to place the result into
// * @size: The size of the buffer, including the trailing null space
// * @fmt: The format string to use
// * @args: Arguments for the format string
// *
// * The return value is the number of characters which would
// * be generated for the given input, excluding the trailing
// * '\0', as per ISO C99. If you want to have the exact
// * number of characters written into @buf as return value
// * (not including the trailing '\0'), use vscnprintf. If the
// * return is greater than or equal to @size, the resulting
// * string is truncated.
// *
// * Call this function if you are already dealing with a va_list.
// * You probably want snprintf instead.
// */
//int my_vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
//{
//	int len;
//	unsigned long long num;
//	int i, base;
//	char *str, *end, c;
//	const char *s;
//
//	int flags;		/* flags to number() */
//
//	int field_width;	/* width of output field */
//	int precision;		/* min. # of digits for integers; max
//				   number of chars for from string */
//	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
//				/* 'z' support added 23/7/1999 S.H.    */
//				/* 'z' changed to 'Z' --davidm 1/25/99 */
//				/* 't' added for ptrdiff_t */
//
//	/* Reject out-of-range values early.  Large positive sizes are
//	   used for unknown buffer sizes. */
//	if ((int) size < 0) {
//		/* There can be only one.. */
//		//static int warn = 1;
//		//WARN_ON(warn);
//		//warn = 0;
//		return 0;
//	}
//
//	str = buf;
//	end = buf + size;
//
//	/* Make sure end is always >= buf */
//	if (end < buf) {
//		end = (char *)((void *)-1);
//		size = end - buf;
//	}
//
//	for (; *fmt ; ++fmt) {
//		if (*fmt != '%') {
//			if (str < end)
//				*str = *fmt;
//			++str;
//			continue;
//		}
//
//		const char *fmt_begin = fmt;
//		/* process flags */
//		flags = 0;
//		repeat:
//			++fmt;		/* this also skips first '%' */
//			switch (*fmt) {
//				case '-': flags |= LEFT; goto repeat;
//				case '+': flags |= PLUS; goto repeat;
//				case ' ': flags |= SPACE; goto repeat;
//				case '#': flags |= SPECIAL; goto repeat;
//				case '0': flags |= ZEROPAD; goto repeat;
//			}
//
//		/* get field width */
//		field_width = -1;
//		if (isdigit(*fmt))
//			field_width = skip_atoi(&fmt);
//		else if (*fmt == '*') {
//			++fmt;
//			/* it's the next argument */
//			field_width = va_arg(args, int);
//			if (field_width < 0) {
//				field_width = -field_width;
//				flags |= LEFT;
//			}
//		}
//
//		/* get the precision */
//		precision = -1;
//		if (*fmt == '.') {
//			++fmt;	
//			if (isdigit(*fmt))
//				precision = skip_atoi(&fmt);
//			else if (*fmt == '*') {
//				++fmt;
//				/* it's the next argument */
//				precision = va_arg(args, int);
//			}
//			if (precision < 0)
//				precision = 0;
//		}
//
//		/* get the conversion qualifier */
//		qualifier = -1;
//		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
//		    *fmt =='Z' || *fmt == 'z' || *fmt == 't') {
//			qualifier = *fmt;
//			++fmt;
//			if (qualifier == 'l' && *fmt == 'l') {
//				qualifier = 'L';
//				++fmt;
//			}
//		}
//
//		/* default base */
//		base = 10;
//
//		switch (*fmt) {
//			case 'c':
//				if (!(flags & LEFT)) {
//					while (--field_width > 0) {
//						if (str < end)
//							*str = ' ';
//						++str;
//					}
//				}
//				c = (unsigned char) va_arg(args, int);
//				if (str < end)
//					*str = c;
//				++str;
//				while (--field_width > 0) {
//					if (str < end)
//						*str = ' ';
//					++str;
//				}
//				continue;
//
//			case 's':
//				s = va_arg(args, char *);
//				if (s == NULL)//(unsigned long)s < 4096/*PAGE_SIZEi*/)
//					s = "<NULL>";
//
//				len = static_cast<int>(strnlen(s, precision));
//
//				if (!(flags & LEFT)) {
//					while (len < field_width--) {
//						if (str < end)
//							*str = ' ';
//						++str;
//					}
//				}
//				for (i = 0; i < len; ++i) {
//					if (str < end)
//						*str = *s;
//					++str; ++s;
//				}
//				while (len < field_width--) {
//					if (str < end)
//						*str = ' ';
//					++str;
//				}
//				continue;
//
//			case 'p':
//				if (field_width == -1) {
//					field_width = 2*sizeof(void *);
//					flags |= ZEROPAD;
//				}
//				str = number(str, end,
//						(unsigned long) va_arg(args, void *),
//						16, field_width, precision, flags);
//				continue;
//
//
//			case 'n':
//				/* FIXME:
//				* What does C99 say about the overflow case here? */
//				if (qualifier == 'l') {
//					long * ip = va_arg(args, long *);
//					*ip = (str - buf);
//				} else if (qualifier == 'Z' || qualifier == 'z') {
//					size_t * ip = va_arg(args, size_t *);
//					*ip = (str - buf);
//				} else {
//					int * ip = va_arg(args, int *);
//					*ip = (str - buf);
//				}
//				continue;
//
//			case 'e':
//			case 'E':
//			case 'f':
//			case 'g':
//				{
//				double df;
//				df = va_arg(args, double);
//				str = decimal_fraction(str, end, df, fmt_begin, fmt+1);
//				}
//				continue;
//
//			case '%':
//				if (str < end)
//					*str = '%';
//				++str;
//				continue;
//
//				/* integer number formats - set up the flags and "break" */
//			case 'o':
//				base = 8;
//				break;
//
//			case 'X':
//				flags |= LARGE;
//			case 'x':
//				base = 16;
//				break;
//
//			case 'd':
//			case 'i':
//				flags |= SIGN;
//			case 'u':
//				break;
//
//			default:
//				if (str < end)
//					*str = '%';
//				++str;
//				if (*fmt) {
//					if (str < end)
//						*str = *fmt;
//					++str;
//				} else {
//					--fmt;
//				}
//				continue;
//		}
//		if (qualifier == 'L')
//			num = va_arg(args, long long);
//		else if (qualifier == 'l') {
//			num = va_arg(args, unsigned long);
//			if (flags & SIGN)
//				num = (signed long) num;
//		} else if (qualifier == 'Z' || qualifier == 'z') {
//			num = va_arg(args, size_t);
//		} else if (qualifier == 't') {
//			num = va_arg(args, ptrdiff_t);
//		} else if (qualifier == 'h') {
//			num = (unsigned short) va_arg(args, int);
//			if (flags & SIGN)
//				num = (signed short) num;
//		} else {
//			num = va_arg(args, unsigned int);
//			if (flags & SIGN)
//				num = (signed int) num;
//		}
//		str = number(str, end, num, base,
//				field_width, precision, flags);
//	}
//	if (size > 0) {
//		if (str < end)
//			*str = '\0';
//		else
//			end[-1] = '\0';
//	}
//	/* the trailing null byte doesn't count towards the total */
//	return static_cast<int>(str - buf);
//}

estring &estring::vsprintf(const char *pszFormat, va_list argList)
{
//#if !defined(va_copy)
//# if defined(__va_copy)
//#  define va_copy __va_copy
//# elif defined(_va_copy)
//#  define va_copy _va_copy
//# else
//#  define va_copy(v1, v2) v1 = v2
//# endif
//#endif
#if defined(_MSC_VER) && _MSC_VER <= 1700
	// < VC 2012
# define va_copy(dst, src) dst = src
#endif

	while(true)
	{
		va_list ap;
		va_copy(ap, argList);
		int n = vsnprintf((char *)c_str(), size() + 1, pszFormat, ap);
		va_end(ap);

		if (n > -1 && n <= (int)size())
		{
			if (n < (int)size())
				resize(n);
			break;
		}

		if (n > -1)
			resize(n);
		else
			resize((size() == 0? 1: size()) * 2);
	}

	return *this;
//
//	va_list _argList;
//#if defined(__va_copy)
//	__va_copy(_argList, argList);
//#elif defined(_va_copy)
//	_va_copy(_argList, argList);
//#elif defined(va_copy)
//	va_copy(_argList, argList);
//#else
//	_argList = argList;
//#endif
//
//	size_type old_size = size();
//	//va_start(argList, pszFormat);
//	int str_size = vsnprintf((char *)c_str(), size()+1, pszFormat, _argList);
//	va_end(_argList);
//
//	if (str_size >= (int)old_size)
//	{
//		resize(str_size + 1);
//
//		//va_start(argList, pszFormat);
//		//argList = _argList;
//		str_size = vsnprintf((char *)c_str(), str_size + 1, pszFormat, argList);
//		//va_end(argList);
//	}
//	
//	resize(str_size);
//
//	return *this;
}
estring &estring::sprintf(const char *pszFormat, ...)
{
	va_list argList;

	va_start(argList, pszFormat);
	this->vsprintf(pszFormat, argList);
	va_end(argList);

	return *this;
}

