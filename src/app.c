#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"

App app_load(const char* file_path) {
   void* handle = dlopen(file_path, RTLD_LAZY);
   if (handle == NULL) {
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
   }

   App_Init app_init = dlsym(handle, "app_init");
   App_Deinit app_deinit = dlsym(handle, "app_deinit");
   App_Update app_update = dlsym(handle, "app_update");
   App_Render app_render = dlsym(handle, "app_render");

   char* error = dlerror();
   if (error != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
   }

   return (App){
      .init = app_init,
      .deinit = app_deinit,
      .update = app_update,
      .render = app_render,
   };
}
