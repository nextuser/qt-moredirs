#include <iostream>
#include <string>
#include <regex>
 
bool matchWildcard(const std::string& pattern, const std::string& str) {
    std::regex wildcardRegex("^" + pattern + "$");
    std::regex_replace(std::back_inserter(str), pattern.begin(), pattern.end(), wildcardRegex);
    return std::regex_match(str, wildcardRegex);
}
 
int main() {
    std::string pattern = "f*o"; // 这里的*作为通配符
    std::string str = "foo";
 
    if (matchWildcard(pattern, str)) {
        std::cout << "Matched!" << std::endl;
    } else {
        std::cout << "Not matched!" << std::endl;
    }
 
    return 0;
}
