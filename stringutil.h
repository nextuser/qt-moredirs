#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>
#include <regex>
#include <string>

class StringUtil
{
public:
    static std::regex wildchardRex(std::string pattern);
    static bool matchWildcard(const std::string pattern, const std::string str);
    static std::string replaceAll(std::string &str, const std::string& oldStr, const std::string& newStr);
private:
    StringUtil() = delete;
};

#endif // STRINGUTIL_H
