#include "main.hpp"

static void *screen;

static bool inMenu = false;
static bool ctrl = false;

void set_pixel(FT_Int x, FT_Int y, unsigned char r, unsigned char g,
               unsigned char b) {
  if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
    return;
  int pos = y * SCREEN_WIDTH + x;
  ((volatile unsigned short *)screen)[pos] =
      ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

void draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y) {
  FT_Int i, j, p, q, c;
  FT_Int x_max = x + bitmap->width;
  FT_Int y_max = y + bitmap->rows;

  for (i = x, p = 0; i < x_max; i++, p++) {
    for (j = y, q = 0; j < y_max; j++, q++) {
      if (i < 0 || j < 0 || i >= SCREEN_WIDTH || j >= SCREEN_HEIGHT)
        continue;

      c = bitmap->buffer[q * bitmap->width + p];

      if (c > 0)
        set_pixel(i, j, 255 - c, 255 - c, 255 - c);
    }
  }
}

FT_Error draw_string(FT_Face face, FT_Int x, FT_Int y, char *str) {
  FT_GlyphSlot slot = face->glyph;
  FT_Vector pen;
  FT_Error error;

  pen.x = x * 64;
  pen.y = y * 64;

  size_t chars = strlen(str);

  if (chars > 30) {
    chars = 30;
  }

  for (size_t n = 0; n < chars; n++) {
    FT_Set_Transform(face, 0, &pen);
    error = FT_Load_Char(face, str[n], FT_LOAD_RENDER);

    if (error)
      return error;

    draw_bitmap(&slot->bitmap, slot->bitmap_left,
                SCREEN_HEIGHT - slot->bitmap_top);

    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }

  return 0;
}

FT_Error draw(FT_Face face, std::shared_ptr<Stack> s) {
  memset(screen, 0xFF, 320 * 240 * sizeof(uint16_t));

  FT_Error error;

  char buf[30];
  for (uint8_t i = 0; i < 4; i++) {
    s->stack_format(buf, 30, i);
    error = draw_string(face, 0, 15 * i, buf);
    if (error) {
      return error;
    }
  }

  char *mode_string;

  switch (s->get_base()) {
  case 0:
    mode_string = (char *)"BIN";
    break;
  case 1:
    mode_string = (char *)"OCT";
    break;
  case 2:
    mode_string = (char *)"DEC";
    break;
  case 3:
  default:
    mode_string = (char *)"HEX";
    break;
  }

  error = draw_string(face, 0, 60, mode_string);
  if (error) {
    return error;
  }

  if (inMenu) {
    error = draw_string(face, 41, 60, (char *)"MENU");
    if (error) {
      return error;
    }
  }

  if (ctrl) {
    error = draw_string(face, 95, 60, (char *)"CTRL");
    if (error) {
      return error;
    }
  }

  lcd_blit(screen, SCR_320x240_565);

  return 0;
}

int main(int, char **argv) {
  FT_Library library;
  FT_Face face;
  // TODO: do proper error handling for freetype
  FT_Error error;

  if (lcd_type() != SCR_320x240_565) {
    return 0;
  }

  screen = malloc(320 * 240 * sizeof(uint16_t));
  memset(screen, 0xFF, SCREEN_WIDTH * (SCREEN_HEIGHT / 3) * sizeof(uint16_t));

  std::shared_ptr<Stack> stack(new Stack);
  bool keyHeld = true;

  lcd_init(SCR_320x240_565);

  error = FT_Init_FreeType(&library);
  if (error) {
    goto err_init;
  }

  // TODO: fix this path
  error = FT_New_Face(library, "./jetbrains.ttf.tns", 0, &face);
  if (error) {
    goto err_face;
  }

  error = FT_Set_Char_Size(face, 13 * 64, 0, 100, 0); /* set character size */
  if (error) {
    goto err;
  }

  error = draw_string(face, 0, 75, argv[0]);
  if (error) {
    goto err;
  }

  error = draw(face, stack);
  if (error) {
    goto err;
  }

  while (true) {
    if (keyHeld) {
      keyHeld =
          isKeyPressed(KEY_NSPIRE_ESC) || isKeyPressed(KEY_NSPIRE_1) ||
          isKeyPressed(KEY_NSPIRE_2) || isKeyPressed(KEY_NSPIRE_3) ||
          isKeyPressed(KEY_NSPIRE_4) || isKeyPressed(KEY_NSPIRE_5) ||
          isKeyPressed(KEY_NSPIRE_6) || isKeyPressed(KEY_NSPIRE_7) ||
          isKeyPressed(KEY_NSPIRE_8) || isKeyPressed(KEY_NSPIRE_9) ||
          isKeyPressed(KEY_NSPIRE_0) || isKeyPressed(KEY_NSPIRE_ENTER) ||
          isKeyPressed(KEY_NSPIRE_DEL) || isKeyPressed(KEY_NSPIRE_PLUS) ||
          isKeyPressed(KEY_NSPIRE_MINUS) || isKeyPressed(KEY_NSPIRE_MULTIPLY) ||
          isKeyPressed(KEY_NSPIRE_DIVIDE) || isKeyPressed(KEY_NSPIRE_PERIOD) ||
          isKeyPressed(KEY_NSPIRE_NEGATIVE) || isKeyPressed(KEY_NSPIRE_Z) ||
          isKeyPressed(KEY_NSPIRE_Y) || isKeyPressed(KEY_NSPIRE_X) ||
          isKeyPressed(KEY_NSPIRE_W) || isKeyPressed(KEY_NSPIRE_A) ||
          isKeyPressed(KEY_NSPIRE_B) || isKeyPressed(KEY_NSPIRE_C) ||
          isKeyPressed(KEY_NSPIRE_D) || isKeyPressed(KEY_NSPIRE_E) ||
          isKeyPressed(KEY_NSPIRE_F) || isKeyPressed(KEY_NSPIRE_RP) ||
          isKeyPressed(KEY_NSPIRE_LP) || isKeyPressed(KEY_NSPIRE_MENU) ||
          isKeyPressed(KEY_NSPIRE_EXP) || isKeyPressed(KEY_NSPIRE_SQU) ||
          isKeyPressed(KEY_NSPIRE_eEXP) || isKeyPressed(KEY_NSPIRE_TENX) ||
          isKeyPressed(KEY_NSPIRE_CTRL) || isKeyPressed(KEY_NSPIRE_SHIFT);
      continue;
    } else if (isKeyPressed(KEY_NSPIRE_ESC)) {
      break;
    } else if (isKeyPressed(KEY_NSPIRE_0)) {
      stack->enter_x(0);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_1)) {
      stack->enter_x(1);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_2)) {
      stack->enter_x(2);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_3)) {
      stack->enter_x(3);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_4)) {
      stack->enter_x(4);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_5)) {
      stack->enter_x(5);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_6)) {
      stack->enter_x(6);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_7)) {
      stack->enter_x(7);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_8)) {
      stack->enter_x(8);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_9)) {
      stack->enter_x(9);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_A)) {
      stack->enter_x(10);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_B)) {
      stack->enter_x(11);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_C)) {
      stack->enter_x(12);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_D)) {
      stack->enter_x(13);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_E)) {
      stack->enter_x(14);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_F)) {
      stack->enter_x(15);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_ENTER)) {
      stack->enter();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_DEL)) {
      stack->clear_x();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_PLUS)) {
      stack->add();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_MINUS)) {
      stack->sub();
      keyHeld = true;
    } else if (isKeyPressed(KEY_NSPIRE_MULTIPLY)) {
      stack->mul();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_DIVIDE)) {
      stack->div();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_PERIOD)) {
      stack->enter_point();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_NEGATIVE)) {
      stack->sign_x();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_Z)) {
      stack->set_base(0);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_W)) {
      stack->set_base(1);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_Y)) {
      stack->set_base(2);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_X)) {
      stack->set_base(3);
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_LP)) {
      stack->bit_shift_left();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_RP)) {
      stack->bit_shift_right();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_MENU)) {
      inMenu = !inMenu;
      // stack->change_precision();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_EXP)) {
      if (stack->get_base() == 2) {
        stack->exp();
      } else {
        stack->bit_xor();
      }
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_SQU)) {
      if (stack->get_base() == 2) {
        stack->square();
      } else {
        stack->bit_or();
      }
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_eEXP)) {
      stack->bit_not();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_TENX)) {
      stack->bit_and();
      keyHeld = true;
      ctrl = false;
    } else if (isKeyPressed(KEY_NSPIRE_CTRL)) {
      ctrl = true;
      keyHeld = true;
    } else if (isKeyPressed(KEY_NSPIRE_SHIFT)) {
      if (ctrl) {
        stack->rotate_down();
      } else {
        stack->rotate_up();
      }
      ctrl = false;
      keyHeld = true;
    } else {
      continue;
    }

    error = draw(face, stack);
    if (error) {
      goto err;
    }
  }

err:
  FT_Done_Face(face);

err_face:
  FT_Done_FreeType(library);

err_init:
  lcd_init(SCR_TYPE_INVALID);

  free(screen);

  return 0;
}
