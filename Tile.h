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

  void render(int x, int y, int tileSize, SDL_Surface *gScreenSurface, map<string, SDL_Surface *> images);

  // Getters and Setters
  string getName() const { return name; }
  void setName(string newName) { name = newName; }
};

#endif