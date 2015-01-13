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

      app_scene->get_camera_instance(0)->set_far_plane(10000);
      app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(150, 150, 150);
      app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateY(-90);
      app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().rotateX(0);

      generate_noise();
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
      if (is_key_down(key::key_shift))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 5, 0);
      }
      if (is_key_down(key::key_ctrl))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -5, 0);
      }

      if (is_key_down(key::key_up))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -5);
      }
      if (is_key_down(key::key_down))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, 5);
      }
      if (is_key_down(key::key_left))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(-5, 0, 0);
      }
      if (is_key_down(key::key_right))
      {
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(5, 0, 0);
      }

      int xm, ym;
      get_viewport_size(vx, vy);
      get_mouse_pos(xm, ym);

      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld[3] = vec4(app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().w().x(), app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().w().y(), app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().w().z(), 1);
      modelToWorld.rotateY((float)-xm*2.0f);
      if (vy / 2 - ym < 70 && vy / 2 - ym > -70)
        modelToWorld.rotateX(vy / 2 - ym);
      if (vy / 2 - ym >= 70)
        modelToWorld.rotateX(70);
      if (vy / 2 - ym <= -70)
        modelToWorld.rotateX(-70);
      app_scene->get_camera_instance(0)->get_node()->access_nodeToParent() = modelToWorld;//apply to the node

    }
    struct my_vertex {
      vec3p pos;
      vec3p nor;
      uint32_t color;
    };
    static uint32_t make_color(float r, float g, float b) {
      return 0xff000000 + ((int)(r*255.0f) << 0) + ((int)(g*255.0f) << 8) + ((int)(b*255.0f) << 16);
    }
    vec3 matrixmult(mat4t objm, vec3 direction)
    {
      vec3 temp(objm.x().x()*direction.x() + objm.y().x()*direction.y() + objm.z().x()*direction.z(),
        objm.x().y()*direction.x() + objm.y().y()*direction.y() + objm.z().y()*direction.z(),
        objm.x().z()*direction.x() + objm.y().z()*direction.y() + objm.z().z()*direction.z());
      return temp;
    }


    void generate_noise(){
      int height = 450, width = 600;
      image.init(height, width);

      //the mesh generatiion
      param_shader *shader = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
      material *red = new material(vec4(1, 0, 0, 1), shader);

      mesh *terrain = new mesh();
      // allocate vertices and indices into OpenGL buffers
      size_t num_vertices = height*width;
      size_t num_indices = 6 * (num_vertices - width) - 6 * height;
      terrain->allocate(sizeof(my_vertex) * num_vertices, sizeof(uint32_t) * num_indices);
      terrain->set_params(sizeof(my_vertex), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);
      // describe the structure of my_vertex to OpenGL
      terrain->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
      terrain->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 12, GL_TRUE);
      gl_resource::wolock vl(terrain->get_vertices());
      my_vertex *vtx = (my_vertex *)vl.u8();
      gl_resource::wolock il(terrain->get_indices());
      uint32_t *idx = idx = il.u32();
      //end of init

      int tempvert = 0;

      unsigned int kk = 0;
      // Visit every pixel of the image and assign a color generated with Perlin noise
      for (int i = 0; i < height; ++i) {     // y
        for (int j = 0; j < width; ++j) {  // x
          float x = (float)j / ((float)width);
          float y = (float)i / ((float)height);

          // Typical Perlin noise
          float n = pn.generate_noise(10.0f * x, 10.0f * y, 0.8f, 10);

          // Wood like structure
          /*n = 20 * pn.generate_noise(x, y, 0.8f, 10);
          n = n - floorf(n);*/

          // Map the values to the [0, 255] interval, for simplicity we use tones of grey
          image.pixel_colour[kk] = (uint8_t)floorf(255 * n);

          float colour_val = floorf(255 * n) / 255.0f;
          float vh = colour_val*30;
          vtx->pos = vec3p(j, vh, i);
          vtx->nor = vec3p(j, vh, i);
          vtx->color = make_color(1, 0, 0);
          vtx++;

          kk++;
        }
      }
      tempvert = 0;
      for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {
          //0---------(1).....(width-1)
          //|     \    |
          //(width)-(width+1)......(2*width-1)
          //|      \   |
          //(2*width)-(2*width+1)...(3*width-1)
          idx[0] = tempvert;
          idx[1] = tempvert + 1;
          idx[2] = tempvert + width + 1;
          idx += 3;

          idx[0] = tempvert;
          idx[1] = tempvert + width + 1;
          idx[2] = tempvert + width;
          idx += 3;

          tempvert++;
        }
        tempvert++;
      }

      scene_node *node = new scene_node();
      app_scene->add_child(node);
      app_scene->add_mesh_instance(new mesh_instance(node, terrain, red));

      image.write("perlin_noise.ppm");
    }

  };
}
