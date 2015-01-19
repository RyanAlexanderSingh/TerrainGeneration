////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

namespace octet {
  /// Scene containing a box with octet.
  class terrain_gen : public app {

    ppm ppm_image;
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

      mat4t &camera_mat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
      camera_mat.translate(150, 150, 150);
      camera_mat.rotateY(-90);
      camera_mat.rotateX(0);

      //start to generate the perlin noise for the terrain generation
      generate(true);

      //create the shape for the skydome and texture it
      //TODO: update the skybox properly
      //create_skybox();
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

      key_presses();

      int mx, my;
      get_mouse_pos(mx, my);
      mouse_control(mx, my, vy);
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

    void generate(bool from_image){

      int height = 0, width = 0;

      if (from_image){
        ppm_image.read("heightmap_exp1.ppm");
        height = ppm_image.height, width = ppm_image.width;
      }
      else{
        height = 450, width = 650;
        ppm_image.init(height, width);
      }

      //the mesh generatiion
      param_shader *shader = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
      material *red = new material(vec4(0.6f, 0.298f, 0.0f, 1.0f), shader);

      mesh *terrain = new mesh();
      // allocate vertices and indices into OpenGL buffers
      size_t num_vertices = height*width;
      size_t num_indices = 6 * (num_vertices - width) - 6 * height;
      terrain->allocate(sizeof(my_vertex)* num_vertices, sizeof(uint32_t)* num_indices);
      terrain->set_params(sizeof(my_vertex), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);
      // describe the structure of my_vertex to OpenGL

      terrain->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
      terrain->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
      terrain->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 24, GL_TRUE);

      gl_resource::wolock vl(terrain->get_vertices());
      my_vertex *vtx = (my_vertex *)vl.u8();
      gl_resource::wolock il(terrain->get_indices());
      uint32_t *idx = idx = il.u32();
      //end of init

      unsigned int kk = 0;
      
      // Visit every pixel of the image and assign a color generated with Perlin noise
      for (int i = 0; i < height; ++i) {     // y
        for (int j = 0; j < width; ++j) {  // x
          float x = 1.0f * j / width;
          float y = 1.0f * i / height;

          if(!from_image) {
            // Typical Perlin noise
            float n = pn.generate_noise(10.0f * x, 10.0f * y, 0.8f, 12, false);
            //cap the perlin noise to prevent it from going positive numbers

            // Map the values to the [0, 255] interval, for simplicity we use tones of grey
            ppm_image.pixel_colour[kk] = floorf(255.0f * n);
           
          }
          float colour_mesh = (float)ppm_image.pixel_colour[kk] / 255.0f;
                   
          float vertex_height =  colour_mesh * 30.0f;
          //printf("%f\n", actual_vert_height);
          vtx->pos = vec3p((float)j, vertex_height, (float)i);
          vtx->nor = vec3p((float)j, vertex_height, (float)i);
         
          if (colour_mesh <= 0.1f)
            vtx->color = make_color(0.0f, 0.0f, 1.0f); 
          else
          if (colour_mesh <= 0.15f&&colour_mesh >= 0.1f)
            vtx->color = make_color(0.0f, 0.65f, 0.0f);
          else
          vtx->color = make_color(colour_mesh, colour_mesh, colour_mesh);

          vtx++;

          kk++;
        }
      }

      int tempvert = 0;
      for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {

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

      if (!from_image)
      ppm_image.write("perlin_noise.ppm");
    }

    void create_skybox(){
      //create the fake skybox
      material *skybox_mat = new material(new image("assets/seamless_sky.jpg"));
      mesh_sphere *skydome = new mesh_sphere(100.0f, 400.0f, 3);
      scene_node *node = new scene_node();
      app_scene->add_child(node);
      app_scene->add_mesh_instance(new mesh_instance(node, skydome, skybox_mat));
    }

    void key_presses(){

      mat4t &camera_mat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();

      if (is_key_down(key_esc)){
        exit(0);
      }

      //camera movement keys
      if (is_key_down(key::key_shift))
      {
        camera_mat.translate(0, 5, 0);
      }
      if (is_key_down(key::key_ctrl))
      {
        camera_mat.translate(0, -5, 0);
      }
      if (is_key_down(key::key_up) || is_key_down('W'))
      {
        camera_mat.translate(0, 0, -5);
      }
      if (is_key_down(key::key_down) || is_key_down('S'))
      {
        camera_mat.translate(0, 0, 5);
      }
      if (is_key_down(key::key_left) || is_key_down('A'))
      {
        camera_mat.translate(-5, 0, 0);
      }
      if (is_key_down(key::key_right) || is_key_down('D'))
      {
        camera_mat.translate(5, 0, 0);
      }
    }

    void mouse_control(int mouse_x, int mouse_y, int viewpoint_y){

      mat4t &camera_mat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
      mat4t modelToWorld;

      modelToWorld.loadIdentity();
      modelToWorld[3] = vec4(camera_mat.w().x(), camera_mat.w().y(), camera_mat.w().z(), 1);
      modelToWorld.rotateY((float)-mouse_x*2.0f);
      if (viewpoint_y / 2 - mouse_y < 70 && viewpoint_y / 2 - mouse_y > -70)
        modelToWorld.rotateX((float)viewpoint_y / 2 - mouse_y);
      if (viewpoint_y / 2 - mouse_y >= 70)
        modelToWorld.rotateX(70);
      if (viewpoint_y / 2 - mouse_y <= -70)
        modelToWorld.rotateX(-70);
      camera_mat = modelToWorld;//apply to the node
    }
  };
}
