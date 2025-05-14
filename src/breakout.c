#include <math.h>
#include <stddef.h>

#include <raylib.h>
#include "app.h"

#define ROWS 4
#define COLS 6
#define PAD_LEN_SCALE 0.1f
#define PAD_THICK_SCALE PROJ_SIZE_SCALE
#define PAD_SPEED PROJ_SPEED
#define PAD_COLOR ((Color){100, 255, 255, 255})
#define PROJ_SIZE_SCALE 0.025f
#define PROJ_SPEED 500.0f
#define PROJ_COLOR ((Color){255, 255, 255, 255})
#define TARGET_WIDTH_SCALE PAD_LEN_SCALE
#define TARGET_HEIGHT_SCALE PAD_THICK_SCALE
#define TARGET_PADDING_SCALE 0.02f
#define TARGETS_CAP 128
#define TARGET_COLORS { \
   ((Color){255, 100, 100, 255}), \
   ((Color){100, 255, 100, 255}), \
   ((Color){100, 100, 255, 255}), \
   ((Color){255, 255, 100, 255})  \
}
#define GLOW_SIZE 0.015f
#define PULSE_SPEED 3.0f

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
   target_pad = screen_width*TARGET_PADDING_SCALE;

   targets_pool_count = 0;

   float grid_width = COLS*target_width + (COLS - 1)*target_pad;

   float start_x = (screen_width - grid_width)/2.0f;
   float start_y = screen_height*0.1f;

   for (size_t i = 0; i < ROWS; i++) {
       for (size_t j = 0; j < COLS; j++) {
           targets_pool[targets_pool_count].x = start_x + j * (target_width + target_pad);
           targets_pool[targets_pool_count].y = start_y + i * (target_height + target_pad);
           targets_pool_count++;
       }
   }
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
   ClearBackground((Color){10, 0, 20, 255});

   float pulse = (sinf(GetTime()*PULSE_SPEED)*0.3f + 0.7f);
   float glow_size = screen_min_dim*GLOW_SIZE;

   DrawRectangleRec(
      (Rectangle){pad_x - glow_size, pad_y - glow_size, 
                  pad_len + glow_size*2.0f, pad_thick + glow_size*2.0f},
      ColorAlpha(PAD_COLOR, 0.3f*pulse)
   );

   DrawCircle(proj_x + proj_size/2.0f, proj_y + proj_size/2.0f, 
              proj_size + glow_size, 
              ColorAlpha(PROJ_COLOR, 0.4f));

   DrawRectangleRounded(
      (Rectangle){pad_x, pad_y, pad_len, pad_thick}, 
      0.8f, 10, PAD_COLOR);
   DrawCircleV(
      (Vector2){proj_x + proj_size/2.0f, proj_y + proj_size/2.0f},
      proj_size/2.0f, PROJ_COLOR);

   Color target_colors[ROWS] = TARGET_COLORS;

   for (size_t i = 0; i < targets_pool_count; i++) {
      Target target = targets_pool[i];
      if (target.dead) continue;

      int row = i/COLS;
      Color baseColor = target_colors[row%ROWS];
        
      DrawRectangleRec(
         (Rectangle){target.x - glow_size/2.0f, target.y - glow_size/2.0f,
                     target_width + glow_size, target_height + glow_size},
         ColorAlpha(baseColor, 0.3f)
      );
        
      DrawRectangleRounded(
         (Rectangle){target.x, target.y, target_width, target_height},
         0.3f, 5, baseColor);
   }

   for (int x = 0; x < screen_width; x += screen_width/20.0f) {
      DrawLine(x, 0, x, screen_height, ColorAlpha(WHITE, 0.05f));
   }
   for (int y = 0; y < screen_height; y += screen_height/20.0f) {
      DrawLine(0, y, screen_width, y, ColorAlpha(WHITE, 0.05f));
   }
}
