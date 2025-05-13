#include <stdlib.h>

#include <raylib.h>
#include "app.h"

#define BACKGROUND_COLOR ((Color){0x2E, 0x34, 0x40, 0xFF})
#define ITEM_SIZE_SCALE 0.30f
#define ITEM_GAP_SCALE 0.1f
#define FRAME_THICK_SCALE 0.005f
#define FRAME_GAP_SCALE 0.01f
#define FRAME_COLOR ((Color){0xEC, 0xEF, 0xF4, 0xFF})
#define TITLE_FONT_SCALE 0.05f
#define TITLE_COLOR ((Color){0xEC, 0xEF, 0xF4, 0xFF})

typedef struct {
   const char* title;
   Texture2D cover;
   const char* app_path;
} Item;

float screen_width;
float screen_height;
float screen_min_dim;
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

void app_init(void) {
   screen_width = GetScreenWidth();
   screen_height = GetScreenHeight();
   screen_min_dim = MIN(screen_width, screen_height);

   item_size = screen_min_dim*ITEM_SIZE_SCALE;
   item_gap = screen_min_dim*ITEM_GAP_SCALE;
   frame_gap = screen_min_dim*FRAME_GAP_SCALE;
   frame_thick = screen_min_dim*FRAME_THICK_SCALE;
   title_font_size = screen_min_dim*TITLE_FONT_SCALE;

   title_font = LoadFontEx("res/fonts/Montserrat-Medium.ttf", 64, NULL, 0);
   SetTextureFilter(title_font.texture, TEXTURE_FILTER_BILINEAR);

   items[0] = (Item){
      .title = "Breakout",
      .cover = LoadTexture("res/covers/breakout.png"),
      .app_path = "./breakout.so",
   };

   items[1] = (Item){
      .title = "Snake",
      .cover = LoadTexture("res/covers/breakout.png"),
      .app_path = "./snake.so",
   };
}

void app_deinit(void) {
   UnloadFont(title_font);
   for (size_t i = 0; i < ITEMS_COUNT; i++) {
      UnloadTexture(items[i].cover);
   }
}

void app_update(App* current_app) {
   if (IsKeyPressed(KEY_RIGHT)) selected = MIN(selected + 1, ITEMS_COUNT - 1);
   if (IsKeyPressed(KEY_LEFT)) selected = MAX(selected - 1, 0);

   if (IsKeyPressed(KEY_ENTER)) {
      App app = app_load(items[selected].app_path);
      *current_app = app;
      app.init();
   }

   float target_screen_offset = selected*(item_size + item_gap);
   screen_offset += (target_screen_offset - screen_offset)*0.1f;
}

void app_render(void) {
   ClearBackground(BACKGROUND_COLOR);

   for (int i = 0; i < ITEMS_COUNT; i++) {
      Item item = items[i];

      float x = screen_width/2.0f - item_size/2.0f + i*(item_size + item_gap) - screen_offset;
      float y = screen_height/2.0f - item_size/2.0f;

      float draw_scale = i == selected ? 1.2f : 1.0f;
      float draw_size = item_size*draw_scale;
      float draw_x = x - (draw_size - item_size)/2.0f;
      float draw_y = y - (draw_size - item_size)/2.0f;

      DrawTexturePro(item.cover,
         (Rectangle){0.0f, 0.0f, item.cover.width, item.cover.height},
         (Rectangle){draw_x, draw_y, draw_size, draw_size},
         (Vector2){0.0f, 0.0f},
         0.0f,
         GetColor(0xFFFFFFFF));

      if (i == selected) {
         float frame_size = draw_size + 2.0f*frame_gap;
         float frame_x = draw_x - frame_gap;
         float frame_y = draw_y - frame_gap;
         DrawRectangleLinesEx((Rectangle){frame_x, frame_y, frame_size, frame_size}, frame_thick, FRAME_COLOR);

         Vector2 title_size = MeasureTextEx(title_font, item.title, title_font_size, 1.0f);
         DrawTextEx(title_font, item.title,
            (Vector2){draw_x + draw_size/2.0f - title_size.x/2.0f,
                      draw_y + draw_size + frame_gap + frame_thick},
            title_font_size, 1.0f,
            TITLE_COLOR);
      }
   }
}
