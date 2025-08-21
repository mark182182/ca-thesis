#include "font.h"
#include "raylib_shim.h"
#include "const.h"
#include <assert.h>

Font fonts_by_type[MAX_FONT_TYPE] = {0};

void LoadFonts() {

  // FiraCode-Retina.ttf
  Font font = LoadFontEx(
      "resources/fonts/firacode/FiraCode-Retina.ttf", MAIN_FONT_SIZE, 0, 250);

  assert(font.baseSize != 0);

  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  fonts_by_type[FONT_TYPE_FIRA_CODE_RETINA] = font;
}
