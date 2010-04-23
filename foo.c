#include "SDL.h"
#include <cairo.h>

#include <math.h>

int fps = 30;
int w = 320;
int h = 240;
float scale = 1.0;

typedef float vec[4];
typedef float mat[4 * 4];

void mat_identity(mat m) {
  m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
  m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
  m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
  m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void vec_mat_mul(vec v, mat m) {
  float x, y, z, w;
  x = v[0] * m[ 0] + v[1] * m[ 1] + v[2] * m[ 2] + v[3] * m[ 3];   
  y = v[0] * m[ 4] + v[1] * m[ 5] + v[2] * m[ 6] + v[3] * m[ 7];
  z = v[0] * m[ 8] + v[1] * m[ 9] + v[2] * m[10] + v[3] * m[11];
  w = v[0] * m[12] + v[1] * m[13] + v[2] * m[14] + v[3] * m[15];
  v[0] = x; v[1] = y; v[2] = z; v[3] = w;
}

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  cairo_t *cr;

  Uint8 *keystate;
  Uint32 next_frame;

  float aspect = 1.0 * w/h;

  mat m;

  int running;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_ShowCursor(0);
  SDL_SetVideoMode(w, h, 32, 0);
  sdl_surface = SDL_GetVideoSurface();

  { /* Initialize Canvas */
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_for_data(
      sdl_surface->pixels,
      CAIRO_FORMAT_RGB24,
      sdl_surface->w,
      sdl_surface->h,
      sdl_surface->pitch
      );
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);
  }

  // Cartesian
  cairo_translate(cr, w/2.0, h/2.0);
  cairo_scale(cr, 1, -1);

  // fixed scale
  cairo_scale(cr, h/scale, h/scale);

  /* Initialize Delay */
  next_frame = 1024.0 / fps;

  /* Game Logic */
  running = 1;
  {
    float a = M_PI / 4.0;
    mat_identity(m);
    m[11] = 8;
  }

  SDL_LockSurface(sdl_surface);
  while (running) {

    /* Render Frame */
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    {
      vec v;
      float x, y;

      v[0] = -1; v[1] = -1; v[2] = 0; v[3] = 1;
      vec_mat_mul(v, m);
      x = v[0] / v[2]; y = v[1] / v[2];
      cairo_move_to(cr, x, y);

      v[0] =  1; v[1] = -1; v[2] = 0; v[3] = 1;
      vec_mat_mul(v, m);
      x = v[0] / v[2]; y = v[1] / v[2];
      cairo_line_to(cr, x, y);

      v[0] =  1; v[1] =  1; v[2] = 0; v[3] = 1;
      vec_mat_mul(v, m);
      x = v[0] / v[2]; y = v[1] / v[2];
      cairo_line_to(cr, x, y);

      v[0] = -1; v[1] =  1; v[2] = 0; v[3] = 1;
      vec_mat_mul(v, m);
      x = v[0] / v[2]; y = v[1] / v[2];
      cairo_line_to(cr, x, y);

      cairo_close_path(cr);
      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
      cairo_fill(cr);
    }

    /* Update Display */
    SDL_UnlockSurface(sdl_surface);
    SDL_Flip(sdl_surface);
    SDL_LockSurface(sdl_surface);

    /* Delay */
    {
      Uint32 now;
      now = SDL_GetTicks();
      if (now < next_frame) {
        SDL_Delay(next_frame - now);
      }
      next_frame = next_frame + 1024.0 / fps;
    }

    /* Game Logic */
    SDL_PumpEvents();
    keystate = SDL_GetKeyState(NULL);
    if (keystate[SDLK_q]) {
      running = 0;
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
