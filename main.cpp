#include "common.h"
#include "util/TimeElapsed.h"
#include "OpenSimplexNoise.h"
#include "Player.h"

/*
    Game by GreatGameDota
    https://github.com/GreatGameDota
*/

int width = 1008;
int height = 608;
int tileSize = 16;
int amountX = width / tileSize;
int amountY = height / tileSize;

bool init();
void CleanUp();
void Run();
bool loadAllImages();
void loadSurface(string path);
SDL_Surface *screen = nullptr;
void renderImage(double x, double y, string name);
map<string, SDL_Surface *> images;

void InitialWorldGen();
void RenderAll();
void AddCustomTiles();
bool rendering = false;
vector<int> Ground;
vector<int> Background;
string groundNames[] = {
    "blank",
    "grass",
    "dirt",
    "stone",
    "flora",
    "wood",
    "torch",
    "tree",
    "tree-b-left",
    "tree-b-right",
    "tree-b-both",
    "tree-branch-right",
    "tree-branch-left",
    "tree-r-left",
    "tree-r-right",
    "tree-r-both",
    "tree-root-right",
    "tree-root-left", // 17
    "tree-top-1",     // 18
    "tree-top",       // 19
    "tree-left-1",    // 20
    "tree-left-2",    // 21
    "tree-left-3",    // 22
    "tree-right-1",   // 23
    "tree-right-2",   // 24
    "tree-right-3",   // 25
    "grass-corner"};
string backgroundNames[] = {"dirt-wall", "stone-wall", "wood-wall", "sky"};
void RenderGroundAtIndex(int idx);
void GetScaledXYFromIndex(int idx);
int GetIndexFromScaledXY(int x, int y);
int GetIndexFromXY(double x, double y);
void GetRealXYFromScaledXY(int x, int y);
double ScaleNum(double n, double minN, double maxN, double min, double max);

OpenSimplexNoise *noise1 = nullptr;
OpenSimplexNoise *noise2 = nullptr;
OpenSimplexNoise *noise3 = nullptr;

int gen = 0;
int feature_size = 100;
int worldXOffset = 0;
int worldYOffset = 0;
vector<double> platformX;
vector<double> platformY;
vector<int> customTileY;
vector<int> customTileScrollX;
vector<int> customTile;
int screens = 31;
bool initialGen = false;
void Generate();
void GenerateGroundAtIndex(int idx);
void GenerateSurface();
vector<int> surface;
void RenderSurface();
void CenterMiddle();
void GenSurfaceAtScaledX(int x);
string CheckNeighbors(int x, int y, string type);
string CheckForEmptyTile(int idx, string type);
void RegenEntireWorld();
template <typename T>
string toStringHelper(T n)
{
    ostringstream oss;
    oss << n;
    return oss.str();
}

int scaled[2];
double pos[2];

bool mouseDown = false;
void digBlock(int x, int y);
void placeBlock(int x, int y, int type);
void placeTorch(int x, int y);
void Rerender(int idx);
vector<int> todo;
vector<int> oldRandomImageG;
vector<int> oldRandomImageB;

Player Player{width, height, tileSize};
void RenderPlayer();
void ClearPlayer();
void RerenderAroundPlayer();

double previousTime;
double currentTime = 0;
double elapsedTime;

bool arrowUp = false;
bool arrowLeft = false;
bool arrowRight = false;
bool wKey = false;
bool aKey = false;
bool dKey = false;
bool space = false;

vector<double> light;
deque<double> nextLight;
vector<int> SunLight;
void CastLight();
void SpreadLight(int x, int y, double lightVal);
void CastSunlight();
void AddLightSource(int idx, double value);
void RenderBrightness(double brightness, double x, double y, int width, int height);
double GetBrightness(int tile, int idx);
vector<int> torches;
double lightSourceValue = 25;
double torchLightValue = 50;

void GenerateTreeAtXY(int x, int y);
void RenderObject(double x, double y, int width, int height, string name, int offsetX, int offsetY);
void RenderObjects();
vector<double> ObjectWidth{80, 30, 32, 26, 30, 34, 34};
vector<double> ObjectHeight{80, 28, 24, 28, 28, 28, 24};
vector<int> ObjectOffsetX{32, 14, 16, 10, 0, 0, 0};
vector<int> ObjectOffsetY{64, 12, -2, 7, 14, 12, -2};
bool initialLoop = false;
bool reRender = false;
double TwoDDist(double x1, double y1, double x2, double y2);

void printToScreen(int x, int y, unsigned char r, unsigned char g, unsigned char b, char *str);
TTF_Font *Font;
int FPS = 0;
const string VERSION = "v0.8.1 (Alpha)";
void DisplayText();
char *toCharArray(int number, char *numberArray);

SDL_Window *window = nullptr;
SDL_GLContext glContext;
// unsigned int WindowFlags;

bool init()
{
    // WindowFlags = SDL_WINDOW_OPENGL;
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }
    //Create Window Instance
    window = SDL_CreateWindow(
        "Terraria in OpenGL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return false;
        }
        else
        {
            screen = SDL_GetWindowSurface(window);
        }
        SDL_Log("Window Successful Generated");
    }
    TTF_Init();
    Font = TTF_OpenFont("Fonts/Andy Bold.ttf", 18);
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);
    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!init())
    {
        printf("Failed to Initialize");
    }
    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //Swap Render Buffers
    SDL_GL_SwapWindow(window);

    if (!loadAllImages())
    {
        SDL_Log("Failed to load images");
    }
    else
    {
        SDL_Log("Successfully loaded images");
    }

    srand(time(NULL));
    long rand1 = rand() * (RAND_MAX + 1) + rand();
    long rand2 = rand() * (RAND_MAX + 1) + rand();
    long rand3 = rand() * (RAND_MAX + 1) + rand();
    noise1 = new OpenSimplexNoise{rand1};
    noise2 = new OpenSimplexNoise{rand2};
    noise3 = new OpenSimplexNoise{rand3};

    InitialWorldGen();

    // startTimer();

    Run();

    CleanUp();

    system("pause");
    return 0;
}

void CleanUp()
{
    for (auto &image : images)
    {
        SDL_FreeSurface(image.second);
    }
    delete noise1;
    delete noise2;
    delete noise3;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void InitialWorldGen()
{
    initialGen = true;
    for (int i = 0; i < amountX * amountY; i++)
    {
        Ground.push_back(2);
        Background.push_back(0);
        todo.push_back(i);
        oldRandomImageG.push_back(0);
        oldRandomImageB.push_back(0);
        light.push_back(-100);
    }
    gen = 0;
    feature_size = 100;
    Generate();
    gen = 1;
    feature_size = 200;
    Generate();
    gen = 2;
    feature_size = 100;
    GenerateSurface();
    initialGen = false;
}

void RenderAll()
{
    AddCustomTiles();
    if (todo.size() >= amountX * amountY)
    {
        CastSunlight();
    }
    rendering = true;
    platformX.clear();
    platformY.clear();
    for (int i = 0; i < todo.size(); i++)
    {
        RenderGroundAtIndex(todo[i]);
    }
    todo.clear();
    rendering = false;
}

void RenderGroundAtIndex(int idx)
{
    GetScaledXYFromIndex(idx);
    GetRealXYFromScaledXY(scaled[0], scaled[1]);
    if (pos[0] > -1 && pos[1] > -1 && pos[0] < width && pos[1] < height)
    {
        int randTile = 0;
        if (Background[idx] != 3)
        {
            string name = "";
            string shape = CheckNeighbors(scaled[0], scaled[1], "background");
            if (shape != "xxxx")
            {
                renderImage(pos[0], pos[1], "sky");
            }
            name = backgroundNames[Background[idx]] + "-" + shape;
            if (shape == "xxxx")
            {
                randTile = rand() % 3 + 1;
                name += toStringHelper(randTile);
            }
            if (shape == "xx0x")
            {
                randTile = rand() % 2 + 1;
                name += toStringHelper(randTile);
            }
            if (shape == "000x")
            {
                randTile = rand() % 2 + 1;
                name = backgroundNames[Background[idx]] + "-xx0x" + toStringHelper(randTile);
            }
            if (shape == "00xx" || shape == "00x0" || shape == "xx00")
            {
                randTile = rand() % 3 + 1;
                name = backgroundNames[Background[idx]] + "-xxxx" + toStringHelper(randTile);
            }
            if (shape == "x000")
            {
                name = backgroundNames[Background[idx]] + "-x0xx";
            }
            if (shape == "0x00")
            {
                name = backgroundNames[Background[idx]] + "-0xxx";
            }
            //Brightness
            if (oldRandomImageB[idx] != name.back() - '0' && name.back() != '0' && name.back() != 'x' && oldRandomImageB[idx] != 0)
            {
                name.back() = '0' + oldRandomImageB[idx];
            }
            else
            {
                if (name.back() - '0' > 0 && name.back() != '0' && name.back() != 'x')
                {
                    oldRandomImageB[idx] = name.back() - '0';
                }
            }
            renderImage(pos[0], pos[1], name);
            RenderBrightness(GetBrightness(Background[idx], idx), pos[0], pos[1], tileSize, tileSize);
        }
        else
        {
            if (Ground[idx] < 19 || Ground[idx] > 25)
            {
                renderImage(pos[0], pos[1], "sky");
            }
        }
        if (Ground[idx] != 0)
        {
            string name = "";
            string shape = CheckNeighbors(scaled[0], scaled[1], "ground");
            name = groundNames[Ground[idx]] + "-" + shape;
            if (Ground[idx] == 2 || Ground[idx] == 3 || Ground[idx] == 5 || Ground[idx] == 1)
            {
                if (shape == "xxxx")
                {
                    randTile = rand() % 3 + 1;
                    name += toStringHelper(randTile);
                }
            }
            if (Ground[idx] == 1 || Ground[idx] == 3 || Ground[idx] == 5)
            {
                if (shape == "xx0x")
                {
                    randTile = rand() % 3 + 1;
                    name += toStringHelper(randTile);
                }
            }
            if (Ground[idx] == 6)
            {
                name = "torch";
            }
            if (Ground[idx] > 6 && Ground[idx] < 19)
            {
                renderImage(pos[0], pos[1], "sky");
                if (Ground[idx] > 7)
                {
                    name = groundNames[Ground[idx]];
                }
                else
                {
                    name = "tree-";
                    randTile = rand() % 3 + 3;
                    name += toStringHelper(randTile);
                }
            }
            if (oldRandomImageG[idx] != name.back() - '0' && name.back() != '0' && name.back() != 'x' && oldRandomImageG[idx] != 0)
            {
                name.back() = '0' + oldRandomImageG[idx];
            }
            else
            {
                if (name.back() - '0' > 0 && name.back() != '0' && name.back() != 'x')
                {
                    oldRandomImageG[idx] = name.back() - '0';
                }
            }
            if (Ground[idx] < 19 || Ground[idx] > 25)
            {
                renderImage(pos[0], pos[1], name);
            }
            if (shape != "xxxx" && Ground[idx] < 6 && Ground[idx] != 0)
            {
                platformX.push_back(pos[0]);
                platformY.push_back(pos[1]);
            }
            if (Ground[idx] < 7)
            {
                RenderBrightness(GetBrightness(Ground[idx], idx), pos[0], pos[1], tileSize, tileSize);
            }
        }
    }
}

void RenderObjects()
{
    for (int i = 0; i < Ground.size(); i++)
    {
        if (Ground[i] > 19 && Ground[i] < 26)
        {
            GetScaledXYFromIndex(i);
            GetRealXYFromScaledXY(scaled[0], scaled[1]);
            if (pos[0] > -1 && pos[1] > -1 && pos[0] < width && pos[1] < height)
            {
                string name = groundNames[Ground[i]];
                int objectNum = Ground[i] - 19;
                if (reRender || TwoDDist(Player.getX() + Player.getWidth() / 2, Player.getY() + Player.getHeight() / 2, pos[0] - ObjectOffsetX[objectNum] + ObjectWidth[objectNum] / 2, pos[1] - ObjectOffsetY[objectNum] + ObjectHeight[objectNum] / 2) < 100)
                {
                    renderImage(pos[0], pos[1], "sky");
                    RenderObject(pos[0], pos[1], ObjectWidth[objectNum], ObjectHeight[objectNum], name, ObjectOffsetX[objectNum], ObjectOffsetY[objectNum]);
                }
            }
        }
    }
    for (int i = 0; i < Ground.size(); i++)
    {
        if (Ground[i] == 19)
        {
            GetScaledXYFromIndex(i);
            GetRealXYFromScaledXY(scaled[0], scaled[1]);
            if (pos[0] > -1 && pos[1] > -1 && pos[0] < width && pos[1] < height)
            {
                string name = groundNames[Ground[i]];
                int objectNum = Ground[i] - 19;
                if (reRender || TwoDDist(Player.getX() + Player.getWidth() / 2, Player.getY() + Player.getHeight() / 2, pos[0] - ObjectOffsetX[objectNum] + ObjectWidth[objectNum] / 2, pos[1] - ObjectOffsetY[objectNum] + ObjectHeight[objectNum] / 2) < 100)
                {
                    renderImage(pos[0], pos[1], "sky");
                    RenderObject(pos[0], pos[1], ObjectWidth[objectNum], ObjectHeight[objectNum], name, ObjectOffsetX[objectNum], ObjectOffsetY[objectNum]);
                }
            }
        }
    }
}

void Generate()
{
    for (int i = 0; i < amountX * amountY; i++)
    {
        GenerateGroundAtIndex(i);
    }
}

void GenerateGroundAtIndex(int idx)
{
    GetScaledXYFromIndex(idx);
    GetRealXYFromScaledXY(scaled[0], scaled[1]);
    double val = 0;
    if (gen == 0)
    {
        val = (*noise1).eval((pos[0] + (worldXOffset * tileSize)) / feature_size, (pos[1] + (worldYOffset * tileSize)) / feature_size);
    }
    else if (gen == 1)
    {
        val = (*noise2).eval((pos[0] + (worldXOffset * tileSize)) / feature_size, (pos[1] + (worldYOffset * tileSize)) / feature_size);
    }
    else
    {
    }
    double newVal = ScaleNum(val, -1, 1, 0, 100);
    if (gen == 0)
    {
        Background[idx] = 0;
        if (newVal < 15 || newVal > 85)
        {
            Ground[idx] = 3;
        }
        else
        {
            Ground[idx] = 2;
        }
    }
    else if (gen == 1)
    {
        if (newVal < 15 || newVal > 80)
        {
            Ground[idx] = 0;
        }
        else
        {
        }
    }
    else
    {
    }
}

void GenerateSurface()
{
    GetRealXYFromScaledXY(1, rand() % 10 + 15);
    int surfaceX = ceil(screens / 2);
    int surfaceY = pos[1];
    for (int i = 0; i < amountX * screens; i++)
    {
        double val = (*noise3).eval(surfaceX / feature_size, 0);
        surfaceY += val * tileSize;
        int idx = GetIndexFromXY(-4, surfaceY);
        GetScaledXYFromIndex(idx);
        GetRealXYFromScaledXY(scaled[0], scaled[1]);
        surface.push_back(pos[1]);
        surfaceX += tileSize;
    }
    RenderSurface();
    CenterMiddle();
    for (int i = 3; i < surface.size() - 3; i += 3)
    {
        int y = surface[i];
        if (surface[i - 1] == y && surface[i + 1] == y)
        {
            double r = static_cast<double>(rand()) / RAND_MAX;
            if (r > 0.7)
            {
                GenerateTreeAtXY(i, y - tileSize);
            }
        }
    }
    // GenerateTreeAtXY((amountX * screens) / 2, surface[(amountX * screens) / 2]);
}

void GenerateTreeAtXY(int x, int y)
{
    customTileY.push_back(y);
    customTileScrollX.push_back(x);
    int rootNum = rand() % 4;
    if (rootNum == 0)
    {
        customTile.push_back(7);
    }
    else if (rootNum == 1)
    {
        customTile.push_back(13);
        customTileY.push_back(y);
        customTileScrollX.push_back(x - 1);
        customTile.push_back(17);
    }
    else if (rootNum == 2)
    {
        customTile.push_back(15);
        customTileY.push_back(y);
        customTileScrollX.push_back(x - 1);
        customTile.push_back(17);
        customTileY.push_back(y);
        customTileScrollX.push_back(x + 1);
        customTile.push_back(16);
    }
    else if (rootNum == 3)
    {
        customTile.push_back(14);
        customTileY.push_back(y);
        customTileScrollX.push_back(x + 1);
        customTile.push_back(16);
    }
    int treeHeight = rand() % (15 - 5 + 1) + 5;
    for (int i = 0; i < treeHeight; i++)
    {
        customTileY.push_back(y - (tileSize * (i + 1)));
        customTileScrollX.push_back(x);
        if (rand() % 11 > 7 && i < treeHeight - 1 && i % 2 == 0 && i != 0)
        {
            int branchType = rand() % 3 + 1;
            if (branchType == 1)
            {
                customTile.push_back(8);
                customTileY.push_back(y - (tileSize * (i + 1)));
                customTileScrollX.push_back(x - 1);
                if (rand() % 3 + 1 == 3)
                {
                    customTile.push_back(12);
                }
                else
                {
                    customTile.push_back(rand() % 3 + 20);
                }
            }
            else if (branchType == 2)
            {
                customTile.push_back(9);
                customTileY.push_back(y - (tileSize * (i + 1)));
                customTileScrollX.push_back(x + 1);
                if (rand() % 3 + 1 == 3)
                {
                    customTile.push_back(11);
                }
                else
                {
                    customTile.push_back(rand() % 3 + 23);
                }
            }
            else if (branchType == 3)
            {
                customTile.push_back(10);
                customTileY.push_back(y - (tileSize * (i + 1)));
                customTileScrollX.push_back(x - 1);
                if (rand() % 3 + 1 == 3)
                {
                    customTile.push_back(12);
                }
                else
                {
                    customTile.push_back(rand() % 3 + 20);
                }
                customTileY.push_back(y - (tileSize * (i + 1)));
                customTileScrollX.push_back(x + 1);
                if (rand() % 3 + 1 == 3)
                {
                    customTile.push_back(11);
                }
                else
                {
                    customTile.push_back(rand() % 3 + 23);
                }
            }
        }
        else
        {
            if (i == treeHeight - 1)
            {
                customTile.push_back(19);
            }
            else
            {
                customTile.push_back(7);
            }
        }
    }
}

void RenderSurface()
{
    SunLight.clear();
    for (int i = 0; i < amountX; i++)
    {
        GenSurfaceAtScaledX(i);
    }
}

void GenSurfaceAtScaledX(int x)
{
    int idx = x + worldXOffset + (amountX * screens) / 2 - amountX / 2;
    GetRealXYFromScaledXY(x, 1);
    int posX = pos[0];
    int posY = surface[idx] + worldYOffset * -1 * tileSize;
    if (posX > -1 && posY > -1 && posX < width - tileSize / 2 && posY < height - tileSize / 2)
    {
        int index = GetIndexFromXY(posX, posY);
        if (Ground[index] == 2)
        {
            Ground[index] = 1;
        }
        else
        {
            Ground[index] = Ground[index] + 100;
        }
        Background[index] = 3;
    }
    if (posY > height - tileSize / 2 || (posX > -1 && posY > -1 && posX < width - tileSize / 2 && posY < height - tileSize / 2))
    {
        int idx2 = x;
        while (!(Ground[idx2] == 1 || Ground[idx2] > 99 || idx2 > amountX * amountY))
        {
            Ground[idx2] = 0;
            Background[idx2] = 3;
            idx2 += amountX;
        }
        SunLight.push_back(idx2);
        if (Ground[idx2] > 99 && !(idx2 > amountX * amountY))
        {
            Ground[idx2] = Ground[idx2] - 100;
        }
        idx2 += amountX;
        if (Ground[idx2] == 0 && !(idx2 > amountX * amountY))
        {
            Ground[idx2 - amountX] = 0;
            Background[idx2 - amountX] = 3;
            SunLight[SunLight.size() - 1] = idx2;
        }
        else
        {
            idx2 += amountX;
            if (Ground[idx2] == 0 && !(idx2 > amountX * amountY))
            {
                Ground[idx2 - 2 * amountX] = 0;
                Background[idx2 - 2 * amountX] = 3;
                SunLight[SunLight.size() - 1] = idx2;
            }
        }
    }
}

void CenterMiddle()
{
    int idx = ceil(amountX / 2);
    if (Ground[idx] > 1)
    {
        worldYOffset -= amountY + 2;
        RegenEntireWorld();
        CenterMiddle();
    }
    else
    {
        int i = 1;
        int center = ceil(amountY / 2) + 2;
        while (Background[idx] == 3 && !(i > amountY - 1))
        {
            idx += amountX;
            i++;
        }
        if (i == center)
        {
            return;
        }
        else
        {
            if (i > amountY)
            {
                worldYOffset += amountY - 2;
                RegenEntireWorld();
                CenterMiddle();
            }
            else if (i > center)
            {
                worldYOffset += i - center;
                RegenEntireWorld();
                CenterMiddle();
            }
            else
            {
                worldYOffset -= center - i;
                RegenEntireWorld();
                CenterMiddle();
            }
        }
    }
    if (!initialGen)
    {
        RenderAll();
    }
}

void RegenEntireWorld()
{
    gen = 0;
    feature_size = 100;
    Generate();
    gen = 1;
    feature_size = 200;
    Generate();
    RenderSurface();
    if (!initialGen)
    {
        oldRandomImageG.clear();
        oldRandomImageB.clear();
        int index = 0;
        for (auto &element : Ground)
        {
            todo.push_back(index++);
            oldRandomImageG.push_back(0);
            oldRandomImageB.push_back(0);
        }
    }
}

string CheckNeighbors(int x, int y, string type)
{
    string shape = "";
    if (x > 0)
    {
        int idx = GetIndexFromScaledXY(x - 1, y);
        shape += CheckForEmptyTile(idx, type);
    }
    else
    {
        shape += "x";
    }
    if (x < amountX - 1)
    {
        int idx = GetIndexFromScaledXY(x + 1, y);
        shape += CheckForEmptyTile(idx, type);
    }
    else
    {
        shape += "x";
    }
    if (y > 0)
    {
        int idx = GetIndexFromScaledXY(x, y - 1);
        shape += CheckForEmptyTile(idx, type);
    }
    else
    {
        shape += "x";
    }
    if (y < amountY - 1)
    {
        int idx = GetIndexFromScaledXY(x, y + 1);
        shape += CheckForEmptyTile(idx, type);
    }
    else
    {
        shape += "x";
    }
    return shape;
}

string CheckForEmptyTile(int idx, string type)
{
    if (type == "ground")
    {
        if (Ground[idx] == 0 || Ground[idx] >= 6)
        {
            return "0";
        }
        else
        {
            return "x";
        }
    }
    else if (type == "background")
    {
        if (Background[idx] == 3)
        {
            return "0";
        }
        else
        {
            return "x";
        }
    }
    else
    {
        return "x";
    }
}

void GetScaledXYFromIndex(int idx)
{
    scaled[0] = idx % amountX;
    scaled[1] = idx / amountX;
}

int GetIndexFromScaledXY(int x, int y)
{
    return x + y * amountX;
}

int GetIndexFromXY(double x, double y)
{
    int mouseX = floor(ScaleNum(x, 0, width, 0, amountX));
    int mouseY = round(ScaleNum(y, 0, height, 0, amountY));
    scaled[0] = mouseX;
    scaled[1] = mouseY;
    return GetIndexFromScaledXY(mouseX, mouseY);
}

void GetRealXYFromScaledXY(int x, int y)
{
    pos[0] = round(ScaleNum(x, 0, amountX, 0, width));
    pos[1] = round(ScaleNum(y, 0, amountY, 0, height));
}

double ScaleNum(double n, double minN, double maxN, double min, double max)
{
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}

double TwoDDist(double x1, double y1, double x2, double y2)
{
    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

void CastLight()
{
    int i = 0;
    while (!(nextLight.size() < 1 || i > 200))
    {
        int idx = nextLight[1];
        nextLight.pop_front();
        int tile = Ground[idx];
        double lightVal = light[idx];
        int background = Background[idx];
        if (tile != -1)
        {
            lightVal -= 3;
            if (tile > 0 && tile < 6 || background != 3)
            {
                lightVal -= 13;
            }
            GetScaledXYFromIndex(idx);
            SpreadLight(scaled[0] + 1, scaled[1] + 0, lightVal);
            SpreadLight(scaled[0] + 0, scaled[1] + 1, lightVal);
            SpreadLight(scaled[0] + -1, scaled[1] + 0, lightVal);
            SpreadLight(scaled[0] + 0, scaled[1] + -1, lightVal);
            if (tile == 0 && background < 3)
            {
                lightVal += 12;
            }
            else
            {
                lightVal -= 10;
            }
            SpreadLight(scaled[0] + 1, scaled[1] + 1, lightVal);
            SpreadLight(scaled[0] + -1, scaled[1] + -1, lightVal);
            SpreadLight(scaled[0] + -1, scaled[1] + 1, lightVal);
            SpreadLight(scaled[0] + 1, scaled[1] + -1, lightVal);
        }
        if ((tile != 0 || background != 3) && lightVal > -85)
        {
            RenderGroundAtIndex(idx);
        }
        i++;
    }
}

void SpreadLight(int x, int y, double lightVal)
{
    int idx = GetIndexFromScaledXY(x, y);
    if (lightVal > light[idx] && lightVal > -85)
    {
        if (x > -1 && x < amountX && y > -1 && y < amountY)
        {
            light[idx] = lightVal;
            nextLight.push_back(idx);
        }
    }
}

void CastSunlight()
{
    for (int i = 0; i < amountX * amountY; i++)
    {
        light[i] = -100;
    }
    nextLight.clear();
    for (int i = 0; i < SunLight.size(); i++)
    {
        AddLightSource(SunLight[i] - amountX, lightSourceValue);
    }
    for (int i = 0; i < torches.size(); i++)
    {
        AddLightSource(torches[i], torchLightValue);
    }
}

void AddLightSource(int idx, double value)
{
    if (light[idx] != value)
    {
        light[idx] = value;
        nextLight.push_back(idx);
    }
}

double GetBrightness(int tile, int idx)
{
    double brightness = light[idx] - 25;
    if (brightness < -95)
    {
        brightness = -100;
    }
    if (brightness > 0)
    {
        brightness = 0;
    }
    return ScaleNum(brightness, -100, 0, 0, 100);
}

void Run()
{
    initialLoop = true;
    reRender = false;
    bool gameLoop = true;
    bool fullScreen = false;
    int loopAmount = 0;
    int frames = 0;
    double accum = 0;
    double updateInterval = 10;
    double timeLeft = 10;
    while (gameLoop)
    {
        if (mouseDown)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            // digBlock(x, y);
            // placeBlock(x, y, 2);
            placeTorch(x, y);
        }
        int xBorder = 10;
        int yBorder = 5;
        if (Player.getX() > width - (amountX / xBorder) * tileSize)
        {
            worldXOffset += amountX - 6 - amountX / xBorder;
            RegenEntireWorld();
            Player.setX((amountX / xBorder) * tileSize);
        }
        if (Player.getX() < (amountX / xBorder) * tileSize)
        {
            worldXOffset -= amountX - 6 - amountX / xBorder;
            RegenEntireWorld();
            Player.setX(width - (amountX / xBorder) * tileSize);
        }
        if (Player.getY() > height - (amountY / yBorder) * tileSize)
        {
            worldYOffset += amountY - 10 - amountY / yBorder;
            RegenEntireWorld();
            Player.setY((amountY / yBorder) * tileSize);
        }
        if (Player.getY() < (amountY / yBorder) * tileSize)
        {
            worldYOffset -= amountY - 10 - amountY / yBorder;
            RegenEntireWorld();
            Player.setY(height - (amountY / yBorder) * tileSize);
        }

        previousTime = currentTime;
        currentTime = finish();
        startTimer();
        elapsedTime = currentTime - previousTime;
        timeLeft -= elapsedTime;
        accum += 1 / elapsedTime;
        frames++;

        if (timeLeft <= 0.0)
        {
            FPS = accum / frames;
            timeLeft = updateInterval;
            accum = 0.0;
            frames = 0;
        }

        reRender = todo.size() >= amountX * amountY;
        CastLight();
        ClearPlayer();
        RenderAll();
        RerenderAroundPlayer();
        RenderObjects();
        RenderPlayer();
        DisplayText();
        SDL_UpdateWindowSurface(window);

        currentTime = finish();

        initialLoop = false;
        loopAmount++;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    gameLoop = false;
                    break;
                case SDLK_UP:
                    arrowUp = true;
                    break;
                case SDLK_LEFT:
                    arrowLeft = true;
                    break;
                case SDLK_RIGHT:
                    arrowRight = true;
                    break;
                case SDLK_w:
                    wKey = true;
                    break;
                case SDLK_a:
                    aKey = true;
                    break;
                case SDLK_d:
                    dKey = true;
                    break;
                case SDLK_SPACE:
                    space = true;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    arrowUp = false;
                    break;
                case SDLK_LEFT:
                    arrowLeft = false;
                    break;
                case SDLK_RIGHT:
                    arrowRight = false;
                    break;
                case SDLK_w:
                    wKey = false;
                    break;
                case SDLK_a:
                    aKey = false;
                    break;
                case SDLK_d:
                    dKey = false;
                    break;
                case SDLK_SPACE:
                    space = false;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                mouseDown = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                mouseDown = false;
            }
        }
    }
}

void AddCustomTiles()
{
    torches.clear();
    for (int j = 0; j < customTile.size(); j++)
    {
        for (int i = 0; i < amountX; i++)
        {
            if (i + worldXOffset + ((amountX * screens) / 2 - amountX / 2) == customTileScrollX[j])
            {
                GetRealXYFromScaledXY(i, 0);
                pos[1] = customTileY[j] + (worldYOffset * -1 * tileSize);
                if (pos[0] > -1 && pos[1] > -1 && pos[0] < width && pos[1] < height)
                {
                    int idx = GetIndexFromXY(pos[0], pos[1]);
                    Ground[idx] = customTile[j];
                    if (customTile[j] == 6)
                    {
                        torches.push_back(idx);
                    }
                }
            }
        }
    }
}

void DisplayText()
{
    string s = toStringHelper(FPS) + " fps";
    char *pw1 = new char[30];
    strcpy(pw1, s.c_str());
    printToScreen(5, height - 18, 255, 255, 255, pw1);
    delete[] pw1;

    char *pw2 = new char[30];
    strcpy(pw2, VERSION.c_str());
    printToScreen(width / 2, 0, 255, 255, 255, pw2);
    delete[] pw2;

    todo.push_back(GetIndexFromScaledXY(0, amountY - 1));
    todo.push_back(GetIndexFromScaledXY(0, amountY - 1) + 1);
    todo.push_back(GetIndexFromScaledXY(0, amountY - 1) + 2);
    if (FPS > 99)
    {
        todo.push_back(GetIndexFromScaledXY(0, amountY - 1) + 3);
    }
}

void ClearPlayer()
{
    SDL_Rect fill{static_cast<int>(Player.getX()), static_cast<int>(Player.getY()), Player.getWidth(), Player.getHeight()};
    SDL_Surface *fill_surf = SDL_CreateRGBSurface(0, fill.w, fill.h,
                                                  screen->format->BitsPerPixel,
                                                  screen->format->Rmask,
                                                  screen->format->Gmask,
                                                  screen->format->Bmask,
                                                  screen->format->Amask);
    SDL_FillRect(fill_surf, &fill, 0x00000000);
    SDL_BlitSurface(fill_surf, NULL, screen, &fill);
    SDL_FreeSurface(fill_surf);
}

void RerenderAroundPlayer()
{
    int idx = GetIndexFromXY(static_cast<int>(Player.getX()) + Player.getWidth() / 2, static_cast<int>(Player.getY()) + Player.getHeight() / 2);
    Rerender(idx);
    todo.push_back(idx + 1 + amountX);
    todo.push_back(idx - 1 + amountX);
    todo.push_back(idx + 1 - amountX);
    todo.push_back(idx - 1 - amountX);
    todo.push_back(idx - amountX * 2);
    todo.push_back(idx - 1 - amountX * 2);
    todo.push_back(idx + 1 - amountX * 2);
}

void digBlock(int x, int y)
{
    int idx = GetIndexFromXY(x, y);
    if (Ground[idx] != 0)
    {
        Ground[idx] = 0;
        Rerender(idx);
        GetRealXYFromScaledXY(scaled[0], scaled[1]);
        customTileY.push_back(pos[1] + worldYOffset * tileSize);
        customTileScrollX.push_back(scaled[0] + worldXOffset + ((amountX * screens) / 2 - amountX / 2));
        customTile.push_back(0);
    }
}

void placeBlock(int x, int y, int type)
{
    int idx = GetIndexFromXY(x, y);
    if (Ground[idx] == 0)
    {
        Ground[idx] = type;
        Rerender(idx);
        GetRealXYFromScaledXY(scaled[0], scaled[1]);
        customTileY.push_back(pos[1] + worldYOffset * tileSize);
        customTileScrollX.push_back(scaled[0] + worldXOffset + ((amountX * screens) / 2 - amountX / 2));
        customTile.push_back(0);
    }
}

void placeTorch(int x, int y)
{
    int idx = GetIndexFromXY(x, y);
    if (Ground[idx] == 0 && (Background[idx] != 3 || (Ground[idx + amountX] != 0 && Ground[idx + amountX] < 6)))
    {
        Ground[idx] = 6;
        GetRealXYFromScaledXY(scaled[0], scaled[1]);
        customTileY.push_back(pos[1] + worldYOffset * tileSize);
        customTileScrollX.push_back(scaled[0] + worldXOffset + ((amountX * screens) / 2 - amountX / 2));
        customTile.push_back(6);
        AddLightSource(idx, torchLightValue);
    }
}

void Rerender(int idx)
{
    todo.push_back(idx);
    todo.push_back(idx - 1);
    todo.push_back(idx + 1);
    todo.push_back(idx - amountX);
    todo.push_back(idx + amountX);
    nextLight.push_back(idx);
    nextLight.push_back(idx - 1);
    nextLight.push_back(idx + 1);
    nextLight.push_back(idx - amountX);
    nextLight.push_back(idx + amountX);
}

void RenderPlayer()
{
    Player.tick(arrowLeft || aKey, arrowRight || dKey, arrowUp || wKey || space, platformX, platformY, elapsedTime);
    SDL_Rect pos;
    pos.x = Player.getX();
    pos.y = Player.getY();
    SDL_Rect size;
    size.w = Player.getWidth();
    size.h = Player.getHeight();
    SDL_Surface *pScaleSurface = SDL_CreateRGBSurface(
        images[Player.getImage()]->flags,
        size.w,
        size.h,
        images[Player.getImage()]->format->BitsPerPixel,
        images[Player.getImage()]->format->Rmask,
        images[Player.getImage()]->format->Gmask,
        images[Player.getImage()]->format->Bmask,
        images[Player.getImage()]->format->Amask);
    SDL_FillRect(pScaleSurface, &size, SDL_MapRGBA(pScaleSurface->format, 0, 0, 0, 0));
    SDL_BlitScaled(images[Player.getImage()], NULL, pScaleSurface, NULL);
    SDL_BlitSurface(pScaleSurface, NULL, screen, &pos);

    SDL_FreeSurface(pScaleSurface);
    pScaleSurface = nullptr;
}

void printToScreen(int x, int y, unsigned char r, unsigned char g, unsigned char b, char *str)
{
    SDL_Rect pos;
    pos.y = y;
    SDL_Color color = {r, g, b};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Font, str, color);
    if (x > 10)
    {
        pos.x = x - surfaceMessage->w / 2;
    }
    else
    {
        pos.x = x;
    }
    SDL_BlitSurface(surfaceMessage, NULL, screen, &pos);

    SDL_FreeSurface(surfaceMessage);
    surfaceMessage = nullptr;
}

void RenderBrightness(double brightness, double x, double y, int width, int height)
{
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    SDL_Surface *s = SDL_CreateRGBSurface(0, width, height,
                                          images["dirt-0000"]->format->BitsPerPixel,
                                          images["dirt-0000"]->format->Rmask,
                                          images["dirt-0000"]->format->Gmask,
                                          images["dirt-0000"]->format->Bmask,
                                          images["dirt-0000"]->format->Amask);
    int alpha = round(ScaleNum(brightness, 100, 0, 0, 255));
    SDL_FillRect(s, NULL, 0x000000 + alpha);
    SDL_BlitSurface(s, NULL, screen, &pos);

    SDL_FreeSurface(s);
    s = nullptr;
}

void renderImage(double x, double y, string name)
{
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
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
    SDL_FillRect(pScaleSurface, &size, SDL_MapRGBA(pScaleSurface->format, 0, 0, 0, 0));
    SDL_BlitScaled(images[name], NULL, pScaleSurface, NULL);
    SDL_BlitSurface(pScaleSurface, NULL, screen, &pos);

    SDL_FreeSurface(pScaleSurface);
    pScaleSurface = nullptr;
}

void RenderObject(double x, double y, int width, int height, string name, int offsetX, int offsetY)
{
    SDL_Rect pos;
    pos.x = x - offsetX;
    pos.y = y - offsetY;
    SDL_Rect size;
    size.w = width;
    size.h = height;
    SDL_Surface *pScaleSurface = SDL_CreateRGBSurface(
        images[name]->flags,
        size.w,
        size.h,
        images[name]->format->BitsPerPixel,
        images[name]->format->Rmask,
        images[name]->format->Gmask,
        images[name]->format->Bmask,
        images[name]->format->Amask);
    SDL_FillRect(pScaleSurface, &size, SDL_MapRGBA(pScaleSurface->format, 0, 0, 0, 0));
    SDL_BlitScaled(images[name], NULL, pScaleSurface, NULL);
    SDL_BlitSurface(pScaleSurface, NULL, screen, &pos);

    SDL_FreeSurface(pScaleSurface);
    pScaleSurface = nullptr;
}

void loadSurface(string path)
{
    SDL_Surface *optimizedSurface = nullptr;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        throw false;
    }
    else
    {
        optimizedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            throw false;
        }
        SDL_FreeSurface(loadedSurface);
    }
    string name = path.substr(7, path.length() - 11);
    images.insert(pair<string, SDL_Surface *>(name, optimizedSurface));
    optimizedSurface = nullptr;
}

bool loadAllImages()
{
    try
    {
        loadSurface("images/dirt-0000.png");
        loadSurface("images/dirt-000x.png");
        loadSurface("images/dirt-00x0.png");
        loadSurface("images/dirt-00xx.png");
        loadSurface("images/dirt-0x00.png");
        loadSurface("images/dirt-0x0x.png");
        loadSurface("images/dirt-0xx0.png");
        loadSurface("images/dirt-0xxx.png");
        loadSurface("images/dirt-x000.png");
        loadSurface("images/dirt-x00x.png");
        loadSurface("images/dirt-x0x0.png");
        loadSurface("images/dirt-x0xx.png");
        loadSurface("images/dirt-xx00.png");
        loadSurface("images/dirt-xx0x.png");
        loadSurface("images/dirt-xxx0.png");
        loadSurface("images/dirt-xxxx1.png");
        loadSurface("images/dirt-xxxx2.png");
        loadSurface("images/dirt-xxxx3.png");
        loadSurface("images/dirt-wall-0x0x.png");
        loadSurface("images/dirt-wall-0xx0.png");
        loadSurface("images/dirt-wall-0xxx.png");
        loadSurface("images/dirt-wall-x00x.png");
        loadSurface("images/dirt-wall-x0x0.png");
        loadSurface("images/dirt-wall-x0xx.png");
        loadSurface("images/dirt-wall-xx0x1.png");
        loadSurface("images/dirt-wall-xx0x2.png");
        loadSurface("images/dirt-wall-xxx0.png");
        loadSurface("images/dirt-wall-xxxx1.png");
        loadSurface("images/dirt-wall-xxxx2.png");
        loadSurface("images/dirt-wall-xxxx3.png");
        loadSurface("images/flora1.png");
        loadSurface("images/flora2.png");
        loadSurface("images/flora3.png");
        loadSurface("images/flora4.png");
        loadSurface("images/flora5.png");
        loadSurface("images/flora6.png");
        loadSurface("images/flora7.png");
        loadSurface("images/flora8.png");
        loadSurface("images/flora9.png");
        loadSurface("images/flora10.png");
        loadSurface("images/flora11.png");
        loadSurface("images/grass-0000.png");
        loadSurface("images/grass-000x.png");
        loadSurface("images/grass-00x0.png");
        loadSurface("images/grass-00xx.png");
        loadSurface("images/grass-0x00.png");
        loadSurface("images/grass-0x0x.png");
        loadSurface("images/grass-0xx0.png");
        loadSurface("images/grass-0xxx.png");
        loadSurface("images/grass-x000.png");
        loadSurface("images/grass-x00x.png");
        loadSurface("images/grass-x0x0.png");
        loadSurface("images/grass-x0xx.png");
        loadSurface("images/grass-xx00.png");
        loadSurface("images/grass-xx0x1.png");
        loadSurface("images/grass-xx0x2.png");
        loadSurface("images/grass-xx0x3.png");
        loadSurface("images/grass-xxx0.png");
        loadSurface("images/grass-xxxx1.png");
        loadSurface("images/grass-xxxx2.png");
        loadSurface("images/grass-xxxx3.png");
        loadSurface("images/grass-corner-0x0x.png");
        loadSurface("images/grass-corner-0xx0.png");
        loadSurface("images/grass-corner-x00x.png");
        loadSurface("images/grass-corner-x0x0.png");
        loadSurface("images/sky.png");
        loadSurface("images/stone-0000.png");
        loadSurface("images/stone-000x.png");
        loadSurface("images/stone-00x0.png");
        loadSurface("images/stone-00xx.png");
        loadSurface("images/stone-0x00.png");
        loadSurface("images/stone-0x0x.png");
        loadSurface("images/stone-0xx0.png");
        loadSurface("images/stone-0xxx.png");
        loadSurface("images/stone-x000.png");
        loadSurface("images/stone-x00x.png");
        loadSurface("images/stone-x0x0.png");
        loadSurface("images/stone-x0xx.png");
        loadSurface("images/stone-xx00.png");
        loadSurface("images/stone-xx0x1.png");
        loadSurface("images/stone-xx0x2.png");
        loadSurface("images/stone-xx0x3.png");
        loadSurface("images/stone-xxx0.png");
        loadSurface("images/stone-xxxx1.png");
        loadSurface("images/stone-xxxx2.png");
        loadSurface("images/stone-xxxx3.png");
        loadSurface("images/stone-wall-0x0x.png");
        loadSurface("images/stone-wall-0xx0.png");
        loadSurface("images/stone-wall-0xxx.png");
        loadSurface("images/stone-wall-x00x.png");
        loadSurface("images/stone-wall-x0x0.png");
        loadSurface("images/stone-wall-x0xx.png");
        loadSurface("images/stone-wall-xx0x.png");
        loadSurface("images/stone-wall-xxx0.png");
        loadSurface("images/stone-wall-xxxx1.png");
        loadSurface("images/stone-wall-xxxx2.png");
        loadSurface("images/stone-wall-xxxx3.png");
        loadSurface("images/torch.png");
        loadSurface("images/tree-3.png");
        loadSurface("images/tree-4.png");
        loadSurface("images/tree-5.png");
        loadSurface("images/tree-b-both.png");
        loadSurface("images/tree-b-left.png");
        loadSurface("images/tree-b-right.png");
        loadSurface("images/tree-branch-left.png");
        loadSurface("images/tree-branch-right.png");
        loadSurface("images/tree-r-both.png");
        loadSurface("images/tree-r-right.png");
        loadSurface("images/tree-r-left.png");
        loadSurface("images/tree-root-left.png");
        loadSurface("images/tree-root-right.png");
        loadSurface("images/tree-top-1.png");
        // loadSurface("images/tree-top-2.png");
        loadSurface("images/tree-top.png");
        loadSurface("images/tree-left-1.png");
        loadSurface("images/tree-left-2.png");
        loadSurface("images/tree-left-3.png");
        loadSurface("images/tree-right-1.png");
        loadSurface("images/tree-right-2.png");
        loadSurface("images/tree-right-3.png");
        loadSurface("images/wood-0000.png");
        loadSurface("images/wood-000x.png");
        loadSurface("images/wood-00x0.png");
        loadSurface("images/wood-00xx.png");
        loadSurface("images/wood-0x00.png");
        loadSurface("images/wood-0x0x.png");
        loadSurface("images/wood-0xx0.png");
        loadSurface("images/wood-0xxx.png");
        loadSurface("images/wood-x000.png");
        loadSurface("images/wood-x00x.png");
        loadSurface("images/wood-x0x0.png");
        loadSurface("images/wood-x0xx.png");
        loadSurface("images/wood-xx00.png");
        loadSurface("images/wood-xx0x1.png");
        loadSurface("images/wood-xx0x2.png");
        loadSurface("images/wood-xx0x3.png");
        loadSurface("images/wood-xxx0.png");
        loadSurface("images/wood-xxxx1.png");
        loadSurface("images/wood-xxxx2.png");
        loadSurface("images/wood-xxxx3.png");
        loadSurface("images/wood-wall-0xx0.png");
        loadSurface("images/wood-wall-0xxx.png");
        loadSurface("images/wood-wall-x00x.png");
        loadSurface("images/wood-wall-x0x0.png");
        loadSurface("images/wood-wall-x0xx.png");
        loadSurface("images/wood-wall-xx0x.png");
        loadSurface("images/wood-wall-xxx0.png");
        loadSurface("images/wood-wall-xxxx1.png");
        loadSurface("images/wood-wall-xxxx2.png");
        loadSurface("images/wood-wall-xxxx3.png");
        loadSurface("images/wood-plat-00.png");
        loadSurface("images/wood-plat-0x.png");
        loadSurface("images/wood-plat-x0.png");
        loadSurface("images/wood-plat-xx.png");
        loadSurface("images/Player-Falling-L.png");
        loadSurface("images/Player-Idle-L.png");
        loadSurface("images/Player-Use-1-L.png");
        loadSurface("images/Player-Use-2-L.png");
        loadSurface("images/Player-Use-3-L.png");
        loadSurface("images/Player-Use-4-L.png");
        loadSurface("images/Player-Walk-1-L.png");
        loadSurface("images/Player-Walk-2-L.png");
        loadSurface("images/Player-Walk-3-L.png");
        loadSurface("images/Player-Walk-4-L.png");
        loadSurface("images/Player-Walk-5-L.png");
        loadSurface("images/Player-Walk-6-L.png");
        loadSurface("images/Player-Walk-7-L.png");
        loadSurface("images/Player-Walk-8-L.png");
        loadSurface("images/Player-Walk-9-L.png");
        loadSurface("images/Player-Walk-10-L.png");
        loadSurface("images/Player-Walk-11-L.png");
        loadSurface("images/Player-Walk-12-L.png");
        loadSurface("images/Player-Walk-13-L.png");
        loadSurface("images/Player-Falling-R.png");
        loadSurface("images/Player-Idle-R.png");
        loadSurface("images/Player-Use-1-R.png");
        loadSurface("images/Player-Use-2-R.png");
        loadSurface("images/Player-Use-3-R.png");
        loadSurface("images/Player-Use-4-R.png");
        loadSurface("images/Player-Walk-1-R.png");
        loadSurface("images/Player-Walk-2-R.png");
        loadSurface("images/Player-Walk-3-R.png");
        loadSurface("images/Player-Walk-4-R.png");
        loadSurface("images/Player-Walk-5-R.png");
        loadSurface("images/Player-Walk-6-R.png");
        loadSurface("images/Player-Walk-7-R.png");
        loadSurface("images/Player-Walk-8-R.png");
        loadSurface("images/Player-Walk-9-R.png");
        loadSurface("images/Player-Walk-10-R.png");
        loadSurface("images/Player-Walk-11-R.png");
        loadSurface("images/Player-Walk-12-R.png");
        loadSurface("images/Player-Walk-13-R.png");
    }
    catch (bool e)
    {
        return false;
    }
    return true;
}