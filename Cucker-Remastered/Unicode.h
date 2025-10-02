#pragma once
#pragma once
#include <string>
#include <windows.h>
#include <vector>
#include "ConsoleSetting.h"

class Ucs4CharRange {
private:
	static constexpr std::pair<int, int> comb[] = {
		{ 768u, 879u },
		{ 1155u, 1158u },
		{ 1160u, 1161u },
		{ 1425u, 1469u },
		{ 1471u, 1471u },
		{ 1473u, 1474u },
		{ 1476u, 1477u },
		{ 1479u, 1479u },
		{ 1536u, 1539u },
		{ 1552u, 1557u },
		{ 1611u, 1630u },
		{ 1648u, 1648u },
		{ 1750u, 1764u },
		{ 1767u, 1768u },
		{ 1770u, 1773u },
		{ 1807u, 1807u },
		{ 1809u, 1809u },
		{ 1840u, 1866u },
		{ 1958u, 1968u },
		{ 2027u, 2035u },
		{ 2305u, 2306u },
		{ 2364u, 2364u },
		{ 2369u, 2376u },
		{ 2381u, 2381u },
		{ 2385u, 2388u },
		{ 2402u, 2403u },
		{ 2433u, 2433u },
		{ 2492u, 2492u },
		{ 2497u, 2500u },
		{ 2509u, 2509u },
		{ 2530u, 2531u },
		{ 2561u, 2562u },
		{ 2620u, 2620u },
		{ 2625u, 2626u },
		{ 2631u, 2632u },
		{ 2635u, 2637u },
		{ 2672u, 2673u },
		{ 2689u, 2690u },
		{ 2748u, 2748u },
		{ 2753u, 2757u },
		{ 2759u, 2760u },
		{ 2765u, 2765u },
		{ 2786u, 2787u },
		{ 2817u, 2817u },
		{ 2876u, 2876u },
		{ 2879u, 2879u },
		{ 2881u, 2883u },
		{ 2893u, 2893u },
		{ 2902u, 2902u },
		{ 2946u, 2946u },
		{ 3008u, 3008u },
		{ 3021u, 3021u },
		{ 3134u, 3136u },
		{ 3142u, 3144u },
		{ 3146u, 3149u },
		{ 3157u, 3158u },
		{ 3260u, 3260u },
		{ 3263u, 3263u },
		{ 3270u, 3270u },
		{ 3276u, 3277u },
		{ 3298u, 3299u },
		{ 3393u, 3395u },
		{ 3405u, 3405u },
		{ 3530u, 3530u },
		{ 3538u, 3540u },
		{ 3542u, 3542u },
		{ 3633u, 3633u },
		{ 3636u, 3642u },
		{ 3655u, 3662u },
		{ 3761u, 3761u },
		{ 3764u, 3769u },
		{ 3771u, 3772u },
		{ 3784u, 3789u },
		{ 3864u, 3865u },
		{ 3893u, 3893u },
		{ 3895u, 3895u },
		{ 3897u, 3897u },
		{ 3953u, 3966u },
		{ 3968u, 3972u },
		{ 3974u, 3975u },
		{ 3984u, 3991u },
		{ 3993u, 4028u },
		{ 4038u, 4038u },
		{ 4141u, 4144u },
		{ 4146u, 4146u },
		{ 4150u, 4151u },
		{ 4153u, 4153u },
		{ 4184u, 4185u },
		{ 4448u, 4607u },
		{ 4959u, 4959u },
		{ 5906u, 5908u },
		{ 5938u, 5940u },
		{ 5970u, 5971u },
		{ 6002u, 6003u },
		{ 6068u, 6069u },
		{ 6071u, 6077u },
		{ 6086u, 6086u },
		{ 6089u, 6099u },
		{ 6109u, 6109u },
		{ 6155u, 6157u },
		{ 6313u, 6313u },
		{ 6432u, 6434u },
		{ 6439u, 6440u },
		{ 6450u, 6450u },
		{ 6457u, 6459u },
		{ 6679u, 6680u },
		{ 6912u, 6915u },
		{ 6964u, 6964u },
		{ 6966u, 6970u },
		{ 6972u, 6972u },
		{ 6978u, 6978u },
		{ 7019u, 7027u },
		{ 7616u, 7626u },
		{ 7678u, 7679u },
		{ 8203u, 8207u },
		{ 8234u, 8238u },
		{ 8288u, 8291u },
		{ 8298u, 8303u },
		{ 8400u, 8431u },
		{ 11930u, 11930u },
		{ 12020u, 12031u },
		{ 12246u, 12271u },
		{ 12284u, 12287u },
		{ 12772u, 12783u },
		{ 12831u, 12831u },
		{ 42125u, 42127u },
		{ 43014u, 43014u },
		{ 43019u, 43019u },
		{ 43045u, 43046u },
		{ 64286u, 64286u },
		{ 65024u, 65039u },
		{ 65050u, 65055u },
		{ 65056u, 65059u },
		{ 65107u, 65107u },
		{ 65127u, 65127u },
		{ 65279u, 65279u },
		{ 65529u, 65531u }
	};

	static constexpr std::pair<int, int> wide[] = {
		{ 4352u, 4447u },
		{ 8986u, 8987u },
		{ 9001u, 9002u },
		{ 9193u, 9196u },
		{ 9200u, 9200u },
		{ 9203u, 9203u },
		{ 9725u, 9726u },
		{ 9748u, 9749u },
		{ 9800u, 9811u },
		{ 9855u, 9855u },
		{ 9875u, 9875u },
		{ 9889u, 9889u },
		{ 9898u, 9899u },
		{ 9917u, 9918u },
		{ 9924u, 9925u },
		{ 9934u, 9934u },
		{ 9940u, 9940u },
		{ 9962u, 9962u },
		{ 9970u, 9971u },
		{ 9973u, 9973u },
		{ 9978u, 9978u },
		{ 9981u, 9981u },
		{ 9989u, 9989u },
		{ 9994u, 9995u },
		{ 10024u, 10024u },
		{ 10060u, 10060u },
		{ 10062u, 10062u },
		{ 10067u, 10069u },
		{ 10071u, 10071u },
		{ 10133u, 10135u },
		{ 10160u, 10160u },
		{ 10175u, 10175u },
		{ 11035u, 11036u },
		{ 11088u, 11088u },
		{ 11093u, 11093u },
		{ 11904u, 12350u },
		{ 12353u, 12438u },
		{ 12441u, 12543u },
		{ 12549u, 12591u },
		{ 12593u, 12686u },
		{ 12688u, 12871u },
		{ 12880u, 19903u },
		{ 19968u, 42182u },
		{ 43360u, 43388u },
		{ 44032u, 55203u },
		{ 63744u, 64255u },
		{ 65040u, 65055u },
		{ 65072u, 65131u },
		{ 65281u, 65376u },
		{ 65504u, 65510u },
		{ 65536u, 1114111u }
	};

	template <typename T>
	static bool binarySearch(T rune, const std::pair<T, T>* table, std::size_t max) {
		std::size_t num = 0;
		if (rune < table[0].first || rune > table[max].second) {
			return 0;
		}

		while (max >= num) {
			std::size_t num2 = (num + max) / 2;
			if (rune > table[num2].second) {
				num = num2 + 1;
				continue;
			}

			if (rune < table[num2].first) {
				max = num2 - 1;
				continue;
			}

			return true;
		}

		return false;
	}
public:
	static inline int GetUcs4Width(int ucs4) {
		if (ucs4 <= 31)
			return -1;
		if (ucs4 < 127)
			return 1;
		if (ucs4 >= 127 && ucs4 <= 159)
			return -1;
		if (binarySearch(ucs4, comb, 136))
			return 0;
		return 1 + (binarySearch(ucs4, wide, 50) ? 1 : 0);
	}
};

inline int GetUtf8Width(unsigned char utf8char) {
	if (utf8char <= 31) return -1;

	// 如果开头是 0x80 - 0xBF，则表示该字节是多字节字符的剩余部分
	if (utf8char >= 0x80 && utf8char <= 0xBF) {
		return -2;
	}

	// 单字节字符（0x00 - 0x7F），占 1 个字符宽度
	if (utf8char <= 0x7F) {
		return 1;
	}

	// 双字节字符（0xC0 - 0xDF），占 2 个字符宽度
	if (utf8char <= 0xDF) {
		return 2;
	}

	// 三字节字符（0xE0 - 0xEF），通常是中文，占 3 个字符宽度
	if (utf8char <= 0xEF) {
		return 3;
	}

	// 四字节字符（0xF0 - 0xF7），通常是较少见的字符，占 4 个字符宽度
	if (utf8char <= 0xF7) {
		return 4;
	}

	return -1;
}

template<class T>
inline std::u32string Utf8toUcs4(const std::basic_string<T>& utf8String)
{
	std::u32string ucs4String;

	for (size_t i = 0; i < utf8String.length();)
	{
		char32_t unicodeValue = 0;
		size_t codeLength = 0;

		// Check the number of leading 1s to determine the length of the UTF-8 code
		if ((utf8String[i] & 0x80) == 0)
		{
			// Single-byte character (ASCII)
			unicodeValue = utf8String[i];
			codeLength = 1;
		}
		else if ((utf8String[i] & 0xE0) == 0xC0)
		{
			// Two-byte character
			unicodeValue = (utf8String[i] & 0x1F) << 6;
			unicodeValue |= (utf8String[i + 1] & 0x3F);
			codeLength = 2;
		}
		else if ((utf8String[i] & 0xF0) == 0xE0)
		{
			// Three-byte character
			unicodeValue = (utf8String[i] & 0x0F) << 12;
			unicodeValue |= (utf8String[i + 1] & 0x3F) << 6;
			unicodeValue |= (utf8String[i + 2] & 0x3F);
			codeLength = 3;
		}
		else if ((utf8String[i] & 0xF8) == 0xF0)
		{
			// Four-byte character
			unicodeValue = (utf8String[i] & 0x07) << 18;
			unicodeValue |= (utf8String[i + 1] & 0x3F) << 12;
			unicodeValue |= (utf8String[i + 2] & 0x3F) << 6;
			unicodeValue |= (utf8String[i + 3] & 0x3F);
			codeLength = 4;
		}
		else
		{
			unicodeValue = '?';
			codeLength = 1;
		}

		ucs4String.push_back(unicodeValue);
		i += codeLength;
	}

	return ucs4String;
}

inline std::u32string Utf16toUcs4(const std::wstring& utf16String) {
	std::u32string ucs4String;

	for (size_t i = 0; i < utf16String.length(); ++i) {
		char32_t unicodeValue = utf16String[i];

		// Handle surrogate pairs
		if (unicodeValue >= 0xD800 && unicodeValue <= 0xDBFF && i + 1 < utf16String.length()) {
			char32_t highSurrogate = unicodeValue;
			char32_t lowSurrogate = utf16String[i + 1];

			if (lowSurrogate >= 0xDC00 && lowSurrogate <= 0xDFFF) {
				unicodeValue = ((highSurrogate - 0xD800) << 10) + (lowSurrogate - 0xDC00) + 0x10000;
				++i; // Skip the low surrogate
			}
		}

		ucs4String.push_back(unicodeValue);
	}

	return ucs4String;
}

inline std::string Ucs4toUtf8(const std::u32string& ucs4String)
{
	std::string utf8String;

	for (char32_t codePoint : ucs4String)
	{
		if (codePoint <= 0x7F)
		{
			// Single-byte character (ASCII)
			utf8String.push_back(static_cast<char>(codePoint));
		}
		else if (codePoint <= 0x7FF)
		{
			// Two-byte character
			utf8String.push_back(static_cast<char>((codePoint >> 6) | 0xC0));
			utf8String.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
		}
		else if (codePoint <= 0xFFFF)
		{
			// Three-byte character
			utf8String.push_back(static_cast<char>((codePoint >> 12) | 0xE0));
			utf8String.push_back(static_cast<char>(((codePoint >> 6) & 0x3F) | 0x80));
			utf8String.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
		}
		else if (codePoint <= 0x10FFFF)
		{
			// Four-byte character
			utf8String.push_back(static_cast<char>((codePoint >> 18) | 0xF0));
			utf8String.push_back(static_cast<char>(((codePoint >> 12) & 0x3F) | 0x80));
			utf8String.push_back(static_cast<char>(((codePoint >> 6) & 0x3F) | 0x80));
			utf8String.push_back(static_cast<char>((codePoint & 0x3F) | 0x80));
		}
		else
		{
			utf8String.push_back('?');
		}
	}

	return utf8String;
}

inline std::string Utf16ToUtf8(const std::wstring& utf16Str, int codepage = $CSetting.codePageID) {
	if (utf16Str.empty()) {
		return std::string();
	}

	// 获取所需缓冲区大小
	int sizeNeeded = WideCharToMultiByte(
		codepage,		    // 目标编码
		0,                  // 标志位（一般设为0）
		utf16Str.c_str(),   // 源UTF-16字符串
		(int)utf16Str.length(), // 字符串长度
		nullptr,            // 输出缓冲区（nullptr表示只计算大小）
		0,                  // 输出缓冲区大小
		nullptr, nullptr    // 默认字符和是否使用默认字符
	);

	if (sizeNeeded == 0) {
		// 转换失败，可以调用 GetLastError() 获取错误代码
		return "";
	}

	// 分配缓冲区
	std::vector<char> utf8Buffer(sizeNeeded + 1); // +1 为终止符

	// 实际转换
	int convertedSize = WideCharToMultiByte(
		codepage, 0,
		utf16Str.c_str(),
		(int)utf16Str.length(),
		utf8Buffer.data(),
		sizeNeeded,
		nullptr, nullptr
	);

	if (convertedSize == 0) {
		return "";
	}

	// 构造并返回std::string
	return std::string(utf8Buffer.data(), convertedSize);
}

inline std::wstring Utf8ToUtf16(const std::string& utf8Str, int codepage = $CSetting.codePageID) {
	if (utf8Str.empty()) {
		return std::wstring();
	}

	// 计算所需缓冲区大小
	int sizeNeeded = MultiByteToWideChar(
		codepage,           // 源编码
		0,                  // 标志位（一般设为0）
		utf8Str.c_str(),    // 源字符串
		(int)utf8Str.length(), // 长度
		nullptr,            // 输出缓冲区（为nullptr时仅计算大小）
		0
	);

	if (sizeNeeded == 0) {
		return L"";
	}

	// 分配缓冲区
	std::vector<wchar_t> utf16Buffer(sizeNeeded + 1); // +1 为终止符

	// 实际转换
	int convertedSize = MultiByteToWideChar(
		codepage, 0,
		utf8Str.c_str(),
		(int)utf8Str.length(),
		utf16Buffer.data(),
		sizeNeeded
	);

	if (convertedSize == 0) {
		return L"";
	}

	// 构造并返回std::wstring
	return std::wstring(utf16Buffer.data(), convertedSize);
}

inline std::string Utf8ToGbk(const std::string& utf8) {
	if (utf8.empty()) return "";
	// 先把 UTF-8 转成宽字符
	int wcsLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	if (wcsLen <= 0) return "";

	std::wstring wcs(wcsLen - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wcs[0], wcsLen);

	// 再把宽字符转成 GBK
	int gbkLen = WideCharToMultiByte(936, 0, wcs.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (gbkLen <= 0) return "";

	std::string gbk(gbkLen - 1, '\0');
	WideCharToMultiByte(936, 0, wcs.c_str(), -1, &gbk[0], gbkLen, nullptr, nullptr);

	return gbk;
}

inline std::string GbkToUtf8(const std::string& gbk) {
	if (gbk.empty()) return "";
	// 先把 GBK 转成宽字符
	int wcsLen = MultiByteToWideChar(936, 0, gbk.c_str(), -1, nullptr, 0);
	if (wcsLen <= 0) return "";

	std::wstring wcs(wcsLen - 1, L'\0');
	MultiByteToWideChar(936, 0, gbk.c_str(), -1, &wcs[0], wcsLen);

	// 再把宽字符转成 UTF-8
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wcs.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (utf8Len <= 0) return "";

	std::string utf8(utf8Len - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wcs.c_str(), -1, &utf8[0], utf8Len, nullptr, nullptr);

	return utf8;
}
