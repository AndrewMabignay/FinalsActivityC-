#include <cstdio>
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cstdlib>
#include <ctime>

// SETTINGS (WINDOWS)
int windowWidth = 550, windowHeight = 700;

// USER MOVEMENT - CHANGES OF VALUE (X, Y)
float userX = 7.5f, userY = 0.0f;

// USER SIZE
const float userSizeLeft = 2.0f, userSizeRight = 3.0f, userSizeBottom = 2.0f, userSizeTop = 3.0f;

// ENEMY SPREAD
const int enemyCount = 3;
float enemyX[enemyCount], enemyY[enemyCount];

// KEYBOARD (Start Jumping)
bool moveLeft = false, moveRight = false;

// USER JUMP 
bool isOnGround = true;
float gravity = 1.0f, velocityY = 0.0f, jumpStrength = 1.15f;

// GROUND

// PLATFORMS
const int platformCount = 3;
float platformX[platformCount] = { 6.0f, 11.0f, 16.0f };
float platformY[platformCount] = { 6.0f, 9.0f, 13.0f };
float platformHalfWidth[platformCount] = { 2.0f, 2.0f, 2.0f };

// AUTO MOVE PLATFORM
float platformSpeed[platformCount] = { 0.05f, 0.07f, 0.06f };
int platformDirection[platformCount] = { 1, -1, 1 };

// CLOSE PLATFORM

// LIVES, SCORING, STAGE & GAME OVER PROGRAM
int lives = 3; // STARTING LIVES
bool isGameOver = false;
int lastHitTime = 0;
const int hitDelay = 1000; // delay in ms (1 second)
int score = 0;
bool landedOnPlatform[platformCount] = { false, false, false };
bool switchOn = false;
int stage = 1;

// START, PAUSE, PLAY, & END (PLATFORM PROGRAM)
bool gameIntro = true; // GAME INTRODUCTION
bool isStart = false; // START GAME
bool isRestart = false; // RESTART GAME
bool isPaused = false; // PAUSE PROGRAM
bool isPlayAgain = false; // HOME WHEN THE PROGRAM PAUSE OR GAME OVER
bool isWin = false;

// ACTIVITY # 1 | VARIABLES
bool isWallUpRightOpen = false;
const float keyLeft = 2.5f, keyRight = 3.5f, keyBottom = 18.0f, keyTop = 19.0f; // KEY OBJECT
const float wallLeftLeft = 0.0f, wallLeftRight = 9.5f, wallLeftBottom = 19.5f, wallLeftTop = 20.0f; // WALL LEFT
const float wallRightLeft = 10.5f, wallRightRight = 20.0f, wallRightBottom = 19.5f, wallRightTop = 20.0f; // WALL RIGHT
const float wallMiddleLeft = 9.5f, wallMiddleRight = 10.5f, wallMiddleBottom = 13.5f, wallMiddleTop = 20.0f; // WALL MIDDLE


GLuint enemyTextures[3];  // 3 frames for enemy animation
int enemyFrame = 0;       // current frame
int enemyFrameCounter = 0;
int enemyFrameDelay = 10; // higher = slower animation

int changeGravity = 1;

int changeMovementRightLeft = 0;

int countdownTime = 120;   // starting seconds
int startTime = 0;        // when countdown started
int remainingTime = 120;   // current countdown
bool countdownActive = false;

int countdownTimeForPortalKeyShesh = 15;
int startTimeForPortalKeyShesh = 0;
int remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
bool countdownActiveForPortal = false;

// ============================ FINALS ACTIVITY (START) | VARIABLE ============================ 
// --------- GAME STATE --------- 
bool typingName = false;
bool showLeaderboard = false;
std::string playerName = "";
int playerScore = 0;
//std::vector<std::tuple<std::string, int, int>> topScores;

std::vector<std::pair<std::string, int>> topScores;
int highestRemainingCountdownTime = 0;

// --------- FILE PATH --------- 
const char* scoreFilePath = "Leaderboards/score.txt";
// ============================ FINALS ACTIVITY (END) | VARIABLE ============================ 

// ============================ THEORY MODIFICATION ADDITIONAL (START) | VARIABLE ============================
// ---------------- #2 STAR / HEART ----------------
//float itemOffsetX = 0.0f;
//float itemDirection = 1.0f;
//float itemSpeed = 0.03f;
//float itemRange = 1.5f;
//
//float itemLeft, itemRight, itemBottom, itemTop; // bounding box
//bool itemCollected = false;
// ============================ THEORY MODIFICATION ADDITIONAL (END) | VARIABLE ============================

// ============================ FINALS ACTIVITY (START) | SAVE & LOAD ALL SCORE FUNCTION ============================ 
// --------- SAVE SCORE --------- 
void saveScoreToFile(const char* filename) {
    // STEP 1: READ EXISTING SCORES
    std::vector<std::pair<std::string, int>> scores;
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::string name;
        int score;

        while (inFile >> name >> score)
            scores.push_back({ name, score });
        inFile.close();
    }

    // STEP 2: ADD THE NEW ONE
    scores.push_back({ playerName, playerScore });

    // STEP 3: SORT FROM HIGHEST TO LOWEST
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // STEP 4: REWRITE FILE IN SORTED ORDER
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cout << "Failed to open file for saving.\n";
        return;
    }

    for (auto& entry : scores)
        outFile << entry.first << " " << entry.second << std::endl;

    outFile.close();
    std::cout << "Saved (sorted): " << playerName << " " << playerScore << std::endl;
}

// --------- LOAD ALL SCORES ---------
std::vector<std::pair<std::string, int>> loadAllScore(const char* filename) {
    std::vector<std::pair<std::string, int>> scores;
    std::ifstream file(filename);
    if (!file.is_open()) return scores;

    std::string name;
    int score;
    while (file >> name >> score)
        scores.push_back({ name, score });
    
    file.close();

    return scores;
}

// ============================ FINALS ACTIVITY (END) | SAVE & LOAD ALL SCORE FUNCTION ============================ 

// ============================ FINALS ACTIVITY (START) | DISPLAY LEADERBOARDS & TYPING UI OBJECT FUNCTION ============================ 
// -------- SHOW LEADERBOARD FUNCTION -------- 
void showLeaderboardsObject() {
    float showLeaderboardLeft = 6.0f, showLeaderboardRight = 15.0f, showLeaderboardBottom = 7.0f, showLeaderboardTop = 14.0f; // INTRODUCTION LOGO

    // SHOW LEADERBOARD CONTAINER
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.8f); // OVERLAY
        glVertex2f(showLeaderboardLeft, showLeaderboardBottom);
        glVertex2f(showLeaderboardRight, showLeaderboardBottom);
        glVertex2f(showLeaderboardRight, showLeaderboardTop);
        glVertex2f(showLeaderboardLeft, showLeaderboardTop);
    glEnd();

    topScores = loadAllScore(scoreFilePath);

    glColor3f(1.0f, 1.0f, 0.0f);
    float yPos = 12.5f;
    glRasterPos2f(7.0f, yPos);
    std::string header = "TOP 5 SCORES:";
    for (char c : header) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    int count = 0;
    for (auto& s : topScores) {
        if (count >= 5) break;
        yPos -= 0.90f;
        glRasterPos2f(7.0f, yPos);
        std::string line = std::to_string(count + 1) + ". " + s.first + " - " + std::to_string(s.second);

        for (char c : line) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        count++;
    }
}

// -------- TYPING UI FUNCTION --------
void typingUiObject() {
    glRasterPos2f(5.0f, 12.0f);
    std::string msg;
    if (typingName) msg = "Enter Name " + playerName + "_";
    else msg = "PAUSED (Press E to Show Scores)";

    for (char c : msg)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// ============================ FINALS ACTIVITY (END) | DISPLAY LEADERBOARDS & TYPING UI OBJECT FUNCTION ============================ 
GLuint loadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setup filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Pumili kung RGB or RGBA
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return textureID;
}

void renderBitmapString(float x, float y, const char* string)
{
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}

void renderBitmapStringPause(float x, float y, const char* string)
{
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

// ============================ OBJECT FUNCTION ============================

// USER | USER SPRITES
GLuint userWalkLeft[3], userWalkRight[3], userIdle;
int currentFrame = 0;
int frameDelay = 10;
int frameCounter = 0;

// USER | USER FUNCTION
void user()
{
    glEnable(GL_TEXTURE_2D);

    // GAME OVER PLATFORM PROGRAM
    if (!isGameOver) {

        // IMAGE FRAME PER FRAME
        if (moveRight) {   // ANIMATE RIGHT
            frameCounter++;
            if (frameCounter >= frameDelay) {
                currentFrame = (currentFrame + 1) % 3;
                frameCounter = 0;
            }
            glBindTexture(GL_TEXTURE_2D, userWalkRight[currentFrame]);
        }
        else if (moveLeft) {   // ANIMATE LEFT
            frameCounter++;
            if (frameCounter >= frameDelay) {
                currentFrame = (currentFrame + 1) % 3;
                frameCounter = 0;
            }
            glBindTexture(GL_TEXTURE_2D, userWalkLeft[currentFrame]);
        }
        else {
            // idle frame (default facing right)
            glBindTexture(GL_TEXTURE_2D, userIdle);
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(userX + userSizeLeft, userY + userSizeBottom); // BOTTOM LEFT
    glTexCoord2f(1.0f, 0.0f); glVertex2f(userX + userSizeRight, userY + userSizeBottom); // BOTTOM RIGHT
    glTexCoord2f(1.0f, 1.0f); glVertex2f(userX + userSizeRight, userY + userSizeTop); // TOP RIGHT
    glTexCoord2f(0.0f, 1.0f); glVertex2f(userX + userSizeLeft, userY + userSizeTop); // TOP LEFT
    glEnd();

    glDisable(GL_TEXTURE_2D); // RENDER OTHER OBJECTS NORMALLY
}

void ground()
{
    glBegin(GL_QUADS);
    glColor3f(0.396f, 0.263f, 0.129f); // DARK BROWN
    glVertex2f(0.0f, 0.0f); // BOTTOM LEFT
    glVertex2f(20.0f, 0.0f); // BOTTOM RIGHT
    glVertex2f(20.0f, 2.0f); // TOP RIGHT
    glVertex2f(0.0f, 2.0f); // TOP LEFT
    glEnd();
}
GLuint woodTexture;

void loadTexture() {
    int width, height, channels;
    unsigned char* data = stbi_load("Images/Glut_Image/wood floating platform.png", &width, &height, &channels, 4);

    if (data) {
        glGenTextures(1, &woodTexture);
        glBindTexture(GL_TEXTURE_2D, woodTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else {
        printf("Failed to load texture!\n");
    }
}

void middleGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);

    for (int i = 0; i < platformCount; i++) {
        glBegin(GL_QUADS);
        // Texture coords (0,0) to (1,1)
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(platformX[i] - platformHalfWidth[i], platformY[i]);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(platformX[i] + platformHalfWidth[i], platformY[i]);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(platformX[i] + platformHalfWidth[i], platformY[i] + 0.5f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(platformX[i] - platformHalfWidth[i], platformY[i] + 0.5f);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

// CLOSE PLATFORM
void closePlatform()
{
    glBegin(GL_QUADS);

    // Top (lighter blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 19.0f); // BOTTOM LEFT
    glVertex2f(20.0f, 19.0f); // BOTTOM RIGHT

    // Bottom (darker blue)
    glColor3f(0.0f, 0.0f, 0.5f);
    glVertex2f(20.0f, 20.0f); // TOP RIGHT
    glVertex2f(0.0f, 20.0f); // TOP LEFT
    glEnd();
}

void enemy()
{
    glEnable(GL_TEXTURE_2D);

    // Animate enemy (switch frames every few calls)
    enemyFrameCounter++;
    if (enemyFrameCounter >= enemyFrameDelay) {
        enemyFrame = (enemyFrame + 1) % 3; // cycle 0 → 1 → 2 → back to 0
        enemyFrameCounter = 0;
    }

    for (int i = 0; i < enemyCount; i++)
    {
        glBindTexture(GL_TEXTURE_2D, enemyTextures[enemyFrame]);

        float size = 1.0f;
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(enemyX[i], enemyY[i]);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(enemyX[i] + size, enemyY[i]);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(enemyX[i] + size, enemyY[i] + size);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(enemyX[i], enemyY[i] + size);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

// ---------------------------- ACTIVITY: ADDITIONAL PLATFORM ----------------------------
// ACTIVITY: WALL MIDDLE UP
// Texture ID for wallMiddleUp
GLuint wallMiddleUpTex;

void loadWallMiddleUpTexture() {
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Images/Glut_Image/wall.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &wallMiddleUpTex);
        glBindTexture(GL_TEXTURE_2D, wallMiddleUpTex);

        // Texture settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else {
        printf("Failed to load wallMiddleUp texture!\n");
    }
}

void wallMiddleUp()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallMiddleUpTex);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(wallMiddleLeft, wallMiddleBottom); // BOTTOM LEFT
    glTexCoord2f(1.0f, 0.0f); glVertex2f(wallMiddleRight, wallMiddleBottom); // BOTTOM RIGHT
    glTexCoord2f(1.0f, 1.0f); glVertex2f(wallMiddleRight, wallMiddleTop);    // TOP RIGHT
    glTexCoord2f(0.0f, 1.0f); glVertex2f(wallMiddleLeft, wallMiddleTop);     // TOP LEFT
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ACTIVITY: WALL LEFT UP
GLuint wallLeftUpTex;

void loadWallLeftUpTexture() {
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Images/Glut_Image/wallHorizontal.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &wallLeftUpTex);
        glBindTexture(GL_TEXTURE_2D, wallLeftUpTex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
}

void wallLeftUp()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallLeftUpTex);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(wallLeftLeft, wallLeftBottom);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(wallLeftRight, wallLeftBottom);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(wallLeftRight, wallLeftTop);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(wallLeftLeft, wallLeftTop);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ACTIVITY: WALL RIGHT UP
GLuint wallRightUpTex;

void loadWallRightUpTexture() {
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Images/Glut_Image/gate.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &wallRightUpTex);
        glBindTexture(GL_TEXTURE_2D, wallRightUpTex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else {
        printf("Failed to load wallRightUp texture!\n");
    }
}

void wallRightUp()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallRightUpTex);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(10.5f, 19.5f); // BOTTOM LEFT
    glTexCoord2f(1.0f, 0.0f); glVertex2f(20.0f, 19.5f); // BOTTOM RIGHT
    glTexCoord2f(1.0f, 1.0f); glVertex2f(20.0f, 20.0f);  // TOP RIGHT
    glTexCoord2f(0.0f, 1.0f); glVertex2f(10.5f, 20.0f);  // TOP LEFT
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


// ACTIVITY: KEY

GLuint keyTexture;
void key()
{
    // bounds used elsewhere: bottom-left (3.5,18.0) top-right (4.5,19.0)
    float left = 2.5f;
    float bottom = 18.0f;
    float right = 3.5f;
    float top = 19.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, keyTexture);

    // ensure texture shows with proper alpha
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(left, bottom);  // bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex2f(right, bottom); // bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex2f(right, top);    // top-right
    glTexCoord2f(0.0f, 1.0f); glVertex2f(left, top);     // top-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}



// ---------------------------- OVERLAY ----------------------------
void blackOverlay()
{
    // BLACK
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // OVERLAY
    glVertex2f(0.0f, 0.0f); // BOTTOM LEFT
    glVertex2f(20.0f, 0.0f); // BOTTOM RIGHT
    glVertex2f(20.0f, 20.0f); // TOP RIGHT
    glVertex2f(0.0f, 20.0f); // TOP LEFT
    glEnd();
}

extern int windowWidth, windowHeight;
const int orthoWidth = 20;
const int orthoHeight = 15;

void drawButton(float left, float bottom, float width, float height, const char* label)
{
    float right = left + width;
    float top = bottom + height;

    // BUTTON BACKGROUND
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(left, bottom);
    glVertex2f(right, bottom);
    glVertex2f(right, top);
    glVertex2f(left, top);
    glEnd();

    // --- TEXT WIDTH IN PIXELS ---
    void* font = GLUT_BITMAP_HELVETICA_12;  // 12px font
    int textWidthPx = 0;
    for (const char* c = label; *c != '\0'; c++)
        textWidthPx += glutBitmapWidth(font, *c);

    // --- CONVERT PIXELS TO WORLD ---
    float pxToWorldX = (float)orthoWidth / windowWidth;
    float pxToWorldY = (float)orthoHeight / windowHeight;

    float textWidthWorld = textWidthPx * pxToWorldX;
    float textHeightWorld = 12 * pxToWorldY; // since 12px font

    // --- CENTER TEXT ---
    float textX = left + (width - textWidthWorld) / 2.0f;
    float textY = bottom + (height - textHeightWorld) / 2.0f;

    // --- DRAW LABEL ---
    glColor3f(1.0f, 1.0f, 1.0f);
    renderBitmapStringPause(textX, textY, label);
}

// --------------------------------------  PROGRAM: INTRODUCTION  --------------------------------------
float creditsX = 0.0f;

// UPDATE CREDITS POSITION
void updateCreditsPosition() {
    creditsX += 0.009f; // 0.007 Adjust speed as needed
    if (creditsX > 20.0f) creditsX = 0.0f; // Loop back if needed
}

// RENDER CREDITS
void renderCredits() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red text

    // Combine all names into one line
    std::string creditLine = "@Credits: Andrew Mabignay | Keith Aldrich Cruz | Edraine Dela Paz";

    // Fixed Y position, moving X position
    float yPos = 2.0f; // adjust as needed
    glRasterPos2f(creditsX, yPos);

    for (char c : creditLine)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
}


float introLeft = 0.0f, introRight = 20.0f, introBottom = 0.0f, introTop = 20.0f; // INTRODUCTION BACKGROUND
float introLogoLeft = 6.5f, introLogoRight = 14.5f, introLogoBottom = 8.0f, introLogoTop = 14.0f; // INTRODUCTION LOGO

// INTRODUCTION OVERLAY
void introductionOverlay()
{
    blackOverlay(); // OVERLAY

    // INTRODUCTION BACKGROUND
    glBegin(GL_QUADS);
    // glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(introLeft, introBottom);
    glVertex2f(introRight, introBottom);
    glVertex2f(introRight, introTop);
    glVertex2f(introLeft, introTop);
    glEnd();

    // INTRODUCTION LOGO 
    glBegin(GL_QUADS);
    // glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(introLogoLeft, introLogoBottom);
    glVertex2f(introLogoRight, introLogoBottom);
    glVertex2f(introLogoRight, introLogoTop);
    glVertex2f(introLogoLeft, introLogoTop);
    glEnd();

    // CLICK [L] TO LEADERBOARD
    glColor3f(1.0f, 0.0f, 0.0f); // RED
    renderBitmapString(5.5f, 5.0f, "Leaderboard [L]");

    // CLICK [TAB] TO START
    glColor3f(1.0f, 0.0f, 0.0f); // RED
    renderBitmapString(12.0f, 5.0f, "Start [Tab]");

    // CREDITS
    renderCredits();
}

// --------------------------------------  SETTINGS: PAUSE & PLAY  -------------------------------------- 
float pauseLogoLeft = 6.5f, pauseLogoRight = 14.5f, pauseLogoBottom = 8.0f, pauseLogoTop = 14.0f; // PAUSE LOGO
void overlayPausePlay()
{
    blackOverlay(); // OVERLAY

    // PAUSE LOGO 
    glBegin(GL_QUADS);
    // glColor3f(0.8f, 0.1f, 0.1f);
    glVertex2f(introLogoLeft, introLogoBottom);
    glVertex2f(introLogoRight, introLogoBottom);
    glVertex2f(introLogoRight, introLogoTop);
    glVertex2f(introLogoLeft, introLogoTop);
    glEnd();

    // BUTTON LAYOUT (centralized)
    float btnWidth = 3.5f;
    float btnHeight = 1.5f;
    float spacing = 1.0f; // space between buttons
    float totalWidth = 3 * btnWidth + 2 * spacing;
    float startX = (20.0f - totalWidth) / 2.0f; // center horizontally
    float btnY = 5.0f; // vertical position (bottom)

    // DRAW 3 BUTTONS SIDE BY SIDE
    drawButton(startX, btnY, btnWidth, btnHeight, "HOME [H]");
    drawButton(startX + (btnWidth + spacing), btnY, btnWidth, btnHeight, "RESTART [R]");
    drawButton(startX + 2 * (btnWidth + spacing), btnY, btnWidth, btnHeight, "RESUME [ESC]");
}

// --------------------------------------  PROGRAM: GAME OVER  --------------------------------------
void gameOverOverlay()
{
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow text

    if (typingName && !showLeaderboard) {
        typingUiObject();
    }
    else {
        showLeaderboardsObject();
    }

    // BLACK OVERLAY
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // OVERLAY
    glVertex2f(0.0f, 0.0f); // BOTTOM LEFT
    glVertex2f(20.0f, 0.0f); // BOTTOM RIGHT
    glVertex2f(20.0f, 20.0f); // TOP RIGHT
    glVertex2f(0.0f, 20.0f); // TOP LEFT
    glEnd();

    // BUTTON LAYOUT (centralized)
    float btnWidth = 3.5f;
    float btnHeight = 1.5f;
    float spacing = 1.0f; // space between buttons
    float totalWidth = 2 * btnWidth + 1 * spacing;
    float startX = (20.0f - totalWidth) / 2.0f; // center horizontally
    float btnY = 5.0f; // vertical position (bottom)

    // DRAW 3 BUTTONS SIDE BY SIDE
    glColor3f(1.0f, 0.0f, 0.0f); // RED
    

    if (typingName) {
        renderBitmapString(5.5f, 5.0f, "Back [Esc]");
        renderBitmapString(10.5f, 5.0f, "Submit [Enter]");
    }
    else {
        renderBitmapString(3.5f, 5.0f, "Home [H]");
        renderBitmapString(7.5f, 5.0f, "Retry [R]");
        renderBitmapString(11.5f, 5.0f, "Type Name [Enter]");
    }
    /*drawButton(startX, btnY, btnWidth, btnHeight, "HOME [H]");
    drawButton(startX + (btnWidth + spacing), btnY, btnWidth, btnHeight, "RETRY [R]");*/
}

GLuint backgroundTexture = 0;

void backgroundProgram()
{
    if (backgroundTexture == 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(20.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(20.0f, 20.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 20.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// ============================ OBJECT PROGRAM (FINAL) ============================

// ============================ ADDITIONAL (START) | OBJECT THEORY MODIFICATION ============================ 

// ---------------- #1 WIND ----------------
bool isWindActivate = false;
int lastToggleWindTime = false;
int toggleWindDelay = 5000;
int windDirection = 1; // 1 = right, -1 = left

void wind()
{
    if (isWindActivate)
    {
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 0.3f); // OVERLAY
        glVertex2f(0.0f, 0.0f);
        glVertex2f(20.0f, 0.0f);
        glVertex2f(20.0f, 20.0f);
        glVertex2f(0.0f, 20.0f);
        glEnd();
    }
}

// ---------------- #2 STAR / HEART ----------------
//void item() 
//{
//    if (itemCollected) return; // hide if collected
//
//    glBegin(GL_QUADS);
//    glColor3f(1.0f, 1.0f, 0.0f);
//    glVertex2f(itemLeft, itemBottom);
//    glVertex2f(itemRight, itemBottom);
//    glVertex2f(itemRight, itemTop);
//    glVertex2f(itemLeft, itemTop);
//    glEnd();
//}


// ============================ ADDITIONAL (END) | OBJECT THEORY MODIFICATION ============================ 

void objectProgram()
{
    backgroundProgram();

    if (!isStart) {
        if (showLeaderboard) {
            showLeaderboardsObject();
        }
        else {
            updateCreditsPosition();
            introductionOverlay();
        }
    }

    // TESTING

    // REVEAL USER, ENEMY, & GROUND (WHEN PROGRAM START)
    if (isStart && (stage == 1 || stage == 2 || stage == 3)) {
        // USER
        user();

        // ENEMY 
        enemy();

        // GROUND
        ground();

        // MIDDLE GROUND
        middleGround();

        // CLOSE PLATFORM
        //closePlatform();

        // ACTIVITY# 1 | WALL MIDDLE UP & KEY
        /// if (!isWallUpRightOpen && !countdownActiveForPortal) {
        if (!isWallUpRightOpen) {
            key();
            wallRightUp();
        }

        wallMiddleUp();
        wallLeftUp();


        /*if (countdownActiveForPortal == false) {
            key();
            wallRightUp();
        }*/

        //
        if (stage == 2) {
            wind();
        }

        if (stage == 3) {
            wind();
        }

        // --------------------- ADDITIONAL THEORY MODIFICATION (START) | OBJECT METHOD DECLARATION ---------------------
        // ---------------- #2 STAR / HEART ----------------
        // item();
        // --------------------- ADDITIONAL THEORY MODIFICATION (END) | OBJECT METHOD DECLARATION ---------------------

        if (isPaused) {
            overlayPausePlay();
        }

        if (isGameOver) gameOverOverlay();
    }
}

// --------------------- MAIN FUNCTION | ENEMY LOGIC ---------------------
void enemyLogic()
{
    for (int i = 0; i < enemyCount; i++) {
        enemyX[i] = rand() % 20;       // random x position
        enemyY[i] = (rand() % 20) + 5; // random y para di sabay-sabay
    }
}

// ============================ RESET GAME ============================
void resetGame()
{
    // RESET STATES
    isGameOver = false;
    isStart = true;
    isPaused = false;

    // RESET POSITIONS
    userX = 7.5f;
    userY = 0.0f;
    velocityY = 0.0f;
    isOnGround = true;

    // RESET STATS
    lives = 3;
    score = 0;
    stage = 1;

    // RESET PLATFORMS (BACK TO THE ORIGINAL POSITIONS)
    float defaultPlatformX[platformCount] = { 6.0f, 11.0f, 16.0f };
    float defaultPlatformY[platformCount] = { 6.0f, 9.0f, 13.0f };

    for (int i = 0; i < platformCount; i++)
    {
        platformX[i] = defaultPlatformX[i];
        platformY[i] = defaultPlatformY[i];
        landedOnPlatform[i] = false;
    }

    // RESET ENEMY
    enemyLogic();

    // RESET COUNTDOWN (MAIN TIMER)
    countdownTime = 30;                     // total seconds
    startTime = glutGet(GLUT_ELAPSED_TIME); // mark new start
    remainingTime = countdownTime;
    countdownActive = true;

    // RESET COUNTDOWN (PORTAL TIMER)
    countdownTimeForPortalKeyShesh = 15;
    startTimeForPortalKeyShesh = glutGet(GLUT_ELAPSED_TIME);
    remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
    countdownActiveForPortal = true;

    isWallUpRightOpen = false;
}

// ============================ KEYBOARD FUNCTION ============================
int jumpCount = 0;
const int maxJumps = 2;

void keyboard(unsigned char key, int x, int y)
{
    if (isStart) {

        switch (key)
        {
            // MOVE LEFT & RIGHT
        case 'a':
        case 'A':
            moveLeft = true;
            break;
        case 'd':
        case 'D':
            moveRight = true;
            break;
        case 32: // SPACE (jump)
            if (isOnGround || velocityY == 0.0f) {
                isOnGround = false;
                velocityY = jumpStrength;
            }
            //if (jumpCount < maxJumps && (isOnGround || velocityY == 0.0f)) {
            //    isOnGround = false;
            //    velocityY = jumpStrength;
            //    jumpCount++;   // increment jump count
            //}

            //if (isOnGround || velocityY == 0.0f) {
            //    if (jumpCount < maxJumps) {
            //        isOnGround = false;
            //        velocityY = jumpStrength;
            //        jumpCount++;   // increment jump count
            //    }
            //}

            break;
        case 27: // ESC (Pause & Play)
            isPaused = !isPaused ? true : false;
            break;

            // THEORY MODIFICATION | CHANGE GRAVITY
        case 'G':
        case 'g':
            changeGravity = (changeGravity % 3) + 1;
            printf("Change Gravity : %d\n", changeGravity);
            break;
        }
    }

    // --------------- RESTART IF GAMEOVER ---------------
    //if (isGameOver) {
    //    if (key == 13) {
    //        typingName = true;
    //        return;
    //    }

    //    if (typingName) {
    //        if (key == 13) { // ENTER
    //            typingName = false;
    //            showLeaderboard = true;
    //            playerScore = score;
    //            saveScoreToFile(scoreFilePath);
    //            std::cout << "Name saved to: " << scoreFilePath << std::endl;
    //            std::cout << "Final countdown saved: " << highestRemainingCountdownTime << std::endl;
    //         
    //            if (key == 'r' || key == 'R') {
    //                showLeaderboard = false;
    //                resetGame();
    //            }
    //        }
    //        else if (key == 8 && !playerName.empty()) {
    //            playerName.pop_back();
    //        }
    //        else if (isalpha(key)) {
    //            playerName += key;
    //        }
    //        return;
    //    }

    //    switch (key)
    //    {
    //    case 'R':
    //    case 'r':
    //        resetGame();
    //        break;
    //    case 'H':
    //    case 'h':
    //        resetGame();
    //        isStart = false;
    //        break;
    //    }
    //}
    if (isGameOver) {
        if (typingName) {
            if (key == 13) { // ENTER
                typingName = false;
                showLeaderboard = true;
                playerScore = score;
                saveScoreToFile(scoreFilePath);
                std::cout << "Name saved to: " << scoreFilePath << std::endl;
                std::cout << "Final countdown saved: " << highestRemainingCountdownTime << std::endl;
            }
            else if (key == 27 && typingName) { // ESC (GO BACK)
                typingName = false;
            }
            else {
                // Handle character input for name
                if (key == 8 && !playerName.empty()) { // BACKSPACE
                    playerName.pop_back();
                }
                else if (key >= 32 && key <= 126) { // Printable characters
                    playerName += key;
                }
            }
            return;
        }
        else {
            switch (key)
            {
                case 'R':
                case 'r':
                    resetGame();
                    break;
                case 'H':
                case 'h':
                    resetGame();
                    isStart = false;
                    break;
            }
        }

        if (key == 13) {
            typingName = true;
            return;
        }

        // Optional: handle retry or home keys here
    }


    if (isPaused == true) {
        switch (key)
        {
        case 'R':
        case 'r':
            resetGame();
            break;
        case 'H':
        case 'h':
            resetGame();
            isStart = false;
            break;
        }
    }

    switch (key)
    {
    case 9: // TAB key
        //isStart = !isStart ? true : false;
        isStart = true;
        countdownActive = true;
        startTime = glutGet(GLUT_ELAPSED_TIME);
        break;
    case 'L':
    case 'l':
        gameIntro = false;
        showLeaderboard = true;
        printf("Show Leaderboard: %s\n", showLeaderboard ? "true" : "false");
        break;
    }

    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (isStart) {
        switch (key)
        {
            // MOVE LEFT & RIGHT (STOP)
        case 'a':
        case 'A':
            moveLeft = false;
            break;
        case 'd':
        case 'D':
            moveRight = false;
            break;
        }
    }

    glutPostRedisplay();
}

// KEYBOARD FUNCTION | ARROW KEYS FUNCTION 
void specialKeys(int key, int x, int y)
{

    if (isStart) {
        switch (key)
        {
            // MOVE LEFT & RIGHT 
        case GLUT_KEY_LEFT:
            moveLeft = true;
            break;
        case GLUT_KEY_RIGHT:
            moveRight = true;
            break;
        }
    }

    glutPostRedisplay();
}

void specialKeysUp(int key, int x, int y)
{
    if (isStart) {
        switch (key)
        {
            // MOVE LEFT & RIGHT (STOP)
        case GLUT_KEY_LEFT:
            moveLeft = false;
            break;
        case GLUT_KEY_RIGHT:
            moveRight = false;
            break;
        }
    }
}

// ============================ UPDATE FUNCTION ============================

// UPDATE FUNCTION | APPLY GRAVITY
void applyGravity()
{
    // USER NOT ON GROUND
    if (!isOnGround)
    {
        // APPLY GRAVITY
        if (changeGravity == 1) gravity = 1.0f; // DEFAULT GRAVITY
        else if (changeGravity == 2) gravity = 1.5f; // LESS GRAVITY
        else if (changeGravity == 3) gravity = 0.5f; // STRONGER GRAVITY

        velocityY -= gravity * 0.1f; // printf("%f\n", velocityY);
        userY += velocityY; //printf("%f\n", userY);
    }

    // USER ON GROUND
    if (userY <= 0.0f)
    {
        userY = 0.0f;
        velocityY = 0.0f;
        isOnGround = true;
    }

    if (isOnGround) {
        jumpCount = 0;   // reset when touching ground
    }

}

// MOVING PLATFORM
void movePlatforms() {
    for (int i = 0; i < platformCount; i++) {
        platformX[i] += platformSpeed[i] * platformDirection[i];

        // Kapag sumobra sa limit, magpalit ng direction
        if (platformX[i] > 18.0f || platformX[i] < 2.0f) {
            platformDirection[i] *= -1;
        }
    }
}

// PLATFORM COLLISION
void platformCollision()
{
    for (int i = 0; i < platformCount; i++)
    {
        // PLATFORM BOUNDS
        float platLeft = platformX[i] - platformHalfWidth[i];
        float platRight = platformX[i] + platformHalfWidth[i];
        float platTop = platformY[i] + 1.5f;
        float platBottom = platformY[i];

        // OBJECT BOUNDS
        float userLeft = userX + userSizeLeft;
        float userRight = userX + userSizeRight;
        float userBottom = userY + userSizeBottom;
        float userTop = userY + userSizeTop;

        if (
            userLeft < platRight &&
            userRight > platLeft &&
            userTop > platBottom &&
            userBottom < platTop
            )
        {
            // LANDING ON TOP
            if (userBottom <= platTop && velocityY < 0) {
                userY = platTop - 3.0f;
                velocityY = 0.0f;

                // MOVING WITH PLATFORM
                userX += platformSpeed[i] * platformDirection[i];

                // Only award score once per landing
                if (!landedOnPlatform[i]) {
                    score += 10;
                    landedOnPlatform[i] = true;
                }

                // -------------------------------------------------- ADDITIONAL JUMP STRENGTH --------------------------------------------------
                if (i == 2) jumpStrength = 1.20f;

                // THEORY MODIFICATION 
            }

            // HITTING FROM BELOW
            else if (userTop >= platBottom && userBottom < platBottom && velocityY > 0)
            {
                userY = platBottom - 3.0f; printf("Legit %f\n", userY);
                velocityY = 0.0f;
            }

            // COLLISION LEFT
            else if (userRight > platLeft && userLeft < platLeft)
            {
                userX = platLeft - 3.0f; printf("Left Collision\n");
            }

            // COLLISION RIGHT
            else if (userLeft < platRight && userRight > platRight)
            {
                userX = platRight - 3.0f; printf("Right Collision\n");
            }
        }
    }
}

// UPDATE FUNCTION | ENEMY FUNCTION
void enemyFunction()
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    for (int i = 0; i < enemyCount; i++)
    {
        enemyY[i] -= 0.1f;  // bagsak

        /*if (stage == 3) {
            enemyY[i] -= 0.1f;
        }*/


        // CHECKING COLLISION WITH PLATFORMS
        for (int j = 0; j < platformCount; j++)
        {
            float platLeft = platformX[j] - platformHalfWidth[j];
            float platRight = platformX[j] + platformHalfWidth[j];
            float platTop = platformY[j] + 0.5f;

            // ENEMY OBJECT BOUNDS
            float enemyLeft = enemyX[i];
            float enemyRight = enemyX[i] + 0.5f;
            float enemyBottom = enemyY[i];
            float enemyTop = enemyY[i] + 0.5f;

            // USER OBJECT BOUNDS
            float userLeft = userX + 2.0f;
            float userRight = userX + 3.0f;
            float userBottom = userY + 2.0f;
            float userTop = userY + 3.0f;

            // Kapag tumuntong sa ibabaw ng platform
            if (enemyLeft < platRight && enemyRight > platLeft &&
                enemyBottom <= platTop && enemyTop > platformY[j])
            {
                enemyY[i] = 20.0f;
                enemyX[i] = rand() % 20;
            }

            // ENEMY COLLIDE TO THE USER
            if (
                userLeft < enemyRight &&
                userRight > enemyLeft &&
                userBottom < enemyTop &&
                userTop > enemyBottom
                ) {
                if (currentTime - lastHitTime > hitDelay)
                {
                    printf("Last Hit Time: %f\n", lastHitTime);
                    printf("Hit Delay: %f\n", hitDelay);
                    printf("Current Time: %f\n", currentTime);

                    lives--;
                    lastHitTime = currentTime; // RESET TIMER

                    // GAME OVER PROGRAM
                    if (lives <= 0) {
                        isGameOver = true;
                    }
                }
            }
        }

        // IF GROUND ON DOWN
        if (enemyY[i] < 2.0f)
        {
            enemyY[i] = 20.0f;
            enemyX[i] = rand() % 20;
        }
    }
}

// UPDATE FUNCTION | PER STAGE 
void proceedToNextStage()
{
    stage++;

    for (int i = 0; i < platformCount; i++)
    {
        landedOnPlatform[i] = false;
    }

    if (stage == 2) {
        isWallUpRightOpen = false;
        remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
        countdownActiveForPortal = false;
    }

    if (stage == 3) {
        isWallUpRightOpen = false;
        remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
        countdownActiveForPortal = false;
    }
}

// --------------------------------------  UPDATE LOGIC: GAME OVER  --------------------------------------
void isGameOverLogicFunction()
{
    // typingName = true;

    printf("Game Over: %s\n", isGameOver ? "true" : "false");
    printf("Typing Name: %s\n", typingName ? "true" : "false");
    printf("Show Leaderboard: %s\n", showLeaderboard ? "true" : "false");

    if (remainingTime > highestRemainingCountdownTime) {
        highestRemainingCountdownTime = remainingTime;
    }
}

// ADDITIONAL UPDATE
// --------------------------- ACTIVITE #1 (START) | UPDATE ---------------------------
// WALL MIDDLE UP : COLLISION
void wallMiddleUpCollision()
{
    // USER BOUNDS
    float userLeft = userX + 2.0f;
    float userRight = userX + 3.0f;
    float userBottom = userY + 2.0f;
    float userTop = userY + 3.0f;

    if (
        userLeft < wallMiddleRight &&
        userRight > wallMiddleLeft &&
        userBottom < wallMiddleTop &&
        userTop > wallMiddleBottom
        ) {
        // LEFT COLLISION
        if (userRight > wallMiddleLeft && userLeft < wallMiddleLeft)
        {
            userX = wallMiddleLeft - userSizeRight; // align sa left side ng wall
        }

        // RIGHT COLLISION
        else if (userLeft < wallMiddleRight && userRight > wallMiddleRight)
        {
            userX = wallMiddleRight - userSizeLeft; // align sa right side ng wall
        }

        // BOTTOM COLLISION (pag tumama sa ilalim ng wall habang tumatalon)
        if (velocityY > 0.0f && userTop > wallMiddleBottom && userBottom < wallMiddleBottom)
        {
            userY = wallMiddleBottom - userSizeTop;
            velocityY = 0.0f;
        }
    }
}

// WALL LEFT UP : COLLISION
void wallLeftUpCollision()
{
    // USER BOUNDS
    float userLeft = userX + userSizeLeft;
    float userRight = userX + userSizeRight;
    float userBottom = userY + userSizeBottom;
    float userTop = userY + userSizeTop;

    // USER & WALL COLLISION 
    if (
        userLeft < wallLeftRight &&
        userRight > wallLeftLeft &&
        userBottom < wallLeftTop &&
        userTop > wallLeftBottom
        )
    {
        if (velocityY > 0 && userBottom < wallLeftBottom)
        {
            // HEAD BUMP
            userY = wallLeftBottom - userSizeTop;
            velocityY = 0;
        }
        else if (userRight > wallLeftLeft && userLeft < wallLeftLeft)
        {
            // HIT LEFT SIDE
            userX = wallLeftLeft - userSizeRight;
        }
        else if (userLeft < wallLeftRight && userRight > wallLeftRight)
        {
            // HIT RIGHT SIDE
            userX = wallLeftRight - userSizeLeft;
        }
    }
}

// WALL RIGHT UP : COLLISION
void wallRightUpCollision()
{
    if (isWallUpRightOpen == true) return;

    // USER BOUNDS
    float userLeft = userX + userSizeLeft;
    float userRight = userX + userSizeRight;
    float userBottom = userY + userSizeBottom;
    float userTop = userY + userSizeTop;

    // USER & WALL COLLISION 
    if (
        userLeft < wallRightRight &&
        userRight > wallRightLeft &&
        userBottom < wallRightTop &&
        userTop > wallRightBottom
        )
    {
        if (velocityY > 0 && userBottom < wallRightBottom)
        {
            // HEAD BUMP
            userY = wallRightBottom - userSizeTop;
            velocityY = 0;
        }
        else if (userRight > wallRightLeft && userLeft < wallRightLeft)
        {
            // HIT LEFT SIDE
            userX = wallRightLeft - userSizeRight;
        }
        else if (userLeft < wallRightRight && userRight > wallRightRight)
        {
            // HIT RIGHT SIDE
            userX = wallRightRight - userSizeLeft;
        }
    }
}

// KEY : COLLISION
void keyCollision()
{
    // USER BOUNDS
    float userLeft = userX + 2.0f;
    float userRight = userX + 3.0f;
    float userBottom = userY + 2.0f;
    float userTop = userY + 3.0f;

    // JUMP USER TO WALL BOTTOM
    // AABB collision check (correct axis checks)
    if (
        userLeft < keyRight &&
        userRight > keyLeft &&
        userBottom < keyTop &&
        userTop > keyBottom
    ) {

        isWallUpRightOpen = true;
        printf("Wall Open: %s\n", isWallUpRightOpen ? "true" : "false");
        countdownActiveForPortal = true;
        startTimeForPortalKeyShesh = glutGet(GLUT_ELAPSED_TIME); // MARK EXACT COLLISION TIME
        remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
    }
}

void wallLeftRightMainCollsion()
{
    // LEFT AND RIGHT WALL BOUNDS
    float wallLeft = 0.0f;
    float wallRight = 19.5f;

    // USER BOUNDS
    float userLeft = userX + 2.0f;
    float userRight = userX + 3.0f;

    if (userLeft < wallLeft) {
        userX = wallLeft - userSizeLeft;
    }
}
// --------------------------- ACTIVITE #1 (END) | UPDATE ---------------------------

// ============================ ADDITIONAL (START) | UPDATE IMPLEMENTATION OBJECT THEORY MODIFICATION ============================
void windUpdate()
{
    if (isWindActivate) {
        if (stage == 3) {
            userX += 0.10f * windDirection;
        }
        else {
            userX += 0.05f * windDirection;
        }
    }
}

void updateWindVisibility()
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastToggleWindTime > toggleWindDelay)
    {
        isWindActivate = !isWindActivate;
        lastToggleWindTime = currentTime;

        // Randomize direction: either -1 or 1
        windDirection = (rand() % 2 == 0) ? -1 : 1;

        printf("Wind toggled: %s | Direction: %s\n",
            isWindActivate ? "ON" : "OFF",
            windDirection == 1 ? "RIGHT" : "LEFT");
    }
}

// ---------------- #2 STAR / HEART ---------------- 
//void updateItem() {
//    // oscillation movement
//    itemOffsetX += itemDirection * itemSpeed;
//    if (itemOffsetX > itemRange || itemOffsetX < -itemRange) {
//        itemDirection *= -1.0f;
//    }
//
//    // attach to middle platform (index 1)
//    int platformIndex = 1;
//    float platformCenterX = platformX[platformIndex];
//    float platformTopY = platformY[platformIndex] + 0.5f;
//
//    itemLeft = platformCenterX - 0.5f + itemOffsetX;
//    itemRight = platformCenterX + 0.5f + itemOffsetX;
//    itemBottom = platformTopY;
//    itemTop = platformTopY + 1.0f;
//}
//
//void checkItemCollision() {
//    if (itemCollected) return; // already collected
//
//    // player bounding box
//    float playerLeft = userX + userSizeLeft;
//    float playerRight = userX + userSizeRight;
//    float playerBottom = userY + userSizeBottom;
//    float playerTop = userY + userSizeTop;
//
//    // AABB collision (Axis-Aligned Bounding Box)
//    bool overlap = !(playerRight < itemLeft ||
//        playerLeft > itemRight ||
//        playerTop < itemBottom ||
//        playerBottom > itemTop);
//
//    if (overlap) {
//        itemCollected = true;
//        score += 100;   // add points
//        lives += 1;     // or give extra life
//        std::cout << "Item collected!" << std::endl;
//    }
//}

// ============================ ADDITIONAL (END) | UPDATE IMPLEMENTATION OBJECT THEORY MODIFICATION ============================

// UPDATE FUNCTION MAIN
void update(int value)
{
    // START PLATFORM GAME
    if (!isStart) {
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }
    else {
        // PAUSE
        if (isPaused) {
            glutPostRedisplay();
            glutTimerFunc(16, update, 0);
            return;
        }

        // GAME OVER
        if (isGameOver || stage == 4) {
            isGameOverLogicFunction();

            glutPostRedisplay();
            glutTimerFunc(16, update, 0);
            return;
        }



        // MOVE LEFT & RIGHT
        if (moveLeft) userX -= 0.15f;
        if (moveRight) userX += 0.15f;

        // TESTING
        /*printf("Velocity Y: ------- %f\n", velocityY);
        printf("User Y: ------- %f\n", userY);
        printf("Wall Open: %s\n", isWallUpRightOpen ? "true" : "false");*/
        if (userY > 18.0f) {
            userX = 7.5f;
            userY = 0.0f;

            proceedToNextStage();
        }

        


        // -------- ADDITIONAL (START) | UPDATE OBJECT DECLARATION THEORY MODIFICATION --------
        if (stage == 2) {
            updateWindVisibility();
            windUpdate();
        }

        if (stage == 3) {
            updateWindVisibility();
            windUpdate();
        }

        /*updateItem();
        checkItemCollision();*/

        // -------- ADDITIONAL (END) | UPDATE OBJECT DECLARATION THEORY MODIFICATION -------- 

        

        // APPLY GRAVITY
        applyGravity();

        // ENEMY FUNCTION
        enemyFunction();

        movePlatforms();

        // PLATFORM COLLISION
        platformCollision();

        // WALL LEFT & RIGHT COLLISION
        wallLeftRightMainCollsion();

        // ACTIVITY #1 | WALL UP COLLISION 
        wallLeftUpCollision();
        wallMiddleUpCollision();
        wallRightUpCollision();
        keyCollision();

        // TESTING
        printf("Stage : %d\n", stage);
        printf("Remaining Time : %d\n", remainingTime);
        printf("Key Remaining Time : %d\n", remainingTimeForPortalKeyShesh);
        // printf("Highest Remaining Countdown Time : %d\n", highestRemainingCountdownTime);
        printf("Wall Open: %s\n", isWallUpRightOpen ? "true" : "false");
        printf("Countdown Active For Portal : %s\n", countdownActiveForPortal ? "true" : "false");
        printf("Typing Name : %s\n", typingName ? "true" : "false");
        printf("Panalo Ba : %s\n", isWin ? "true" : "false");
        // printf("Stage #: %s\n");

        if (countdownActive) {
            int currentTime = glutGet(GLUT_ELAPSED_TIME);
            int elapsed = (currentTime - startTime) / 1000;
            remainingTime = countdownTime - elapsed;

            if (remainingTime <= 0) {
                remainingTime = 0;
                countdownActive = false;
                isGameOver = true;
            }
        }

        // SEPARATE TIMER FOR PORTAL KEY
        if (countdownActiveForPortal) {
            int currentTimePortalKey = glutGet(GLUT_ELAPSED_TIME);
            int elapsedPortalKey = (currentTimePortalKey - startTimeForPortalKeyShesh) / 1000;
            remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh - elapsedPortalKey;

            if (remainingTimeForPortalKeyShesh <= 0) {
                remainingTimeForPortalKeyShesh = countdownTimeForPortalKeyShesh;
                countdownActiveForPortal = false;
                isWallUpRightOpen = false;
            }
        }

        updateCreditsPosition();
        renderCredits();


        // ============================ ADDITIONAL (END) | UPDATE IMPLEMENTATION THEORY MODIFICATION ============================
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ============================ ENEMY IMAGE ============================

GLuint loadEnemyTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        printf("Failed to load image: %s\n", filename);
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}


// ============================ BACKGROUND IMAGE ============================
GLuint backgroundTex;

void drawBackground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTex);
    glColor3f(1.0f, 1.0f, 1.0f); // Keep original colors

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);   // Bottom Left
    glTexCoord2f(1.0f, 0.0f); glVertex2f(20.0f, 0.0f);  // Bottom Right
    glTexCoord2f(1.0f, 1.0f); glVertex2f(20.0f, 20.0f); // Top Right
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 20.0f);  // Top Left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// ============================ DISPLAY FUNCTION ============================ 
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glOrtho(0, 20, 0, 20, -1, 1);
    drawBackground();

    objectProgram();

    // WHEN THE GAME IS NOT YET STARTED - DISPLAY INTRO (START)
    if (isStart) {
        // DISPLAY LIVES
        char livesText[20];
        glColor3f(1.0f, 1.0f, 1.0f); // RED
        snprintf(livesText, sizeof(livesText), "Lives: %d", lives);
        renderBitmapString(2.0f, 1.0f, livesText);

        // DISPLAY SCORE
        char scoreText[20];
        glColor3f(1.0f, 1.0f, 1.0f); // BLACK
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        renderBitmapString(15.0f, 1.0f, scoreText);
    }

    glFlush();
}

GLuint platformTexture;
// ============================ MAIN FUNCTION ============================ 
void timer(int value) {
    glutPostRedisplay();           // REDRAW
    glutTimerFunc(16, timer, 0);   // ~60 FPS
}



int main(int arg, char** argv)
{
    glutInit(&arg, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    // WINDOW CONFIGURATION (APPLICATION SETTINGS)
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(posX, posY);

    // APPLICATION TITLE
    glutCreateWindow("Application namin ni papi mosh");
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutTimerFunc(16, update, 0);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutKeyboardFunc(keyboard); // Keyboard Keys Manipulation
    glutKeyboardUpFunc(keyboardUp);
    glClearColor(0.678f, 0.847, 0.902f, 1.0f); // background

    // ENEMY LOGIC
    // enemyLogic();

    // PICTURE LOADED TEXTURE
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    loadWallRightUpTexture();
    loadWallMiddleUpTexture();
    loadWallLeftUpTexture();

    backgroundTex = loadTexture("Images/Glut_Image/background1.png");

    userIdle = loadTexture("Images/Glut_Image/standing1.png");

    userWalkRight[0] = loadTexture("Images/Glut_Image/walk-right1.png");
    userWalkRight[1] = loadTexture("Images/Glut_Image/walk-right2.png");
    userWalkRight[2] = loadTexture("Images/Glut_Image/walk-right3.png");
    userWalkRight[3] = loadTexture("Images/Glut_Image/walk-right4.png");

    userWalkLeft[0] = loadTexture("Images/Glut_Image/walk-left1.png");
    userWalkLeft[1] = loadTexture("Images/Glut_Image/walk-left2.png");
    userWalkLeft[2] = loadTexture("Images/Glut_Image/walk-left3.png");
    userWalkLeft[3] = loadTexture("Images/Glut_Image/walk-left4.png");

    keyTexture = loadTexture("Images/Glut_Image/key.png");
    platformTexture = loadTexture("Images/Glut_Image/wood floating platform.png");

    enemyTextures[0] = loadTexture("Images/Glut_Image/meteor2/Meteor/meteor1-0000.png");
    enemyTextures[1] = loadTexture("Images/Glut_Image/meteor2/Meteor/meteor2-0000.png");
    enemyTextures[2] = loadTexture("Images/Glut_Image/meteor2/Meteor/meteor3-0000.png");
    loadTexture();

    glutMainLoop();

    return 0;
}