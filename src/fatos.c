#include <dlfcn.h>

#include <raylib.h>
#include "./app.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

int main(void) {
   SetConfigFlags(FLAG_WINDOW_RESIZABLE);
   InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FatOS");

   SetExitKey(0);
   SetTargetFPS(60);

   App main_menu = app_load("./main_menu.so");
   App current_app = main_menu;
   main_menu.init();

   while (!WindowShouldClose()) {
      // TODO: find a cool way to prevent exitting from main menu
      if (IsKeyPressed(KEY_ESCAPE)) {
         current_app.deinit();
         current_app = main_menu;
         current_app.init();
      }
      current_app.update(&current_app);
      BeginDrawing();
      current_app.render();
      EndDrawing();
   }

   main_menu.deinit();
   CloseWindow();
   return 0;
}
