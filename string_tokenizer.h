#ifndef string_tokenizer_h__
#define string_tokenizer_h__

#include <vector>
#include <sstream>
#include <string>
using namespace std;

class StringTokenizer
{
    vector<string> _lines;
    
public:
    StringTokenizer(string string_to_split, string seperatorString = "\n")
    {
        auto string_to_split_size = string_to_split.size();
        auto cstring = new char[string_to_split_size + 1];
        memcpy(cstring, (const void*)string_to_split.c_str(), string_to_split_size * sizeof(char));
        cstring[string_to_split_size] = '\0';
        auto cseperator = seperatorString.c_str();
        auto token = strtok(cstring, cseperator);
        while (token)
        {
            _lines.push_back(string(token));
            token = strtok(0, cseperator);
        }
        delete[] cstring;
    }
    
    size_t LineCount()
    {
        return _lines.size();
    }
    
    string operator[](unsigned long index)
    {
        return _lines[index];
    }
};

#endif
