#include <math.h>
#include <stdlib.h>
#include <raylib.h>
#include "app.h"

#define ITEM_SIZE_SCALE 0.30f
#define ITEM_GAP_SCALE 0.1f
#define FRAME_THICK_SCALE 0.008f
#define FRAME_GAP_SCALE 0.015f
#define FRAME_COLOR ((Color){100, 255, 255, 255})
#define FRAME_GLOW_COLOR ((Color){100, 255, 255, 50})
#define TITLE_FONT_SCALE 0.04f
#define TITLE_COLOR ((Color){230, 255, 255, 255})
#define TITLE_GLOW_COLOR ((Color){100, 255, 255, 100})

#define FONT_PATH "res/fonts/Rajdhani-Medium.ttf"

#define SELECTION_ANIM_SPEED 8.0f
#define HOVER_PULSE_SPEED 3.0f

float lerpf(float a, float b, float t) {
   return a*(1.0f - t) + (b*t);
}

typedef struct {
   const char* title;
   Texture2D cover;
   const char* app_path;
} Item;

float screen_width;
float screen_height;
float screen_min_dim;
Shader bg;
int bg_time_loc;
int bg_res_loc;
float title_font_size;
Font title_font;
float item_size;
float item_gap;
#define ITEMS_COUNT 2
Item items[ITEMS_COUNT];
float frame_gap;
float frame_thick;
int selected;
float screen_offset;

void handle_resize(void) {
   screen_width = GetScreenWidth();
   screen_height = GetScreenHeight();
   screen_min_dim = MIN(screen_width, screen_height);

   item_size = screen_min_dim*ITEM_SIZE_SCALE;
   item_gap = screen_min_dim*ITEM_GAP_SCALE;
   frame_gap = screen_min_dim*FRAME_GAP_SCALE;
   frame_thick = screen_min_dim*FRAME_THICK_SCALE;
   title_font_size = screen_min_dim*TITLE_FONT_SCALE;
}

void app_init(void) {
   handle_resize();

   bg = LoadShader(0, "res/shaders/neon.glsl");
   bg_time_loc = GetShaderLocation(bg, "time");
   bg_res_loc = GetShaderLocation(bg, "resolution");

   title_font = LoadFontEx(FONT_PATH, 128, NULL, 0);
   SetTextureFilter(title_font.texture, TEXTURE_FILTER_TRILINEAR);

   items[0] = (Item){
      .title = "BREAKOUT",
      .cover = LoadTexture("res/covers/breakout.png"),
      .app_path = "./breakout.so",
   };
   items[1] = (Item){
      .title = "SNAKE",
      .cover = LoadTexture("res/covers/snake.png"),
      .app_path = "./snake.so",
   };
}

void app_update(App* current_app) {
   if (IsWindowResized()) handle_resize();

   if (IsKeyPressed(KEY_RIGHT)) selected = MIN(selected + 1, ITEMS_COUNT - 1);
   if (IsKeyPressed(KEY_LEFT)) selected = MAX(selected - 1, 0);

   float target_screen_offset = selected*(item_size + item_gap);
   screen_offset += (target_screen_offset - screen_offset)*SELECTION_ANIM_SPEED*GetFrameTime();

   if (IsKeyPressed(KEY_ENTER)) {
      App app = app_load(items[selected].app_path);
      *current_app = app;
      app.init();
   }
}

void draw_neon_frame(float x, float y, float size, float thickness, Color color) {
   float glow_intensity = (sinf(GetTime()*3.0f)*0.3f) + 0.7f;

   for (int i = 0; i < 3; i++) {
      float glow_size = size + thickness*i*2.0f;
      float glow_x = x - (glow_size - size)/2.0f;
      float glow_y = y - (glow_size - size)/2.0f;
      DrawRectangleLinesEx(
         (Rectangle){glow_x, glow_y, glow_size, glow_size},
         thickness,
         ColorAlpha(FRAME_GLOW_COLOR, (0.3f - i*0.1f)*glow_intensity)
      );
   }

   Color frame_color = ColorAlpha(color, glow_intensity*0.8f + 0.2f);
   DrawRectangleLinesEx(
      (Rectangle){x, y, size, size},
      thickness,
      frame_color
   );
}

void app_render(void) {
   float time = GetTime();
   Vector2 res = {screen_width, screen_height};
   BeginShaderMode(bg);
   SetShaderValue(bg, bg_time_loc, &time, SHADER_UNIFORM_FLOAT);
   SetShaderValue(bg, bg_res_loc, &res, SHADER_UNIFORM_VEC2);
   DrawRectangle(0, 0, res.x, res.y, WHITE);
   EndShaderMode();

   for (int i = 0; i < ITEMS_COUNT; i++) {
      Item item = items[i];

      float x = screen_width/2.0f - item_size/2.0f + i*(item_size + item_gap) - screen_offset;
      float y = screen_height/2.0f - item_size/2.0f;

      float draw_scale = (i == selected) ? 1.15f : 1.0f;
      float draw_size = item_size*draw_scale;
      float draw_x = x - (draw_size - item_size)/2.0f;
      float draw_y = y - (draw_size - item_size)/2.0f;

      DrawTexturePro(item.cover,
         (Rectangle){0.0f, 0.0f, item.cover.width, item.cover.height},
         (Rectangle){draw_x, draw_y, draw_size, draw_size},
         (Vector2){0.0f, 0.0f},
         0.0f,
         WHITE);

      if (i == selected) {
         float frame_size = draw_size + 2.0f*frame_gap;
         float frame_x = draw_x - frame_gap;
         float frame_y = draw_y - frame_gap;
         draw_neon_frame(frame_x, frame_y, frame_size, frame_thick, FRAME_COLOR);

         Vector2 title_size = MeasureTextEx(title_font, item.title, title_font_size, 1.0f);
         float text_y = draw_y + draw_size + frame_gap + frame_thick;

         for (int i = 0; i < 3; i++) {
            DrawTextEx(title_font, item.title,
               (Vector2){draw_x + draw_size/2.0f - title_size.x/2.0f + i - 1,
                         text_y + i - 1},
               title_font_size, 1.0f,
               ColorAlpha(TITLE_GLOW_COLOR, 0.3f - i*0.1f));
         }

         DrawTextEx(title_font, item.title,
            (Vector2){draw_x + draw_size/2.0f - title_size.x/2.0f,
                      text_y},
            title_font_size, 1.0f,
            TITLE_COLOR);
      }
   }
}
