#include "Player.h"
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

bool intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
bool checkIntersection(int x, int y, vector<double> platformX, vector<double> platformY, int width, int height, int tileSize);

template <typename T>
string toStringHelper(T n)
{
  ostringstream oss;
  oss << n;
  return oss.str();
}

Player::Player(int width, int height, int size) : screenWidth{width}, screenHeight{height}, tileSize{size}
{
  RespawnPlayer();
}

void Player::tick(bool left, bool right, bool jump, vector<double> platformX, vector<double> platformY)
{
  if (left)
  {
    vx = -1 * xSpeed;
    frame += 1 / animationSpeed;
  }
  else if (right)
  {
    vx = xSpeed;
    frame += 1 / animationSpeed;
  }
  else
  {
    vx = 0;
    frame = 0;
  }
  if (canjump > 0)
  {
    canjump--;
    if (jump)
    {
      vy = jumpHeight;
    }
  }
  MovePlayer(vx, 0, platformX, platformY);
  vy += gravity;
  MovePlayer(0, vy, platformX, platformY);
  SetCostume();
  if (y > screenHeight)
  {
    RespawnPlayer();
  }
}

void Player::MovePlayer(double sx, double sy, vector<double> platformX, vector<double> platformY)
{
  y += sy;
  bool intersect = checkIntersection(x, y, platformX, platformY, width, height, tileSize);
  if (!intersect)
  {
    return;
  }
  if (sx == 0)
  {
    vy = 0;
  }
  int slope = 0;
  while (!(slope == 13 || !intersect))
  {
    y -= 0.01;
    intersect = checkIntersection(x, y, platformX, platformY, width, height, tileSize);
    slope++;
  }
  if (!intersect)
  {
    canjump = 7;
  }
  if (slope == 13)
  {
    x += sx;
    y += sy + 10;
  }
}

void Player::SetCostume()
{
  if (frame = 0)
  {
    if (canjump == 7)
    {
      image = "Player-Idle";
    }
    else
    {
      image = "Player-Falling";
    }
  }
  else
  {
    if (canjump == 7)
    {
      image = "Player-Walk-" + toStringHelper(floor(frame % 13) + 1);
    }
    else
    {
      image = "Player-Falling";
    }
  }
}

void Player::PositionPlayer()
{
  //Lighting?
}

void Player::RespawnPlayer()
{
  x = 504;
  y = 200;
  image = "Player-Idle";
  vx = 0;
  vy = 0;
  canjump = 0;
  frame = 0;
  animationSpeed = 1;
  gravity = 0.01;
  jumpHeight = 6;
  xSpeed = 2.5;
}

bool checkIntersection(int x, int y, vector<double> platformX, vector<double> platformY, int width, int height, int tileSize)
{
  for (int i = 0; i < platformX.size(); i++)
  {
    if (intersect(x, y, x + width, y + height, platformX[i], platformY[i], platformX[i] + tileSize, platformY[i] + tileSize))
    {
      return true;
    }
  }
  return false;
}

bool intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
  return x1 < x4 && x2 > x3 && y1 < y4 && y2 > y3;
}