#include "Utilities.h"
#include <sstream>

void split_str(const std::string& str, char delimiter, std::vector<std::string>& tokens) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
}
