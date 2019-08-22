#include "Tile.h"
#include "common.h"

Tile::Tile(string imageName) : name{imageName}
{
}

Tile::Tile(const Tile &t) : Tile(t.name)
{
}

Tile::Tile(Tile &&t) : name{t.name}
{
}

Tile::~Tile()
{
}

void Tile::render(int x, int y, SDL_Window *window, SDL_Surface *gScreenSurface, map<string, SDL_Surface *> images)
{
  SDL_Rect pos;
  pos.x = x;
  pos.y = y;
  SDL_BlitSurface(images[name], NULL, gScreenSurface, &pos);
  SDL_UpdateWindowSurface(window);
}