#ifndef __ENGINE_COMMON_GAME_OBJECT_H
#define __ENGINE_COMMON_GAME_OBJECT_H

namespace engine
{
	class c_game_object
	{
	public:

		virtual void init() {}
		virtual void update(){}
		virtual void draw(){}
		
	};
}


#endif