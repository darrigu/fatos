#ifndef APP_H
#define APP_H

typedef struct App App;

typedef void (*App_Init)(void);
typedef void (*App_Deinit)(void);
typedef void (*App_Update)(App*);
typedef void (*App_Render)(void);

struct App {
   App_Init init;
   App_Deinit deinit;
   App_Update update;
   App_Render render;
};

App app_load(const char* file_path);

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#endif // APP_H
