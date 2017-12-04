#ifndef _RENDERING
#define _RENDERING

#include <string>
#include <SDL_image.h>

extern SDL_Renderer* renderer;
extern const int window[2];
extern int zoom, fullscreen;

void load_option_rendering(std::string s, std::string s2);
void render_init();
void render_init_update();
SDL_Texture* load_image(std::string s);
void render_everything(bool blink=true);

#endif // _RENDERING
