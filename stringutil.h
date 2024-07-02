#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>
#include <regex>
#include <string>
#include <QString>
#include <QList>
#include <regex>
class StringUtil
{
public:
    static std::regex wildchardRex(std::string pattern);
    static bool matchWildcard(QString pattern, QString parentString);

    static bool match(const QList<std::regex> &regs,QString parentString);
    static std::string replaceAll(std::string &str, const std::string& oldStr, const std::string& newStr);
private:
    StringUtil() = delete;
};

#endif // STRINGUTIL_H
