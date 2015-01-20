////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//


namespace octet {
  /// Scene containing a box with octet.
  class inputs : public resource {
    // scene for drawing box
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    inputs() {
    }

    app *the_app;
    void init(app *app){
      this->the_app = app;
    }

    ///these keys are used for geenerating the terrain

    //pressing Q generates the terrain using a random seed 
    bool Q_KEY(){
      if (the_app->is_key_going_down('Q')){
        return true;
      }
      return false;
    }

    //Pressing R generates the terrain reading in from a .PPM file, therefore not reading in from random seed
    bool R_KEY(){
      if (the_app->is_key_going_down('R')){
        return true;
      }
      return false;
    }

    //pressing P generates the terrain using ken perlins pseudo-random permutation table 
    bool P_KEY(){
      if (the_app->is_key_going_down('P')){
        return true;
      }
      return false;
    }

    //misc. key presses such as camera movement and closing the program
    void key_presses(mat4t &camera){



      if (the_app->is_key_down(key_esc)){
        exit(0);
      }

      //camera movement keys (arrow keys and WASD)
      if (the_app->is_key_down(key::key_shift))
      {
        camera.translate(0, 5, 0);
      }
      if (the_app->is_key_down(key::key_ctrl))
      {
        camera.translate(0, -5, 0);
      }
      if (the_app->is_key_down(key::key_up) || the_app->is_key_down('W'))
      {
        camera.translate(0, 0, -5);
      }
      if (the_app->is_key_down(key::key_down) || the_app->is_key_down('S'))
      {
        camera.translate(0, 0, 5);
      }
      if (the_app->is_key_down(key::key_left) || the_app->is_key_down('A'))
      {
        camera.translate(-5, 0, 0);
      }
      if (the_app->is_key_down(key::key_right) || the_app->is_key_down('D'))
      {
        camera.translate(5, 0, 0);
      }
    }

    //mouse control using x and y pos of mouse
    void mouse_control(){

      int x, y;
      the_app->get_mouse_pos(x, y);
      int vx, vy;
      the_app->get_viewport_size(vx, vy);
      mat4t &camera_mat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
      mat4t modelToWorld;

      modelToWorld.loadIdentity();
      modelToWorld[3] = vec4(camera_mat.w().x(), camera_mat.w().y(), camera_mat.w().z(), 1);
      modelToWorld.rotateY((float)-x*2.0f);
      if (vy / 2 - y < 70 && vy / 2 - y > -70)
        modelToWorld.rotateX((float)vy / 2 - y);
      if (vy / 2 - y >= 70)
        modelToWorld.rotateX(70);
      if (vy / 2 - y <= -70)
        modelToWorld.rotateX(-70);
      camera_mat = modelToWorld;//apply to the node
    }


  };
}

