////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Main source is based off PPM.h by Sol of Solarian Programmer (https://solarianprogrammer.com/2011/12/16/cpp-11-thread-tutorial/)
//

#include <fstream>

namespace octet {
  /// Scene containing a box with octet.
  class ppm : public resource {
    // scene for drawing box
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    ppm() {
    }

    int width = 0, height = 0, size = 0, max_col_val = 255;
    std::vector<unsigned char> pixel_colour;

    //check if we need
    unsigned int nr_lines, nr_columns;

    void init(int _height, int _width){
      width = _width;
      height = _height;
      nr_lines = height;
      nr_columns = width;
      size = width*height;

      pixel_colour.resize(size);
    }

    //creates a file with a given name and writes to that text file with the generated perlin noise
    void write(std::string fname){
      std::ofstream input(fname.c_str(), std::ios::out | std::ios::binary);
      if (input.is_open()){

        input << "P6\n";
        input << width;
        input << " ";
        input << height << "\n";
        input << max_col_val << "\n";

        char aux;
        for (int i = 0; i < size; ++i){
          //run for loop three times for the R, G, B of each pixel
          for (int j = 0; j < 3; ++j){
            aux = (char)pixel_colour[i];
            input.write(&aux, 1);
          }
        }
      }
      else {
        printf("Unable to open file: %s", fname);
      }
      input.close();
    }
  };
}

