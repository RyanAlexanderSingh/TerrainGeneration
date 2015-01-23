////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Main source is based off PPM.h by Sol of Solarian Programmer (https://solarianprogrammer.com/2011/12/16/cpp-11-thread-tutorial/)
//
#ifndef BMP_IMAGE_H_INCLUDED
#define BMP_IMAGE_H_INCLUDED

#include <fstream>
#include <exception>
#include <sstream>

namespace octet {
  /// Scene containing a box with octet.
  class bmp_image : public resource {
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    bmp_image() {
    }

    //read a .ppm file heightmap
    void read(std::string fname){

      std::ifstream input(fname.c_str(), std::ios::in | std::ios::binary);
      if (input.is_open()){
        std::string line;
        std::getline(input, line);
        if (line != "P6"){
          printf("Error! Unrecognized file format..Could not find 'P6' in the file.");
          return;
        }
        std::getline(input, line);
        while (line[0] == '#') {
          std::getline(input, line);
        }
        std::stringstream dimensions(line);

        try {
          dimensions >> width;
          dimensions >> height;
          nr_lines = height;
          nr_columns = width;
        }
        catch (std::exception &e) {
          printf("Header file format error. \n", e.what());
          return;
        }

        std::getline(input, line);
        std::stringstream max_val(line);
        try {
          max_val >> max_col_val;
        }
        catch (std::exception &e) {
          printf("Header file format error. \n", e.what());
          return;
        }

        size = width*height;

        pixel_colour.resize(size);
        pixel_colour.reserve(size);

        char aux;
        for (int i = 0; i < size; ++i) {
          for (int j = 0; j < 3; ++j){
            input.read(&aux, 1);
            //the RGB colour values are stored together so we'll just take the last one (they're all the same)
            if (j == 2){
              pixel_colour[i] = (unsigned char)aux;
            }
          }
        }
      }
      else {
        printf("Error, unable to open %s", fname);
      }
      input.close();
    }


    //creates a file with a given name and writes to that text file with the generated perlin noise
    void write(std::string fname){

      const unsigned height = 650, width = 650;
      float image[height][width];
      
      std::ofstream input(fname.c_str(), std::ios::out | std::ios::binary);
      if (input.is_open()){

        //P6 complies with .ppm file formats
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
#endif
