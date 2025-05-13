#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define SRC_FOLDER "src/"
#define BUILD_FOLDER "build/"
#define RES_FOLDER "res/"

const char* app_names[] = {
   "main_menu",
   "breakout",
};

bool build_app(Cmd* cmd, const char* app_name) {
   size_t mark = temp_save();

   const char* so_path = temp_sprintf("%s%s.so", BUILD_FOLDER, app_name);
   const char* src_path = temp_sprintf("%s%s.c", SRC_FOLDER, app_name);
   nob_cc(cmd);
   nob_cc_flags(cmd);
   cmd_append(cmd, "-fPIC", "-shared");
   nob_cc_output(cmd, so_path);
   nob_cc_inputs(cmd, SRC_FOLDER "app.c", src_path);
   cmd_append(cmd, "-lraylib");
   if (!cmd_run_sync_and_reset(cmd)) return false;

   nob_log(INFO, "--- %s finished ---", so_path);

   temp_rewind(mark);
   return true;
}

bool build_fatos(Cmd* cmd) {
   size_t mark = temp_save();

   const char* bin_path = BUILD_FOLDER "fatos";
   nob_cc(cmd);
   nob_cc_flags(cmd);
   nob_cc_output(cmd, bin_path);
   nob_cc_inputs(cmd, SRC_FOLDER "app.c", SRC_FOLDER "fatos.c");
   cmd_append(cmd, "-lraylib");
   if (!cmd_run_sync_and_reset(cmd)) return false;

   nob_log(INFO, "--- %s finished ---", bin_path);

   temp_rewind(mark);
   return true;
}

int main(int argc, char** argv) {
   NOB_GO_REBUILD_URSELF(argc, argv);

   if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;
   if (!copy_directory_recursively(RES_FOLDER, BUILD_FOLDER RES_FOLDER)) return 1;

   Cmd cmd = {0};
   if (!build_fatos(&cmd)) return 1;

   for (size_t i = 0; i < ARRAY_LEN(app_names); i++) {
      if (!build_app(&cmd, app_names[i])) return 1;
   }

   return 0;
}
