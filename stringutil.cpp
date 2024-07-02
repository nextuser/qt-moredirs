#include "stringutil.h"
#include <iostream>
#include <string>
#include <regex>
#include <QDebug>
using namespace std;


std::regex StringUtil::wildchardRex(string pattern){
    std::string  regstr = pattern;
    replaceAll(regstr,"\\","\\\\");
    replaceAll(regstr,".","\\.");

    replaceAll(regstr,"^","\\^");
    replaceAll(regstr,"$","\\$");
    replaceAll(regstr,"*","[\\w\\W]*");
    return std::regex(regstr);
}

bool StringUtil::matchWildcard(const std::string pattern, const std::string str) {

    auto wildcardRegex = std::regex(pattern);
    return std::regex_search(str, wildcardRegex);
}

string StringUtil::replaceAll(string &str, const string& oldStr, const string& newStr){
    string::size_type pos = str.find(oldStr);
    if(oldStr.empty()) return str;
    while(pos != string::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos = str.find(oldStr,pos + newStr.length());
    }
    return str;
}
