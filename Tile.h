#ifndef _TILE_H_
#define _TILE_H_

#include "common.h"

class Tile
{
private:
  string name;

public:
  Tile(string imageName = "imageName");
  Tile(const Tile &t);
  Tile(Tile &&t);
  ~Tile();

  void render(int x, int y, SDL_Window *window, SDL_Surface *gScreenSurface);

  // Getters and Setters
  string getName() const { return name; }
  void setName(string newName) { name = newName; }
};

#endif