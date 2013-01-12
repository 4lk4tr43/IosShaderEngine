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
    StringTokenizer(string string_to_split, char delimiter = '\n')
    {
		stringstream string_stream(string_to_split);
		string item;
		while(getline(string_stream, item, delimiter))
			_lines.push_back(item);
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
