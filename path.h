#ifndef path_h__
#define path_h__

#include <string>
using namespace std;

class Path
{
public:
	static string folder(string path)
	{
		auto pos = path.find_last_of("/");
		if (pos != string::npos)
            return path.substr(0, pos + 1);
		throw invalid_argument("Couldn't determine folder of path string.");
	}
    
	static string file(string path)
	{
		auto pos = path.find_last_of("/");
		if (pos != string::npos)
		{
			auto fileName = path.substr(pos + 1);
			if (fileName.length() != 0)	return fileName;
		}
		throw invalid_argument("Couldn't determine file of path string.");
	}
};

#endif
