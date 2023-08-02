#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "font_data.h"
#include "pixel_shader.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"

const int WIDTH = 500;
const int HEIGHT = 375;
const std::unordered_map<int, std::tuple<Color, int>> bkgColors = {{0, {LIGHTGRAY, 0x828282FF}},
                                                                   {1, {DARKGRAY, 0xC8C8C8FF}}};

struct AppState {
    bool configMode{false};
    bool hFramesEditMode{false};
    int hFramesValue{1};
    int tempHFramesValue{1};
    bool vFramesEditMode{false};
    int vFramesValue{1};
    int tempVFramesValue{1};
    bool frameEditMode{false};
    int framesValue{0};
    bool playAnimChecked{false};
    bool rotationChecked{true};
    bool pixelizerChecked{false};
    int bkgColorId{0};
    Color backgroundColor = LIGHTGRAY;
    int textColor{static_cast<int>(0x828282FF)};
    bool frameSpeedEditMode{false};
    float frameSpeedValue{1.0f};
    bool frameSpeedValueMode{false};
    Vector2 frameSize{1, 1};
    bool uiVisibilityChecked{true};
};

struct Sprite {
    std::string path;
    long modTime;
    Texture2D tex;
    Vector2 origin;
    float rotation{0};
    Rectangle texRec;
    std::vector<Rectangle> drawRecs;
    int currentFrame{0};
};

Sprite CreateSprite() {
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

    return Sprite{path, modTime, tex};
}

void UpdateModifiedSprite(Sprite &sprite) {
    Image tempImg = LoadImage(sprite.path.c_str());
    ImageFlipVertical(&tempImg);

    if (tempImg.data != nullptr) {
        UnloadTexture(sprite.tex);
        sprite.tex = LoadTextureFromImage(tempImg);
    }
}

void UpdateSpriteFrames(Sprite &sprite, uint32_t hFrames, uint32_t vFrames, float scale) {
    uint32_t frameWidth = sprite.tex.width / hFrames;
    uint32_t frameHeight = sprite.tex.height / vFrames;

    if (!sprite.drawRecs.empty()) sprite.drawRecs.clear();

    for (auto i = 0; i < hFrames; i++) {
        Rectangle rec = {WIDTH / 2.0f, ((HEIGHT / 2.0f) + hFrames * 4) - (i * 8), (float)frameWidth * scale,
                         (float)frameHeight * scale};
        sprite.drawRecs.push_back(rec);
    }

    sprite.texRec = {0.0f, 0.0f, (float)frameWidth, (float)frameHeight};
    sprite.origin = {(frameWidth * scale) / 2.0f, (frameHeight * scale) / 2.0f};
}

void ChangeBkgColor(AppState &state) {
    size_t colors = bkgColors.size();
    int nextColor = state.bkgColorId + 1 >= colors ? 0 : state.bkgColorId + 1;
    state.bkgColorId = nextColor;

    Color bkgColor = std::get<0>(bkgColors.at(nextColor));
    int textColor = std::get<1>(bkgColors.at(nextColor));

    state.backgroundColor = bkgColor;
    state.textColor = textColor;
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, state.textColor);
}

void DrawConfigMode(AppState &state) {
    if (GuiSpinner(Rectangle{390, 10, 100, 24}, "H-Frames ", &state.tempHFramesValue, 1, 100,
                   state.hFramesEditMode)) {
        state.hFramesEditMode = !state.hFramesEditMode;
    }
    if (GuiSpinner(Rectangle{390, 40, 100, 24}, "V-Frames ", &state.tempVFramesValue, 1, 100,
                   state.vFramesEditMode)) {
        state.vFramesEditMode = !state.vFramesEditMode;
    }
    if (GuiButton(Rectangle{390, 70, 100, 24}, "Confirm")) state.configMode = false;

    // Sprite frame size
    auto size = std::to_string((int)state.frameSize.x) + "x" + std::to_string((int)state.frameSize.y);
    GuiLabel(Rectangle{430, 342, 72, 24}, size.c_str());

    state.hFramesValue = state.tempHFramesValue <= 0 ? 1 : state.tempHFramesValue;
    state.vFramesValue = state.tempVFramesValue <= 0 ? 1 : state.tempVFramesValue;
}

void DrawPreviewMode(AppState &state, Sprite &sprite) {
    if (GuiSpinner(Rectangle{390, 10, 100, 24}, "Frame ", &sprite.currentFrame, 0, state.vFramesValue - 1,
                   state.frameEditMode)) {
        state.frameEditMode = !state.frameEditMode;
    }
    // TODO: Changing the value directly dosen't work for the custom float spinner
    if (GuiSpinnerF(Rectangle{390, 40, 100, 24}, "Frame Dur. (s) ", &state.frameSpeedValue, 0.1f, 1.0f, 0.1f,
                    state.frameSpeedEditMode)) {
        state.frameSpeedEditMode = !state.frameSpeedEditMode;
    }

    GuiCheckBox(Rectangle{390, 70, 24, 24}, " Rotate", &state.rotationChecked);

    GuiCheckBox(Rectangle{390, 100, 24, 24}, " Pixelizer", &state.pixelizerChecked);
    // TODO: Changing the background's color makes everything else hard to read or see.
    if (GuiButton(Rectangle{390, 130, 100, 24}, "#29#Bkg")) ChangeBkgColor(state);

    if (!state.playAnimChecked) {
        if (GuiButton(Rectangle{390, 160, 100, 24}, "#150#Play")) state.playAnimChecked = true;
    } else {
        if (GuiButton(Rectangle{390, 160, 100, 24}, "#149#Stop")) state.playAnimChecked = false;
    }

    if (GuiButton(Rectangle{466, 312, 24, 24}, "#44#")) state.uiVisibilityChecked = false;

    if (GuiButton(Rectangle{466, 342, 24, 24}, "#142#")) state.configMode = true;
}

int main() {
    bool spriteLoaded = false;
    size_t frameCount{0};
    AppState state;
    Sprite mainSprite;

    InitWindow(WIDTH, HEIGHT, "MotionStaker");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_TOPMOST);

    RenderTexture2D target = LoadRenderTexture(WIDTH, HEIGHT);

    Shader pixelShader = LoadShaderFromMemory(nullptr, pixelizer_frag);

    Font ubuFont = LoadFontFromMemory(".ttf", ___assets_Ubuntu_Regular_ttf, ___assets_Ubuntu_Regular_ttf_len,
                                      17, nullptr, 0);
    GuiSetFont(ubuFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 17);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, state.textColor);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x444444FF);

    while (!WindowShouldClose()) {
        // File
        if (IsFileDropped()) {
            state.configMode = true;
            state.playAnimChecked = false;
            state.pixelizerChecked = false;
            state.tempHFramesValue = 1;
            state.tempVFramesValue = 1;
            state.uiVisibilityChecked = true;

            mainSprite = CreateSprite();
            if (mainSprite.tex.id != 0) spriteLoaded = true;
            UpdateSpriteFrames(mainSprite, state.hFramesValue, state.vFramesValue, 1.0f);
        }

        // Show UI when it is hidden
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !state.uiVisibilityChecked)
            state.uiVisibilityChecked = true;

        // Check if sprite has been modified
        if (mainSprite.modTime != GetFileModTime(mainSprite.path.c_str())) {
            UpdateModifiedSprite(mainSprite);
            mainSprite.modTime = GetFileModTime(mainSprite.path.c_str());
        }

        // Update sprite frame size
        state.frameSize.x = mainSprite.texRec.width;
        state.frameSize.y = mainSprite.texRec.height;

        // Rotation
        if (state.rotationChecked) mainSprite.rotation += GetFrameTime() * 20;

        // Anim
        frameCount++;
        if (frameCount / (60 * state.frameSpeedValue) >= 1.0f && state.playAnimChecked) {
            if (mainSprite.currentFrame < state.vFramesValue - 1)
                ++mainSprite.currentFrame;
            else
                mainSprite.currentFrame = 0;

            frameCount = 0;
        }

        // Drawing
        BeginTextureMode(target);
        ClearBackground(state.backgroundColor);
        // Stacked drawing
        for (auto i = 0; i < mainSprite.drawRecs.size(); i++) {
            mainSprite.texRec.x = (float)i * (float)mainSprite.tex.width / state.hFramesValue;
            mainSprite.texRec.y = mainSprite.currentFrame * (float)mainSprite.tex.height / state.vFramesValue;
            DrawTexturePro(mainSprite.tex, mainSprite.texRec, mainSprite.drawRecs[i], mainSprite.origin,
                           mainSprite.rotation, WHITE);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(state.backgroundColor);

        // Small texture preview
        // DrawTexture(mainSprite.tex, 15, 15, WHITE);

        if (state.pixelizerChecked) {
            BeginShaderMode(pixelShader);
            DrawTextureRec(target.texture,
                           Rectangle{0, 0, (float)target.texture.width, (float)-target.texture.height},
                           Vector2{0, 0}, WHITE);
            EndShaderMode();
        } else {
            DrawTextureRec(target.texture,
                           Rectangle{0, 0, (float)target.texture.width, (float)-target.texture.height},
                           Vector2{0, 0}, WHITE);
        }

        // GUI
        if (!spriteLoaded) {
            GuiLabel(Rectangle{(WIDTH / 2.0f) - 100, (HEIGHT / 2.0f) - 20, 200, 20},
                     "Drag sprite to the window");
        }

        if (state.uiVisibilityChecked) {
            if (state.configMode) {
                DrawConfigMode(state);
            } else {
                DrawPreviewMode(state, mainSprite);
            }
        }

        UpdateSpriteFrames(mainSprite, state.hFramesValue, state.vFramesValue, 8.0f);

        EndDrawing();
    }

    UnloadTexture(mainSprite.tex);
    UnloadShader(pixelShader);
    UnloadRenderTexture(target);

    CloseWindow();

    return 0;
}