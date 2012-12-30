#ifndef root_h__
#define root_h__

#include "asset_manager.h"

class Root
{
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
