#ifndef _DIALOG
#define _DIALOG

#include <string>
#include <SDL.h>

void load_drinks();
bool visual_novel(SDL_Texture* tex, std::string text, std::string text2, bool player);
void dialog(std::string file_name, SDL_Texture* tex);

#endif // _DIALOG
