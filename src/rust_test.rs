#[repr(C)]
pub struct Color {
   pub r: u8,
   pub g: u8,
   pub b: u8,
   pub a: u8,
}

extern "C" {
   pub fn ClearBackground(color: Color);
   pub fn DrawRectangle(posX: i32, posY: i32, width: i32, height: i32, color: Color);
}

#[no_mangle]
pub extern "C" fn app_init() {}

#[no_mangle]
pub extern "C" fn app_deinit() {}

#[no_mangle]
pub extern "C" fn app_update() {}

#[no_mangle]
pub unsafe extern "C" fn app_render() {
   ClearBackground(Color { r: 0, g: 0, b: 0, a: 255 });
   DrawRectangle(0, 0, 100, 100, Color { r: 255, g: 0, b: 0, a: 255 });
}
