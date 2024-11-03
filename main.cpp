#include "raylib.h"

int main() {
    SetTargetFPS(15);

    InitWindow(1200, 800, "Renderer");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Hello", 0, 0, 30, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}