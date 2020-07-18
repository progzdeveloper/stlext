#ifndef UTILS_HPP
#define UTILS_HPP

#include <bitset>
#include <string>

#include <cstdio>
#include <cstdint>
#include <climits>


#ifdef _MSC_VER
#include <stdarg.h>
#define __builtin_alloca   alloca
#define __builtin_va_list  va_list
#define __builtin_va_start va_start
#define __builtin_va_end   va_end
#endif

template<class _Int>
struct __powers_of_10;

template<>
struct __powers_of_10<uint32_t> {
    static const uint32_t values[10];
};


template<>
struct __powers_of_10<uint64_t> {
    static const uint64_t values[20];
};



template<>
struct __powers_of_10<int32_t> {
    static const int32_t values[19];
};


template<>
struct __powers_of_10<int64_t> {
    static const int64_t values[37];
};






namespace utils {

// Helper for the to_string / to_wstring functions.
template<typename _String, typename _CharT>
_String
__to_xstring(int (*__convf) (_CharT*, std::size_t, const _CharT*,
                             __builtin_va_list), std::size_t __n,
                             const _CharT* __fmt, ...)
{
    // XXX Eventually the result will be constructed in place in
    // the C++0x string, likely with the help of internal hooks.
    _CharT* __s = static_cast<_CharT*>(__builtin_alloca(sizeof(_CharT)
                                                        * __n));

    __builtin_va_list __args;
    __builtin_va_start(__args, __fmt);

    const int __len = __convf(__s, __n, __fmt, __args);

    __builtin_va_end(__args);

    return _String(__s, __s + __len);
}



template<unsigned _Base, class _Int>
inline std::string to_string(_Int __val);

template<unsigned _Base, class _Int>
inline std::wstring to_wstring(_Int __val);


// ASCII CHARACTER SET

// DECIMAL

template<>
inline std::string to_string<10, int8_t>(int8_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(int8_t) + 1, "%d", __val); }

template<>
inline std::string to_string<10, uint8_t>(uint8_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(uint8_t) + 1, "%u", __val); }

template<>
inline std::string to_string<10, int16_t>(int16_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(int16_t) + 1, "%d", __val); }

template<>
inline std::string to_string<10, uint16_t>(uint16_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(uint16_t) + 1, "%u", __val); }

template<>
inline std::string to_string<10, int>(int __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(int) + 1, "%d", __val); }

template<>
inline std::string to_string<10, unsigned>(unsigned __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(unsigned) + 1, "%u", __val); }

template<>
inline std::string to_string<10, long>(long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(long) + 1, "%ld", __val); }

template<>
inline std::string to_string<10, unsigned long>(unsigned long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 4 * sizeof(unsigned long) + 1, "%lu", __val); }

template<>
inline std::string to_string<10, long long>(long long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf,  4 * sizeof(long long) + 1, "%lld", __val); }

template<>
inline std::string to_string<10, unsigned long long>(unsigned long long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf,   4 * sizeof(unsigned long long) + 1,"%llu", __val); }



// HEXADECIMAL

template<>
inline std::string to_string<16, int8_t>(int8_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(char) + 1, "%X", __val); }

template<>
inline std::string to_string<16, uint8_t>(uint8_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(char) + 1, "%X", __val); }

template<>
inline std::string to_string<16, int16_t>(int16_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(int16_t) + 1, "%X", __val); }

template<>
inline std::string to_string<16, uint16_t>(uint16_t __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(uint16_t) + 1, "%X", __val); }

template<>
inline std::string to_string<16, int>(int __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(int) + 1, "%X", __val); }

template<>
inline std::string to_string<16, unsigned>(unsigned __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(unsigned) + 1, "%X", __val); }

template<>
inline std::string to_string<16, long>(long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(long) + 1, "%lX", __val); }

template<>
inline std::string to_string<16, unsigned long>(unsigned long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(unsigned long) + 1, "%lX", __val); }

template<>
inline std::string to_string<16, long long>(long long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(long long) + 1, "%llX", __val); }

template<>
inline std::string to_string<16, unsigned long long>(unsigned long long __val)
{ return __to_xstring<std::string, char>(&std::vsnprintf, 2 * sizeof(unsigned long long) + 1, "%llX", __val); }





// WIDE-CHARACTER SET

// DECIMAL
template<>
inline std::wstring to_wstring<10, int8_t>(int8_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(int8_t) + 1, L"%d", __val); }

template<>
inline std::wstring to_wstring<10, uint8_t>(uint8_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(uint8_t) + 1, L"%u", __val); }

template<>
inline std::wstring to_wstring<10, int16_t>(int16_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(int16_t) + 1, L"%d", __val); }

template<>
inline std::wstring to_wstring<10, uint16_t>(uint16_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(uint16_t) + 1, L"%u", __val); }

template<>
inline std::wstring to_wstring<10, int>(int __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(int) + 1, L"%d", __val); }

template<>
inline std::wstring to_wstring<10, unsigned>(unsigned __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(unsigned) + 1,L"%u", __val); }

template<>
inline std::wstring to_wstring<10, long>(long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(long) + 1, L"%ld", __val); }

template<>
inline std::wstring to_wstring<10, unsigned long>(unsigned long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(unsigned long) + 1, L"%lu", __val); }

template<>
inline std::wstring to_wstring<10, long long>(long long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(long long) + 1, L"%lld", __val); }

template<>
inline std::wstring to_wstring<10, unsigned long long>(unsigned long long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 4 * sizeof(unsigned long long) + 1, L"%llu", __val); }


// HEXADECIMAL
template<>
inline std::wstring to_wstring<16, int8_t>(int8_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(int8_t) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, uint8_t>(uint8_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(uint8_t) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, int16_t>(int16_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(int16_t) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, uint16_t>(uint16_t __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(uint16_t) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, int>(int __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(int) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, unsigned>(unsigned __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(unsigned) + 1, L"%X", __val); }

template<>
inline std::wstring to_wstring<16, long>(long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(long) + 1, L"%lX", __val); }

template<>
inline std::wstring to_wstring<16, unsigned long>(unsigned long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(unsigned long) + 1, L"%lX", __val); }

template<>
inline std::wstring to_wstring<16, long long>(long long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(long long) + 1, L"%llX", __val); }

template<>
inline std::wstring to_wstring<16, unsigned long long>(unsigned long long __val)
{ return __to_xstring<std::wstring, wchar_t>(&std::vswprintf, 2 * sizeof(unsigned long long) + 1, L"%llX", __val); }

} // end namespace utils


namespace ascii {

template<class _Num>
std::string to_binary(_Num x) {
    return std::bitset<sizeof(_Num)*CHAR_BIT>(x).to_string();
}

template<class _Num>
std::string to_hexadeciaml(_Num x) {
    return utils::to_string<16>(x);
}

template<class _Num>
std::string to_decimal(_Num x) {
    return utils::to_string<10>(x);
}
template<class _Num>
std::string to_octal(_Num x) {
    return utils::to_string<8>(x);
}

} // end namespace ascii


namespace wide {

template<class _Num>
std::wstring to_binary(_Num x) {
    return std::bitset<sizeof(_Num)*CHAR_BIT>(x).to_string(wchar_t('0'), wchar_t('1'));
}

template<class _Num>
std::wstring to_hexadeciaml(_Num x) {
    return utils::to_wstring<16>(x);
}

template<class _Num>
std::wstring to_decimal(_Num x) {
    return utils::to_wstring<10>(x);
}
template<class _Num>
std::wstring to_octal(_Num x) {
    return utils::to_wstring<8>(x);
}

} // end namespace wide


#endif // UTILS_HPP
