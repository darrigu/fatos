#include <stddef.h>

#include <raylib.h>
#include "app.h"

#define PAD_LEN_SCALE 0.1f
#define PAD_THICK_SCALE PROJ_SIZE_SCALE
#define PAD_SPEED PROJ_SPEED
#define PROJ_SIZE_SCALE 0.025f
#define PROJ_SPEED 500.0f
#define TARGET_WIDTH_SCALE PAD_LEN_SCALE
#define TARGET_HEIGHT_SCALE PAD_THICK_SCALE
#define TARGET_PAD_SCALE 0.05f
#define TARGETS_CAP 128

typedef struct {
   float x;
   float y;
   bool dead;
} Target;

static Target targets_pool[TARGETS_CAP];
size_t targets_pool_count = 0;

float screen_width;
float screen_height;
float screen_min_dim;
float pad_len;
float pad_thick;
float pad_x;
float pad_y;
float pad_dx;
float proj_size;
float proj_x;
float proj_y;
float proj_dx;
float proj_dy;
float target_width;
float target_height;
float target_pad;
bool started;
bool paused;

void handle_resize(void) {
   screen_width = GetScreenWidth();
   screen_height = GetScreenHeight();
   screen_min_dim = MIN(screen_width, screen_height);

   pad_len = screen_width*PAD_LEN_SCALE;
   pad_thick = screen_height*PAD_THICK_SCALE;
   pad_y = screen_height - pad_thick - screen_height*0.1f;

   proj_size = screen_min_dim*PROJ_SIZE_SCALE;

   if (!started) {
      pad_x = screen_width/2.0f - pad_len/2.0f;
      proj_x = screen_width/2.0f - proj_size/2.0f;
      proj_y = pad_y - pad_thick/2.0f - proj_size;
   }

   target_width = screen_width*TARGET_WIDTH_SCALE;
   target_height = screen_height*TARGET_HEIGHT_SCALE;
   target_pad = screen_width*TARGET_PAD_SCALE;

   targets_pool_count = 0;
   #define TARGET(X, Y) targets_pool[targets_pool_count].x = (X); targets_pool[targets_pool_count++].y = (Y);
   TARGET(100.0f, 100.0f);
   TARGET(100.0f + target_width + target_pad, 100.0f);
   TARGET(100.0f + (target_width + target_pad)*2, 100.0f);
   TARGET(100.0f + (target_width + target_pad)*3, 100.0f);
   TARGET(100.0f + (target_width + target_pad)*4, 100.0f);
   #undef TARGET
}

void app_init(void) {
   pad_dx = 0.0f;
   proj_dx = 1.0f;
   proj_dy = 1.0f;
   started = false;
   paused = false;

   for (size_t i = 0; i < targets_pool_count; i++) {
      targets_pool[i].dead = false;
   }

   handle_resize();
}

void app_deinit(void) {}

void app_update(void) {
   if (IsWindowResized()) handle_resize();

   pad_dx = 0.0f;
   if (IsKeyDown(KEY_LEFT)) {
      pad_dx -= 1.0f;
      if (!started) {
         proj_dx = -1.0f;
         started = true;
      }
   }
   if (IsKeyDown(KEY_RIGHT)) {
      pad_dx += 1.0f;
      if (!started) {
         proj_dx = 1.0f;
         started = true;
      }
   }

   if (IsKeyPressed(KEY_SPACE)) paused = !paused;
   if (paused || !started) return;

   float frame_time = GetFrameTime();

   pad_x = MIN(screen_width - pad_len, MAX(0, pad_x + pad_dx*PAD_SPEED*frame_time));

   float proj_nx = proj_x + proj_dx*PROJ_SPEED*frame_time;
   bool cond_x = proj_nx < 0.0f || proj_nx + proj_size > screen_width || CheckCollisionRecs((Rectangle){proj_nx, proj_y, proj_size, proj_size}, (Rectangle){pad_x, pad_y, pad_len, pad_thick});
   for (size_t i = 0; i < targets_pool_count; i++) {
      if (cond_x) break;
      Target* target = &targets_pool[i];
      if (!target->dead) {
         cond_x = cond_x || CheckCollisionRecs((Rectangle){proj_nx, proj_y, proj_size, proj_size}, (Rectangle){target->x, target->y, target_width, target_height});
         if (cond_x) target->dead = true;
      }
   }
   if (cond_x) {
      proj_dx *= -1.0f;
      proj_nx = proj_x + proj_dx*PROJ_SPEED*frame_time;
   }
   proj_x = proj_nx;

   float proj_ny = proj_y + proj_dy*PROJ_SPEED*frame_time;
   bool cond_y = proj_ny < 0.0f || proj_ny + proj_size > screen_height;
   if (!cond_y) {
       cond_y = cond_y || CheckCollisionRecs((Rectangle){proj_x, proj_ny, proj_size, proj_size}, (Rectangle){pad_x, pad_y, pad_len, pad_thick});
       if (cond_y && pad_dx != 0.0f) {
          proj_dx = pad_dx;
       }
   }
   for (size_t i = 0; i < targets_pool_count; i++) {
      if (cond_y) break;
      Target* target = &targets_pool[i];
      if (!target->dead) {
         cond_y = cond_y || CheckCollisionRecs((Rectangle){proj_x, proj_ny, proj_size, proj_size}, (Rectangle){target->x, target->y, target_width, target_height});
         if (cond_y) target->dead = true;
      }
   }
   if (cond_y) {
      proj_dy *= -1.0f;
      proj_ny = proj_y + proj_dy*PROJ_SPEED*frame_time;
   }
   proj_y = proj_ny;
}

void app_render(void) {
   ClearBackground(GetColor(0x000000FF));

   DrawRectangleRec((Rectangle){pad_x, pad_y - pad_thick/2.0f, pad_len, pad_thick}, GetColor(0xFF0000FF));
   DrawRectangleRec((Rectangle){proj_x, proj_y, proj_size, proj_size}, GetColor(0xFFFFFFFF));

   for (size_t i = 0; i < targets_pool_count; i++) {
      Target target = targets_pool[i];
      if (target.dead) continue;
      DrawRectangleRec((Rectangle){target.x, target.y - target_height/2.0f, target_width, target_height}, GetColor(0x00FF00FF));
   }
}
