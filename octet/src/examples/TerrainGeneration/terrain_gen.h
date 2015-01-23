////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#ifndef TERRAIN_GEN_H_INCLUDED
#define TERRAIN_GEN_H_INCLUDED

namespace octet {

  /// Scene containing a box with octet.
  class terrain_gen : public app {

    //bmp_image img;
    perlin pn;
    inputs inputs;
    bmp_image img_gen;

    // scene for drawing box
    ref<visual_scene> app_scene;

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
      InitDayNightCycle();
      initUI();

      //init other class files
      inputs.init(this);

      ice_pert_from = 7.0f;
      ice_pert_to = 10.0f;

      water_pert_from = 0.0f;
      water_pert_to = 4.0f;

      grass_pert_from = 4.0f;
      grass_pert_to = 7.0f;

      generate(false, false);

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

      mat4t &camera = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
      //run key_presses loop to check for inputs
      inputs.key_presses(camera);
      inputs.mouse_control(camera);
      DayNightCycle();
      updateUI(vx, vy);
      pop_up_clear();
      if (inputs.Q_KEY()){
        setup_pop_up(1);
        terrain_type = 0;
        app_scene->pop_mesh_instance();
        generate(false, true);
        setup_pop_up(4);
      }
      if (inputs.R_KEY()){
        setup_pop_up(2);
        terrain_type = 1;
        app_scene->pop_mesh_instance();
        generate(true, false);
        setup_pop_up(4);
      }
      if (inputs.P_KEY()){
        setup_pop_up(3);
        terrain_type = 2;
        app_scene->pop_mesh_instance();
        generate(false, false);
        setup_pop_up(4);
      }

      if (inputs.G_KEY())
      {
        switch (terrain_type)
        {
        case 0:
          setup_pop_up(1);
          terrain_type = 0;
          app_scene->pop_mesh_instance();
          generate(false, true);
          setup_pop_up(4);
          break;
        case  1:
          setup_pop_up(2);
          terrain_type = 1;
          app_scene->pop_mesh_instance();
          generate(true, false);
          setup_pop_up(4);
          break;
        case  2:
          setup_pop_up(3);
          terrain_type = 2;
          app_scene->pop_mesh_instance();
          generate(false, false);
          setup_pop_up(4);
          break;
        }

      }

      if (is_key_down(key::key_up))
      {
        switch (selected_attrib)
        {
        case 0: if (ice_pert_from <= ice_pert_to && ice_pert_from >= 0.0f)
          ice_pert_from += 0.01f;
          break;
        case 1:
          ice_pert_to += 0.01f;
          break;

        case 2:
          if (water_pert_from <= water_pert_to)
            water_pert_from += 0.01f;
          break;
        case 3:
          if (water_pert_to <= grass_pert_from)
            water_pert_to += 0.01f;
          break;

        case 4:
          if (grass_pert_from <= grass_pert_to)
          {
            grass_pert_from += 0.01f;
          }
          break;

        case 5:
          if (grass_pert_to <= ice_pert_from)
          {
            grass_pert_to += 0.01f;
          }
          break;
        }
      }

      if (is_key_down(key::key_down))
      {
        switch (selected_attrib)
        {
        case 0: if (ice_pert_from >= grass_pert_to)
          ice_pert_from -= 0.01f;
          break;
        case 1:if (ice_pert_to >= ice_pert_from)
          ice_pert_to -= 0.01f;
          break;
        case 2:if (water_pert_from >= 0.01f)
          water_pert_from -= 0.01f;
          break;
        case 3: if (water_pert_to >= water_pert_from)
        {
          water_pert_to -= 0.01f;
        }
                break;
        case 4: if (grass_pert_from >= water_pert_to)
        {
          grass_pert_from -= 0.01f;
        }
                break;
        case 5: if (grass_pert_to >= grass_pert_from)
        {
          grass_pert_to -= 0.01f;
        }
                break;
        }
      }

      if (is_key_going_up(key::key_right))
      {
        if (selected_attrib == 5)
          selected_attrib = 0;
        else
          selected_attrib++;
      }

      if (is_key_going_up(key::key_left))
      {
        if (selected_attrib == 0)
          selected_attrib = 5;
        else
          selected_attrib--;
      }
    }

    void generate(bool from_image, bool random_seed){



      int height = 5, width = 2;

      //the mesh generatiion
      param_shader *shader = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
      material *red = new material(vec4(0.6f, 0.298f, 0.0f, 1.0f), shader);

      mesh *terrain = new mesh();
      // allocate vertices and indices into OpenGL buffers
      size_t num_vertices = height * width; //height * width (change this possibly)
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
      my_vertex *basevert = vtx;
      float higest_point = 0.0f;
      float lowest_point = 100.0f;

      float temp_low = 1.0f;
      float temp_high = 0.0f;

      float min = 0.0f, max = 0.0f;
      pn.fill_image(min, max, 16);

      // Visit every pixel of the image and assign a color generated with Perlin noise
      for (int i = 0; i < height; ++i) {     // y
        for (int j = 0; j < width; ++j) {  // x
          float x = 1.0f * j / width;
          float y = 1.0f * i / height;



          //if (!from_image) {
          //  // Typical Perlin noise
          //  float n = pn.generate_noise(x, y, 0.8f, 16, random_seed);
          //  //cap the perlin noise to prevent it from going positive numbers

          //  // Map the values to the [0, 255] interval, for simplicity we use tones of grey
          //  ppm_image.pixel_colour[kk] = n;

          //  /*if (n < temp_low){
          //    temp_low = n;
          //    printf("low: %f \n ", temp_low);
          //  }
          //  if (n > temp_high){
          //    temp_high = n;
          //    printf(" high:  %f \n", temp_high);
          //  }*/

          //}

          float colour_mesh = 0;
          float vertex_height = colour_mesh * 30.0f;
          vtx->pos = vec3p((float)j, vertex_height, (float)i);
          vtx->nor = vec3p((float)j, vertex_height, (float)i);
          if (use_colour_map)
          {
            if (vertex_height >= water_pert_from && vertex_height <= water_pert_to)
              vtx->color = make_color(0.0f, 0.0f, 1.0f);
            else
              if (vertex_height >= grass_pert_from && vertex_height <= grass_pert_to)
                vtx->color = make_color(0.0f, 0.65f, 0.0f);
              else
                if (vertex_height >= ice_pert_from && vertex_height <= ice_pert_to)
                {
              vtx->color = make_color(0.647f, 0.949f, 0.95f);
                }
                else
                  vtx->color = make_color(0.3294f, 0.2705f, 0.1803f);
          }
          else
          {
            if (vertex_height >= higest_point)
            {
              higest_point = vertex_height;
            }
            if (vertex_height <= lowest_point)
            {
              lowest_point = vertex_height;
            }
          }
          //vtx->color = make_color(ppm_image.pixel_colour[kk], ppm_image.pixel_colour[kk], ppm_image.pixel_colour[kk]);
          vtx++;
          kk++;
        }
      }

      if (!use_colour_map)
      {
        use_colour_map = true;//make this false when the user wants us to find a colour map
        vtx = basevert;
        float height_change_value = (higest_point - lowest_point) / 2.0f;
        ice_pert_from = higest_point - height_change_value / 2.0f;
        ice_pert_to = higest_point;

        water_pert_from = lowest_point;
        water_pert_to = lowest_point + height_change_value / 8;

        grass_pert_from = lowest_point + height_change_value / 3;
        grass_pert_to = grass_pert_from + height_change_value / 8;

        for (int i = 0; i < height*width; i++)
        {
          float vertex_height = vtx->pos.y();
          if (vertex_height >= water_pert_from && vertex_height <= water_pert_to)
            vtx->color = make_color(0.0f, 0.0f, 1.0f);
          else
            if (vertex_height >= grass_pert_from && vertex_height <= grass_pert_to)
              vtx->color = make_color(0.0f, 0.65f, 0.0f);
            else
              if (vertex_height >= ice_pert_from && vertex_height <= ice_pert_to)
              {
            vtx->color = make_color(0.647f, 0.949f, 0.95f);
              }
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

      //ppm_image.write("perlin_noise.ppm");
    }


    scene_node *daynightnode;
    void InitDayNightCycle()
    {
      mat4t modelToWorld;
      mat4t modelToWorld_sun;
      mat4t modelToWorld_moon;
      mat4t modelToWorld_sky;
      mat4t modelToWorld_sunlight;

      modelToWorld_sunlight.loadIdentity();
      modelToWorld_sunlight.translate(4900, 0, 0);
      modelToWorld_sunlight.rotateY(-90);

      modelToWorld_sky.loadIdentity();
      modelToWorld.loadIdentity();
      modelToWorld_sun.loadIdentity();
      modelToWorld_sun.translate(5000, 0, 0);
      modelToWorld_moon.loadIdentity();
      modelToWorld_moon.translate(-5000, 0, 0);
      daynightnode = new scene_node(modelToWorld, atom_);
      app_scene->add_child(daynightnode);
      scene_node *sky_node = new scene_node(modelToWorld_sky, atom_);
      scene_node *sun_node = new scene_node(modelToWorld_sun, atom_);
      scene_node *moon_node = new scene_node(modelToWorld_moon, atom_);
      scene_node *sunlight_node = new scene_node(modelToWorld_sunlight, atom_);

      mesh_sphere *sky = new mesh_sphere(vec3(0, 0, 0), 5100);
      mesh_sphere *sun = new mesh_sphere(vec3(5000, 0, 0), 200);
      mesh_sphere *moon = new mesh_sphere(vec3(-5000, 0, 0), 200);
      /*
      SPOT LIGHT IS't WORKING
      light *lighttype = new light(atom_spot);
      lighttype->set_kind(atom_spot);
      lighttype->set_near_far(0.1,150);
      light_instance *sunlight = new light_instance(sunlight_node,lighttype);
      sunlight->set_light(lighttype);
      app_scene->add_light_instance(sunlight);
      sun_node->add_child(sunlight_node);
      */
      material *sun_mat = new material(vec4(0.992, 0.721, 0.074, 1));
      material *moon_mat = new material(vec4(0.266, 0.388, 0.956, 1));
      material *sky_mat = new material(vec4(0.247, 0.3039, 0.580, 1));
      app_scene->add_mesh_instance(new mesh_instance(sun_node, sun, sun_mat));
      app_scene->add_mesh_instance(new mesh_instance(moon_node, moon, moon_mat));
      app_scene->add_mesh_instance(new mesh_instance(sky_node, sky, sky_mat));
      daynightnode->add_child(sun_node);
      daynightnode->add_child(moon_node);
    }
    /** @fn  void InitDayNightCycle()
    *   @brief This function is used to init the day night cycle
    */
    void DayNightCycle()
    {
      daynightnode->access_nodeToParent().rotateZ(0.1);
      app_scene->get_light_instance(0)->get_node()->access_nodeToParent().rotateX(-0.1);
    }



    //The UI
    text_overlay *overlay;
    /** @var  text_overlay *overlay
    *   @brief text overlay for the UI.
    */
    mesh_text *UI_top;
    /** @var   mesh_text *UI_top
    *   @brief text mesh for the top UI.
    */
    mesh_text *UI_bot;
    /** @var   mesh_text *UI_bot
    *   @brief text mesh for the bottom UI.
    */
    mesh_text *UI_popup;
    /** @var  mesh_text *UI_popup
    *   @brief text mesh for the popup UI.
    */
    mesh_text *target;
    /** @var  mesh_text *target
    *   @brief text mesh for the target UI.
    */
    int popupmsg;
    /** @var int popupmsg
    *   @brief The msg number for the pop up UI
    */
    int tempframpopup;
    /** @var int tempframpopup
    *   @brief The frames lapsed since the popup UI was on
    */
    int framepopup;
    /** @var int framepopup
    *   @brief The number of frames the popup UI should show up
    */
    void initUI()
    {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      printf(" x %i , y %i", vx, vy);
      overlay = new text_overlay();
      // get the defualt font.
      bitmap_font *font = overlay->get_default_font();
      // create a box containing text (in pixels)
      aabb bb_bot(vec3(vx * 4 / 8, -550, 0.0f), vec3(vx, 200.0f, 0.0f));
      UI_bot = new mesh_text(font, "", &bb_bot);

      aabb bb_top(vec3(vx * 1 / 8, 200, 0.0f), vec3(vx, 200.0f, 0.0f));
      UI_top = new mesh_text(font, "", &bb_top);

      aabb bb_popup(vec3(vx * 7 / 8, -240, 0.0f), vec3(vx, 200.0f, 0.0f));
      UI_popup = new mesh_text(font, "", &bb_popup);

      aabb bb_target(vec3(vx, -100, 0.0f), vec3(vx, 100.0f, 0.0f));
      target = new mesh_text(font, "+", &bb_target);
      // add the mesh to the overlay.

      overlay->add_mesh_text(UI_bot);
      overlay->add_mesh_text(UI_top);
      overlay->add_mesh_text(UI_popup);
      overlay->add_mesh_text(target);
    }
    /** @fn  void initUI()
    *   @brief This function is used to init the UI variables ie overlay textmesh....
    */

    int pop_up_var = 0;
    bool clear_popup = false;
    float temp_popup_time;
    float total_popup_time;
    int terrain_type = 1;

    float ice_pert_to;
    float water_pert_to;
    float grass_pert_to;

    float ice_pert_from;
    float water_pert_from;
    float grass_pert_from;

    int selected_attrib = 0;
    bool use_colour_map = true;

    void setup_pop_up(int num)
    {
      total_popup_time = 2.0f;
      temp_popup_time = 0.0f;
      pop_up_var = num;
      clear_popup = true;
    }
    void pop_up_clear()
    {
      if (clear_popup)
      {
        if (total_popup_time - temp_popup_time <= 0)
        {
          clear_popup = false;
          pop_up_var = 0;
        }
        else
        {
          temp_popup_time += 0.03f;
        }
      }
    }
    void updateUI(int vx, int vy)
    {
      string current_element;
      switch (terrain_type)
      {
      case 0: current_element = "Random Terrain";
        break;

      case 1: current_element = "Example Heightmap";
        break;

      case 2: current_element = "Heightmap Based On Ken Perlin's Default Permutation Table";
        break;
      }
      UI_bot->clear();
      UI_bot->format(
        "Current Terrain Type : %s   Press G to regenerate the current Terrain", current_element);
      // convert it to a mesh.
      UI_bot->update();


      string current_attrib;
      switch (selected_attrib)
      {
      case 0: current_attrib = "ICE From";
        break;
      case 1:
        current_attrib = "ICE To";
        break;
      case 2: current_attrib = "Water From";
        break;
      case 3:current_attrib = "Water To";
        break;
      case 4: current_attrib = "Grass From";
        break;
      case 5:current_attrib = "Grass To";
        break;
      }
      UI_top->clear();
      UI_top->format(
        "Current Attrib : %s      Ice : From :%f To:%f  Water: From:%f To:%f  Grass: From:%f To:%f", current_attrib, ice_pert_from, ice_pert_to, water_pert_from, water_pert_to, grass_pert_from, grass_pert_to);
      // convert it to a mesh.
      UI_top->update();


      UI_popup->clear();
      switch (pop_up_var)
      {
      case 0:UI_popup->format("");
        break;
      case 1:UI_popup->format("Please wait...generating random terrain. This may take a while.");

        break;
      case 2:UI_popup->format("Generating an example heightmap.");

        break;
      case 3:UI_popup->format("Generating a heightmap based on Ken Perlin's default permutation table.");

        break;
      case 4:UI_popup->format("Terrain successfully generated.");
        break;
      }
      UI_popup->update();
      // draw the text overlay
      overlay->render(vx, vy);
    }
  };
}
#endif