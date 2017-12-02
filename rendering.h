#ifndef _RENDERING
#define _RENDERING

#include <string>
#include <SDL_image.h>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

extern SDL_Renderer* renderer;
extern const int window[2];

void render_init();
SDL_Texture* load_image(std::string s);

#endif // _RENDERING
