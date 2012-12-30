#ifndef asset_manager_h__
#define asset_manager_h__

#include <exception>
#include <map>
using namespace std;

#include "File.h"
#include "string_tokenizer.h"

class AssetManager
{
    class FileNotFoundException : public exception {};
    
    map<string, char*> _loaded_assets;
    
    string FindFolderForFile(string file_name)
    {
        size_t folder_index = -1;
        for (size_t i = 0; i < base_folders->LineCount(); ++i)
        {
            if (File::Exists(base_folders->operator[](i) + file_name))
            {
                folder_index = i;
                break;
            }
        }
        if (folder_index == -1)
            throw FileNotFoundException();
        return base_folders->operator[](folder_index);
    }
    
    char* LoadNew(string file_name, string folder_path, bool load_as_string)
    {
        if (folder_path.length() == 0)
        {
            if (load_as_string)
                return (char *)new string(File::ReadString(FindFolderForFile(file_name) + file_name));
            else
                return File::ReadNew(FindFolderForFile(file_name) + file_name);
        }
        else
            if (File::Exists(folder_path + file_name))
            {
                if (load_as_string)
                    return (char *)new string(File::ReadString(folder_path + file_name));
                else
                    return File::ReadNew(folder_path + file_name);
            }
            else
                throw FileNotFoundException();
    }
    
public:
	StringTokenizer *base_folders;
    
	AssetManager(string folders_seperated_by_comma = "")
	{
		base_folders = new StringTokenizer(folders_seperated_by_comma, ",");
	}
    
    ~AssetManager()
    {
        for(map<string, char*>::iterator iter = _loaded_assets.begin(); iter != _loaded_assets.end(); ++iter)
            Unload(iter->first);
        delete base_folders;
    }
    
    template <class T>
    T Get(string file_name, string folder_path = "", bool load_as_string = false)
    {
        string key = folder_path + file_name;
        if (_loaded_assets.find(key) == _loaded_assets.end())
            _loaded_assets[key] = LoadNew(file_name, folder_path, load_as_string);
        return (T)_loaded_assets[key];
    }
    
    void Unload(string file_name, string folder_path = "")
    {
        string key;
        if (folder_path.length() == 0)
            key = FindFolderForFile(file_name) + file_name;
        else
            key = folder_path + file_name;
        if (_loaded_assets.find(key) == _loaded_assets.end())
            delete[] _loaded_assets[key];
    }
};

#endif
