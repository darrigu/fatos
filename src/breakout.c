#include <raylib.h>
#include "app.h"

#define PROJ_SIZE_SCALE 0.025f
#define PROJ_SPEED 500.0f
#define PAD_LEN_SCALE 0.1f
#define PAD_THICK_SCALE PROJ_SIZE_SCALE
#define PAD_SPEED PROJ_SPEED

float screen_width;
float screen_height;
float screen_min_dim;
float proj_size;
float proj_x;
float proj_y;
float proj_dx;
float proj_dy;
float pad_len;
float pad_thick;
float pad_x;
float pad_y;
bool paused;

void handle_resize(void) {
   screen_width = GetScreenWidth();
   screen_height = GetScreenHeight();
   screen_min_dim = MIN(screen_width, screen_height);

   proj_size = screen_min_dim*PROJ_SIZE_SCALE;
   pad_len = screen_width*PAD_LEN_SCALE;
   pad_thick = screen_height*PAD_THICK_SCALE;
   pad_x = screen_width/2.0f - pad_len/2.0f;
   pad_y = screen_height - pad_thick - screen_height*0.1f;
}

void app_init(void) {
   handle_resize();

   proj_x = 100.0f;
   proj_y = 100.0f;
   proj_dx = 1.0f;
   proj_dy = 1.0f;
   paused = false;
}

void app_deinit(void) {}

void app_update(void) {
   if (IsWindowResized()) handle_resize();

   if (IsKeyPressed(KEY_SPACE)) paused = !paused;
   if (paused) return;

   float frame_time = GetFrameTime();

   float proj_nx = proj_x + proj_dx*PROJ_SPEED*frame_time;
   if (proj_nx < 0.0f || proj_nx + proj_size > screen_width || CheckCollisionRecs((Rectangle){proj_nx, proj_y, proj_size, proj_size}, (Rectangle){pad_x, pad_y, pad_len, pad_thick})) {
      proj_dx *= -1.0f;
      proj_nx = proj_x + proj_dx*PROJ_SPEED*frame_time;
   }
   proj_x = proj_nx;

   float proj_ny = proj_y + proj_dy*PROJ_SPEED*frame_time;
   if (proj_ny < 0.0f || proj_ny + proj_size > screen_height || CheckCollisionRecs((Rectangle){proj_x, proj_ny, proj_size, proj_size}, (Rectangle){pad_x, pad_y, pad_len, pad_thick})) {
      proj_dy *= -1.0f;
      proj_ny = proj_y + proj_dy*PROJ_SPEED*frame_time;
   }
   proj_y = proj_ny;

   float pad_dx = 0.0f;
   if (IsKeyDown(KEY_LEFT)) pad_dx -= 1.0f;
   if (IsKeyDown(KEY_RIGHT)) pad_dx += 1.0f;

   pad_x += pad_dx*PAD_SPEED*frame_time;
}

void app_render(void)
{
   ClearBackground(GetColor(0x000000FF));

   DrawRectangleRec((Rectangle){proj_x, proj_y, proj_size, proj_size}, GetColor(0xFFFFFFFF));
   DrawRectangleRec((Rectangle){pad_x, pad_y - pad_thick/2.0f, pad_len, pad_thick}, GetColor(0xFF0000FF));
}
