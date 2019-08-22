#include "common.h"
#include "util/TimeElapsed.h"
#include "Tile.h"

bool isRunning = true;
int width = 1008;
int height = 608;
int amountX = width / 16;
int amountY = height / 16;

bool init();
void CleanUp();
void Run();
bool loadAllImages();
void loadSurface(string path);
SDL_Surface *gScreenSurface = nullptr;
vector<Tile> tiles;
map<string, SDL_Surface *> images;

void InitialWorldGen();
void RenderAll();
bool rendering = false;
vector<string> Ground;
vector<string> Background;
vector<float> Light;
void RenderGroundAtIndex(int idx);
int *GetScaledXYFromIndex(int idx);
int GetIndexFromScaledXY(int x, int y);
int GetIndexFromXY(int x, int y);
int *GetRealXYFromScaledXY(int x, int y);
float ScaleNum(float n, float minN, float maxN, float min, float max);

SDL_Window *window = nullptr;
SDL_GLContext glContext;
unsigned int WindowFlags;

bool init()
{
    WindowFlags = SDL_WINDOW_OPENGL;
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

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
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
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface(window);
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
        // return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
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

    InitialWorldGen();
    RenderAll();
    // RenderGroundAtIndex(GetIndexFromScaledXY(amountX, 0));
    // RenderGroundAtIndex(1);
    // SDL_Rect pos;
    // pos.x = width - 16;
    // pos.y = 0;
    // SDL_BlitSurface(images["dirt-0000"], NULL, gScreenSurface, &pos);
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
        image.second = nullptr;
    }
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void InitialWorldGen()
{
    for (int i = 0; i < amountX * amountY; i++)
    {
        tiles.push_back(Tile{});
        Ground.push_back("dirt-xxxx1");
        Background.push_back("");
    }
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
    int *scaled = GetScaledXYFromIndex(idx);
    int *pos = GetRealXYFromScaledXY(*scaled, *(scaled + 1));
    tiles[idx].setName(Ground[idx]);
    tiles[idx].render(*pos, *(pos + 1), window, gScreenSurface, images);
}

int *GetScaledXYFromIndex(int idx)
{
    static int returns1[2];
    returns1[0] = idx % amountX;
    returns1[1] = idx / amountX;
    return returns1;
}

int GetIndexFromScaledXY(int x, int y)
{
    return x + y * amountX;
}

int GetIndexFromXY(int x, int y)
{
    int mouseX = round(ScaleNum(x, 0, width, 1, amountX));
    int mouseY = round(ScaleNum(y, 0, height, 0, amountY));
    return GetIndexFromScaledXY(mouseX, mouseY);
}

int *GetRealXYFromScaledXY(int x, int y)
{
    static int returns2[2];
    returns2[0] = round(ScaleNum(x, 0, amountX, 0, width));
    returns2[1] = round(ScaleNum(y, 0, amountY, 0, height));
    return returns2;
}

float ScaleNum(float n, float minN, float maxN, float min, float max)
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

void loadSurface(string path)
{
    //The final optimized image
    SDL_Surface *optimizedSurface = nullptr;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        throw false;
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
            throw false;
        }

        //Get rid of old loaded surface
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