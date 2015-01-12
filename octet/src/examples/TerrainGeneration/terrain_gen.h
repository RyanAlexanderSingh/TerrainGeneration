////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>

namespace octet {
  /// Scene containing a box with octet.
  class terrain_gen : public app {

    ppm image;
    perlin pn;

    // scene for drawing box
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    terrain_gen(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene = new visual_scene();
      app_scene->create_default_camera_and_lights();

      std::string file = "perlin_noise.ppm";
      std::ifstream pnoise(file);
      if (pnoise.is_open()){
        printf("perlin noise file found, loading it...");
        image.read(file);
      }
      else{
        generate_noise();
      }
    }


  /// this is called to draw the world
  void draw_world(int x, int y, int w, int h) {
    int vx = 0, vy = 0;
    get_viewport_size(vx, vy);
    app_scene->begin_render(vx, vy);

    // update matrices. assume 30 fps.
    app_scene->update(1.0f / 30);

    // draw the scene
    app_scene->render((float)vx / vy);

    if (is_key_down(key_esc)){
      exit(0);
    }
  }

  void generate_noise(){
    int height = 450, width = 600;
    image.init(height, width);

    //create the perlin noise object
    pn.init();

    unsigned int kk = 0;
    // Visit every pixel of the image and assign a color generated with Perlin noise
    for (int i = 0; i < height; ++i) {     // y
      for (int j = 0; j < width; ++j) {  // x
        float x = (float)j / ((float)width);
        float y = (float)i / ((float)height);

        // Typical Perlin noise
        float n = pn.noise(10 * x, 10 * y, 0.8f);

        // Wood like structure
        /*n = 20 * pn.noise(x, y, 0.8f);
        n = n - floorf(n);*/

        // Map the values to the [0, 255] interval, for simplicity we use 
        // tones of grey
        image.r[kk] = (uint8_t)floorf(255 * n);
        image.g[kk] = (uint8_t)floorf(255 * n);
        image.b[kk] = (uint8_t)floorf(255 * n);
        kk++;
      }
    }
    image.write("perlin_noise.ppm");
  }

};
}
