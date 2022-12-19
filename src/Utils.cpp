#include <Utils.h>

std::vector<String> splitString(const String& text, String sep) {
    String modifiedString = text;

    std::vector<String> result;
    while (modifiedString.indexOf(sep) != -1) {
        int index = modifiedString.indexOf(sep);

        String found = modifiedString.substring(0, index);
        result.push_back(found);

        modifiedString = modifiedString.substring(index + sep.length(), modifiedString.length());
    }

    if (modifiedString.length() > 0) {
        result.push_back(modifiedString);
    }

    return result;
}