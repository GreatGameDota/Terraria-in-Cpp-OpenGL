#include "Player.h"
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

bool intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

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

void Player::tick(bool left, bool right, bool jump, vector<double> platformX, vector<double> platformY, double time)
{
  if (left)
  {
    vx = -1 * xSpeed;
    frame += 1 / animationSpeed;
    dir = "-L";
  }
  else if (right)
  {
    vx = xSpeed;
    frame += 1 / animationSpeed;
    dir = "-R";
  }
  else
  {
    vx = 0;
    frame = 0;
  }
  if (canjump > 0)
  {
    if (vy < 0)
      canjump--;
    if (jump)
    {
      vy = -1 * jumpHeight;
    }
  }
  // vx *= time * 100;
  MovePlayer(vx, 0, platformX, platformY);
  vy += gravity;
  // vy *= time * 100;
  MovePlayer(0, vy, platformX, platformY);
  SetCostume();
  if (y > screenHeight)
  {
    RespawnPlayer();
  }
}

void Player::MovePlayer(double sx, double sy, vector<double> platformX, vector<double> platformY)
{
  if (x > screenWidth - width)
  {
    x -= xSpeed;
  }
  else if (x < 0)
  {
    x += xSpeed;
  }
  else
  {
    x += sx;
  }
  y += sy;
  bool intersect = checkIntersection(platformX, platformY);
  if (!intersect)
  {
    return;
  }
  if (sx == 0)
  {
    vy = 0;
  }
  int slope = 0;
  int maxSlope = 17;
  while (!(slope == maxSlope || !intersect))
  {
    y -= 1;
    intersect = checkIntersection(platformX, platformY);
    slope++;
  }
  if (!intersect)
  {
    canjump = 7;
  }
  if (slope == maxSlope)
  {
    // cout << sy << endl;
    if (sy == 0)
    {
      y += tileSize + 10;
      if (sx > 0)
      {
        x -= sx + 2;
      }
      else if (sx < 0)
      {
        x += sx + 2;
      }
    }
    else if (sy < 0.001)
    {
      y += tileSize + 10;
      canjump = 0;
    }
  }
}

void Player::SetCostume()
{
  if (frame == 0)
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
      image = "Player-Walk-" + toStringHelper(static_cast<int>(round(frame)) % 13 + 1);
    }
    else
    {
      image = "Player-Falling";
    }
  }
  image += dir;
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
  animationSpeed = 25;
  gravity = 0.01;
  jumpHeight = 1.5;
  xSpeed = 0.3;
  dir = "-L";
}

bool Player::checkIntersection(vector<double> platformX, vector<double> platformY)
{
  double offsetW = 5;
  double offsetH = 5;
  for (int i = 0; i < platformX.size(); i++)
  {
    if (intersect(x + offsetW, y + offsetH, x + width - offsetW, y + height - 3, platformX[i], platformY[i], platformX[i] + tileSize, platformY[i] + tileSize))
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