#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"

const int WIDTH = 700;
const int HEIGHT = 525;

struct AppState {
    bool hFramesEditMode{false};
    int hFramesValue{1};
    bool vFramesEditMode{false};
    int vFramesValue{1};
    bool PlayAnimChecked{false};
    bool RotationChecked{false};
};

struct SpriteData {
    std::string path;
    long modTime;
    Texture2D tex;
    Vector2 origin;
    float rotation{0};
    Rectangle texRec;
    std::vector<Rectangle> drawRecs;
};

SpriteData CreateSprite() {
    Texture2D tex;
    std::string path = "";
    long modTime;

    FilePathList droppedFile = LoadDroppedFiles();

    if (droppedFile.count == 1) {
        path = droppedFile.paths[0];
        modTime = GetFileModTime(droppedFile.paths[0]);

        Image tempImg = LoadImage(droppedFile.paths[0]);
        ImageFlipVertical(&tempImg);

        if (tempImg.data != nullptr) {
            UnloadTexture(tex);
            tex = LoadTextureFromImage(tempImg);
        }
    }

    UnloadDroppedFiles(droppedFile);

    return SpriteData{path, modTime, tex};
}

void UpdateModifiedSprite(SpriteData &sprite) {
    Image tempImg = LoadImage(sprite.path.c_str());
    ImageFlipVertical(&tempImg);

    if (tempImg.data != nullptr) {
        UnloadTexture(sprite.tex);
        sprite.tex = LoadTextureFromImage(tempImg);
    }
}

void UpdateSpriteFrames(SpriteData &sprite, uint32_t hFrames, uint32_t vFrames, float scale) {
    uint32_t frameWidth = sprite.tex.width / hFrames;
    uint32_t frameHeight = sprite.tex.height / vFrames;

    if (sprite.drawRecs.size() > 0) sprite.drawRecs.clear();

    for (auto i = 0; i < hFrames; i++) {
        Rectangle rec = {WIDTH / 2.0f, ((HEIGHT / 2.0f) + hFrames * 4) - (i * 8),
                         (float)frameWidth * scale, (float)frameHeight * scale};
        sprite.drawRecs.push_back(rec);
    }

    sprite.texRec = {0.0f, 0.0f, (float)frameWidth, (float)frameHeight};
    sprite.origin = {(float)(frameWidth * scale) / 2.0f, (float)(frameHeight * scale) / 2.0f};
}

int main() {
    uint32_t currentFrame = 0;
    AppState state;
    SpriteData mainSprite;

    InitWindow(WIDTH, HEIGHT, "StackAnim");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_TOPMOST);

    while (!WindowShouldClose()) {
        // File
        if (IsFileDropped()) {
            mainSprite = CreateSprite();
            UpdateSpriteFrames(mainSprite, state.hFramesValue, state.vFramesValue, 8.0f);
        }

        // Check if sprite has been modified
        if (mainSprite.modTime != GetFileModTime(mainSprite.path.c_str())) {
            std::cout << "sprite modified!" << std::endl;
            UpdateModifiedSprite(mainSprite);
            mainSprite.modTime = GetFileModTime(mainSprite.path.c_str());
        }

        // Rotation
        if (state.RotationChecked) mainSprite.rotation += GetFrameTime() * 20;

        // Drawing
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        if (mainSprite.drawRecs.size() == 0) {
            DrawText("Drag sprite to the window", (WIDTH / 2.0f) - 135, (HEIGHT / 2.0f) - 20, 20,
                     GRAY);
        }

        // Small texture preview
        DrawTexture(mainSprite.tex, 15, 15, WHITE);

        // Stacked drawing
        for (auto i = 0; i < mainSprite.drawRecs.size(); i++) {
            mainSprite.texRec.x = (float)i * (float)mainSprite.tex.width / state.hFramesValue;
            mainSprite.texRec.y = currentFrame * (float)mainSprite.tex.height / state.vFramesValue;
            DrawTexturePro(mainSprite.tex, mainSprite.texRec, mainSprite.drawRecs[i],
                           mainSprite.origin, mainSprite.rotation, WHITE);
        }

        // GUI
        if (GuiSpinner(Rectangle{580, 16, 100, 24}, "H-Frames ", &state.hFramesValue, 1, 100,
                       state.hFramesEditMode)) {
            std::cout << state.hFramesValue << std::endl;
            state.hFramesEditMode = !state.hFramesEditMode;
        }

        GuiCheckBox(Rectangle{580, 144, 24, 24}, " Rotate", &state.RotationChecked);

        UpdateSpriteFrames(mainSprite, state.hFramesValue, state.vFramesValue, 8.0f);

        EndDrawing();
    }

    UnloadTexture(mainSprite.tex);

    CloseWindow();

    return 0;
}