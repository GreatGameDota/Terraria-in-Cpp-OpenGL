#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cmath>
#include <string>
#include <vector>

using namespace std;

class Player
{
private:
  double x;
  double y;
  string image;
  int width{60 / 2};
  int height{96 / 2};
  int screenWidth;
  int screenHeight;
  int tileSize;
  double vx;
  double vy;
  int canjump;
  int frame;
  int animationSpeed;
  double gravity;
  double jumpHeight;
  double xSpeed;

public:
  Player(int width, int height, int tileSize);

  void RespawnPlayer();
  void PositionPlayer();
  void tick(bool left, bool right, bool jump, vector<double> platformX, vector<double> platformY);
  void MovePlayer(double sx, double sy, vector<double> platformX, vector<double> platformY);
  void SetCostume();

  double getX() const { return x; }
  void setX(int newX) { x = newX; }
  double getY() const { return y; }
  void setY(int newY) { y = newY; }
  string getImage() const { return image; }
  void setImage(string newImage) { image = newImage; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }
};

#endif