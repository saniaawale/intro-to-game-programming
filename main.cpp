/**
* Author: Sania Kishor Awale
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include <math.h>

constexpr int SCREEN_WIDTH = 1200,
SCREEN_HEIGHT = 800,
FPS = 60;

// sizes 
constexpr Vector2 BIG_FISH_SIZE = { 80.0f, 80.0f };
constexpr Vector2 STARFISH_SIZE = { 80.0f, 80.0f };
constexpr Vector2 JELLYFISH_SIZE = { 80.0f, 100.0f };

// movement parameters
constexpr float BIG_FISH_SPEED = 80.0f;
constexpr float STARFISH_SPEED = 30.0f;
constexpr float JELLYFISH_SPEED = 90.0f;

constexpr float BIG_FISH_WAVE_AMPLITUDE = 50.0f;
constexpr float BIG_FISH_WAVE_FREQUENCY = 1.0f;

constexpr float STARFISH_ORBIT_RADIUS = 80.0f;
constexpr float STARFISH_ORBIT_SPEED = 3.0f;
constexpr float STARFISH_ROTATION_SPEED = 45.0f;  

constexpr float JELLYFISH_FLOAT_AMPLITUDE = 30.0f;
constexpr float JELLYFISH_FLOAT_FREQUENCY = 1.5f;
constexpr float JELLYFISH_PULSE_SPEED = 3.0f;
constexpr float JELLYFISH_PULSE_AMPLITUDE = 0.2f;

// background
constexpr float WATER_COLOR_SPEED = 0.3f;
Color gBackgroundColor;

constexpr char BIG_FISH_TEXTURE[] = "assets/big_fish.png";
constexpr char STARFISH_TEXTURE[] = "assets/starfish.png";
constexpr char JELLYFISH_TEXTURE[] = "assets/jellyfish.png";

AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTotalTime = 0.0f;

// fish swimming horizontal with wave pattern + flips when reaching end of screen
Vector2 gBigFishPosition = { 0, SCREEN_HEIGHT * 0.3f };
Vector2 gBigFishScale = BIG_FISH_SIZE;
float gBigFishRotation = 0.0f;
float gBigFishWaveOffset = 0.0f;
bool gBigFishMovingRight = true;
bool gBigFishFacingRight = true;

// starfish orbiting around the big fish + continuous rotation
Vector2 gStarfishPosition;
Vector2 gStarfishScale = STARFISH_SIZE;
float gStarfishOrbitAngle = 0.0f;
float gStarfishRotation = 0.0f;

// jellyfish floating up and down + pulsing
Vector2 gJellyfishPosition = { 0, SCREEN_HEIGHT * 0.7f };
Vector2 gJellyfishScale = JELLYFISH_SIZE;
float gJellyfishFloatOffset = 0.0f;
float gJellyfishPulseOffset = 0.0f;
bool gJellyfishMovingRight = true;



// all textures
Texture2D gBigFishTexture;
Texture2D gStarfishTexture;
Texture2D gJellyfishTexture;


void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale, float rotation, bool flipHorizontal = false);
Color getWaterBackgroundColor(float time);


void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple 2D Scene - Underwater World");

    gBigFishTexture = LoadTexture(BIG_FISH_TEXTURE);
    gStarfishTexture = LoadTexture(STARFISH_TEXTURE);
    gJellyfishTexture = LoadTexture(JELLYFISH_TEXTURE);

    gBackgroundColor = getWaterBackgroundColor(0.0f);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // Delta time calculation
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTotalTime += deltaTime;


    // flipping the fish when reaches end of screen 
    if (gBigFishMovingRight) {
        gBigFishPosition.x += BIG_FISH_SPEED * deltaTime;
        gBigFishFacingRight = true;

        if (gBigFishPosition.x > SCREEN_WIDTH - BIG_FISH_SIZE.x) {
            gBigFishMovingRight = false;
        }
    }
    else {
        gBigFishPosition.x -= BIG_FISH_SPEED * deltaTime;
        gBigFishFacingRight = false;

        if (gBigFishPosition.x < BIG_FISH_SIZE.x / 2) {
            gBigFishMovingRight = true;
            gBigFishFacingRight = true;
        }

    }

    gBigFishWaveOffset += deltaTime * BIG_FISH_WAVE_FREQUENCY;
    gBigFishPosition.y = SCREEN_HEIGHT * 0.3f + BIG_FISH_WAVE_AMPLITUDE * sin(gBigFishWaveOffset);

   
    // RELATIVE POSITION FOR STARFISH -- ORBITS NEMO AND ROTATES CONTINUOUSLY 
    gStarfishOrbitAngle += STARFISH_ORBIT_SPEED * deltaTime;
    gStarfishPosition.x = gBigFishPosition.x + STARFISH_ORBIT_RADIUS * cos(gStarfishOrbitAngle);
    gStarfishPosition.y = gBigFishPosition.y + STARFISH_ORBIT_RADIUS * sin(gStarfishOrbitAngle);

    // Continuous rotation for the starfish
    gStarfishRotation += STARFISH_ROTATION_SPEED * deltaTime;

    // jellyfish - vertical floating
    gJellyfishFloatOffset += deltaTime * JELLYFISH_FLOAT_FREQUENCY;
    gJellyfishPosition.y = SCREEN_HEIGHT * 0.7f + JELLYFISH_FLOAT_AMPLITUDE * sin(gJellyfishFloatOffset);

   
    // jelly fish horizontal movement 
    if (gJellyfishMovingRight) {
        gJellyfishPosition.x += JELLYFISH_SPEED * deltaTime;
        if (gJellyfishPosition.x > SCREEN_WIDTH - JELLYFISH_SIZE.x / 2) {
            gJellyfishMovingRight = false;
        }
    }
    else {
        gJellyfishPosition.x -= JELLYFISH_SPEED * deltaTime;
        if (gJellyfishPosition.x < JELLYFISH_SIZE.x/2) {
            gJellyfishMovingRight = true; 
        }
    }

    // pulsing animation for jellyfish 
    gJellyfishPulseOffset += deltaTime * JELLYFISH_PULSE_SPEED;
    float pulseFactor = 1.0f + JELLYFISH_PULSE_AMPLITUDE * sin(gJellyfishPulseOffset);
    gJellyfishScale = { JELLYFISH_SIZE.x * pulseFactor, JELLYFISH_SIZE.y * pulseFactor };

    //update background color 
    gBackgroundColor = getWaterBackgroundColor(gTotalTime);
}

void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale, float rotation, bool flipHorizontal)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width) * (flipHorizontal ? -1.0f : 1.0f),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        scale->x,
        scale->y
    };

    Vector2 originOffset = {
        scale->x / 2.0f,
        scale->y / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea,
        destinationArea,
        originOffset,
        rotation,
        WHITE
    );
}

//changing water color function
Color getWaterBackgroundColor(float time)
{
    float colorShift = sin(time * WATER_COLOR_SPEED) * 0.5f + 0.5f;
        
    float r = 0.1f * colorShift;        
    float g = 0.3f * colorShift;        
    float b = 0.4f + colorShift * 0.5f;

    return (Color) {
        (unsigned char)(r * 255),
            (unsigned char)(g * 255),
            (unsigned char)(b * 255),
            255
    };
}

void render()
{
    BeginDrawing();
    
    ClearBackground(gBackgroundColor);

    renderObject(&gJellyfishTexture, &gJellyfishPosition, &gJellyfishScale, 0.0f);
    renderObject(&gBigFishTexture, &gBigFishPosition, &gBigFishScale, 0.0f, !gBigFishFacingRight);
    renderObject(&gStarfishTexture, &gStarfishPosition, &gStarfishScale, gStarfishRotation);

    EndDrawing();
}


void shutdown() { CloseWindow(); }


int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}