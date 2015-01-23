////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#define OCTET_BULLET 1

#include "../../octet.h"

#include "perlin.h"
#include "bmp_image.h"
#include "ppm.h"
#include "inputs.h"
#include "terrain_gen.h"

/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::terrain_gen app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}


