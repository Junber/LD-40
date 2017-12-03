#ifndef _RENDERING
#define _RENDERING

#include <string>
#include <SDL_image.h>

extern SDL_Renderer* renderer;
extern const int window[2];

void load_option_rendering(std::string s, std::string s2);
void render_init();
SDL_Texture* load_image(std::string s);
void render();

#endif // _RENDERING
