#include "stringutil.h"
#include <iostream>
#include <string>
#include <regex>
#include <QDebug>
#include <QList>
#include <QString>
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

bool StringUtil::matchWildcard(QString pattern, QString parentString) {

    QList<std::regex> regList;
    QList<QString> filters = pattern.split(";");
    for(QString filter: filters){
        regList.append(StringUtil::wildchardRex(filter.toStdString()));
    }

    return StringUtil::match(regList,parentString);
}

bool StringUtil::match(const QList<std::regex> &regs,QString parentString){

    std::string name = parentString.toLower().toStdString();
    for(auto reg: regs){
        if(std::regex_search(name,reg)){
            return true;
        }
    }
    return false;
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
