#ifndef _RENDERING
#define _RENDERING

#include <string>
#include <SDL_image.h>

extern SDL_Renderer* renderer;

void render_init();
SDL_Texture* load_image(std::string s);

#endif // _RENDERING
