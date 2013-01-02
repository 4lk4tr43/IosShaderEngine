#ifndef root_h__
#define root_h__

#include <functional>
using namespace std;

#include "asset_manager.h"
#include <mutex>

class Root
{
	map<string, char*> _object_register;

public:
	AssetManager *asset_manager;
    
	Root(string folders_seperated_by_comma = "")
	{
		asset_manager = new AssetManager(folders_seperated_by_comma);
	}
    
	~Root()
	{
        delete asset_manager;
    }
    
	template <class T>
	void RegisterObject(string object_name, T object)
	{
		_object_register[object_name] = (char*)object;
	}

	template <class T> 
	T GetObject(string object_name)
	{
		return (T)_object_register[object_name];
	}

	void UnregisterObject(string object_name)
	{
		_object_register.erase(object_name);
	}

    void Init(function<void (Root*)> init)
    {
        init(this);
    }
    
    void Render(function<void (Root*)> render)
    {
        render(this);
    }
    
    void Release(function<void (Root*)> release)
    {
        release(this);
    }
    
    void Update(function<void (Root*)> update)
    {
        update(this);
    }
};

#endif
