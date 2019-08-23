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

Tile::~Tile(){};

void Tile::render(int x, int y, int tileSize, SDL_Surface *gScreenSurface, map<string, SDL_Surface *> images)
{
  SDL_Rect pos;
  pos.x = x;
  pos.y = y;
  // SDL_BlitSurface(images[name], NULL, gScreenSurface, &pos);
  SDL_Rect size;
  size.w = tileSize;
  size.h = tileSize;
  SDL_Surface *pScaleSurface = SDL_CreateRGBSurface(
      images[name]->flags,
      size.w,
      size.h,
      images[name]->format->BitsPerPixel,
      images[name]->format->Rmask,
      images[name]->format->Gmask,
      images[name]->format->Bmask,
      images[name]->format->Amask);
  SDL_FillRect(pScaleSurface, &size, SDL_MapRGBA(pScaleSurface->format, 255, 0, 0, 255));

  SDL_BlitScaled(images[name], NULL, pScaleSurface, NULL);
  SDL_BlitSurface(pScaleSurface, NULL, gScreenSurface, &pos);

  SDL_FreeSurface(pScaleSurface);
  pScaleSurface = nullptr;
}