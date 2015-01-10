////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// 
//

namespace octet {
  /// Scene containing a box with octet.
  class ppm : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    ppm(int argc, char **argv) : app(argc, argv) {
    }



    /// this is called once OpenGL is initialized
    void app_init() {

    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {

    }
  };
}
