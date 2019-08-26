#include "common.h"
#include "util/TimeElapsed.h"
#include "OpenSimplexNoise.h"

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
SDL_Surface *gScreenSurface = nullptr;
void renderImage(double x, double y, string name);
map<string, SDL_Surface *> images;

void InitialWorldGen();
void RenderAll();
bool rendering = false;
vector<int> Ground;
vector<int> Background;
string groundNames[] = {"blank", "grass", "dirt", "stone", "flora", "wood", "torch", "tree", "tree-b-left", "tree-b-right", "tree-b-both", "tree-branch-right", "tree-branch-left", "tree-r-left", "tree-r-right", "tree-r-both", "tree-root-right", "tree-root-left"};
string backgroundNames[] = {"dirt-wall", "stone-wall", "wood-wall", "sky"};
vector<double> Light;
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
void Generate();
void GenerateGroundAtIndex(int idx);
string CheckNeighbors(int x, int y, string type);
string CheckForEmptyTile(int idx, string type);
template <typename T>
string toStringHelper(T n)
{
    ostringstream oss;
    oss << n;
    return oss.str();
}

int scaled[2];
double pos[2];

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
        "Game Engine",
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
            gScreenSurface = SDL_GetWindowSurface(window);
            SDL_SetSurfaceBlendMode(gScreenSurface, SDL_BLENDMODE_BLEND);
        }
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);
    return true;
}

int WinMain()
{
    // startTimer();
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
    RenderAll();

    SDL_UpdateWindowSurface(window);

    Run();

    CleanUp();

    // finish();
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
    SDL_Quit();
}

void InitialWorldGen()
{
    for (int i = 0; i < amountX * amountY; i++)
    {
        Ground.push_back(2);
        Background.push_back(0);
    }
    gen = 0;
    feature_size = 100;
    Generate();
    gen = 1;
    feature_size = 200;
    Generate();
    gen = 2;
    feature_size = 500;
}

void RenderAll()
{
    rendering = true;
    startTimer();
    for (int i = 0; i < Ground.size(); i++)
    {
        RenderGroundAtIndex(i);
    }
    rendering = false;
    finish();
}

void RenderGroundAtIndex(int idx)
{
    GetScaledXYFromIndex(idx);
    GetRealXYFromScaledXY(scaled[0], scaled[1]);
    int randTile = 0;
    if (Background[idx] != 3)
    {
        string name = "";
        string shape = CheckNeighbors(scaled[0], scaled[1], "background");
        // name = backgroundNames[Background[idx]] + "-xxxx1";
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
        if (Ground[idx] == 0)
        {
            if (shape == "xx0x")
            {
                randTile = rand() % 2 + 1;
                name += toStringHelper(randTile);
            }
        }
        //Brightness
        renderImage(pos[0], pos[1], name);
    }
    else
    {
        renderImage(pos[0], pos[1], "sky");
    }
    if (Ground[idx] != 0)
    {
        string name = "";
        string shape = CheckNeighbors(scaled[0], scaled[1], "ground");
        // cout << Ground[idx] << "-" << shape << endl;
        // name = groundNames[Ground[idx]] + "-xxxx1";
        name = groundNames[Ground[idx]] + "-" + shape;
        if (Ground[idx] == 2 || Ground[idx] == 3)
        {
            if (shape == "xxxx")
            {
                randTile = rand() % 3 + 1;
                name += toStringHelper(randTile);
            }
        }
        if (Ground[idx] == 1 || Ground[idx] == 3 || Ground[idx] == 6)
        {
            if (shape == "xx0x")
            {
                randTile = rand() % 3 + 1;
                name += toStringHelper(randTile);
            }
        }
        if (Ground[idx] == 5)
        {
            name = "torch";
        }
        renderImage(pos[0], pos[1], name);
        if (shape != "xxxx" && Ground[idx] != 5)
        {
            platformX.push_back(pos[0]);
            platformY.push_back(pos[1]);
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
        if (Ground[idx] == 0 || Ground[idx] == 7)
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
    int mouseX = round(ScaleNum(x, 0, width, 0, amountX));
    int mouseY = round(ScaleNum(y, 0, height, 0, amountY));
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

void Run()
{
    bool gameLoop = true;
    bool fullScreen = false;
    while (gameLoop)
    {
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
                default:
                    break;
                }
            }
        }
    }
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
    SDL_BlitSurface(pScaleSurface, NULL, gScreenSurface, &pos);

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
        loadSurface("images/grass-xxxx.png");
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
        loadSurface("images/tree-top-2.png");
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
    }
    catch (bool e)
    {
        return false;
    }
    return true;
}