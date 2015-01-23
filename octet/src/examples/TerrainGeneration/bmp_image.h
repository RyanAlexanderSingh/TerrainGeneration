////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// note source
//

#ifndef BMP_IMAGE_H_INCLUDED
#define BMP_IMAGE_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

using namespace std;

namespace octet {

#define width_image 1024
#define height_image 1024

  /// Scene containing a box with octet.
  class bmp_image : public resource {
    ref<visual_scene> app_scene;

  public:
    /// this is called when we construct the class before everything is initialised.
    bmp_image() {
    }

    int width = 0, height = 0, size = 0, max_col_val = 255;
    std::vector<uint8_t> r;
    std::vector<uint8_t> g;
    std::vector<uint8_t> b;

    unsigned int nr_lines, nr_columns;

    void init(){
      width = width_image;
      height = height_image;
      nr_lines = height;
      nr_columns = width;
      size = width*height;

      r.resize(size);
      g.resize(size);
      b.resize(size);
    }

    //colour structre for storing rgb values of each pixel
    struct colour{
      unsigned char v[3];

      colour(unsigned char r, unsigned char g, unsigned char b){
        v[0] = r;
        v[1] = g;
        v[2] = b;
      }
    };

    //const varibles for height_image and width_image of bmp image


    colour lerp(colour c1, colour c2, float value){
      colour tcolor(0, 0, 0);

      for (int g = 0; g < 3; g++){
        if (c1.v[g] > c2.v[g]){
          tcolor.v[g] = c2.v[g] + (unsigned char)(float(c1.v[g] - c2.v[g])*value);
        }
        else{
          tcolor.v[g] = c1.v[g] + (unsigned char)(float(c2.v[g] - c1.v[g])*value);
        }
      }
      return (tcolor);
    }

    //creates a file with a given name and writes to that text file with the generated perlin noise
    void write_greyscale_image(float *image, float &_min, float &_max){

      //set up some variables
      float diff = _max - _min,
        flood = 0.5f,//flood level
        rock = 0.7f, //rock level
        mount = 0.85f;//mountain level

      flood *= diff;
      mount *= diff;
      rock *= diff;

      int i, j, k;

      ////these can be changed for interesting results
      colour rocklow(0, 76, 153),
        rockhigh(0, 25, 51),
        grasslow(0, 64, 0),
        grasshigh(116, 182, 133),
        waterlow(55, 0, 0), //done
        waterhigh(106, 53, 0), //done
        mountlow(147, 157, 167), //done
        mounthigh(212, 240, 255); //done

      //creat a new stream and open the file
      //start writing to the file
      ofstream s;
      s.open("perlin_noise.bmp", ofstream::binary);
      if (!(s.is_open())){
        printf("Cannot open the file...");
        exit(0);
      }

      ofstream grey;
      grey.open("perlin_noise_grey.bmp", ofstream::binary);
      if (!(grey.is_open())){
        printf("Cannot open the file...");
        exit(0);
      }

      s.put(char(66));
      s.put(char(77));

      grey.put(char(66));
      grey.put(char(77));

      for (i = 0; i < 8; i++){
        s.put(char(0));
      }
      s.put(char(54));

      for (i = 0; i < 3; i++){
        s.put(char(0));
      }

      for (i = 0; i < 8; i++){
        grey.put(char(0));
      }
      grey.put(char(54));

      for (i = 0; i < 3; i++){
        grey.put(char(0));
      }

      s.put(char(40));

      grey.put(char(40));

      //white space - ignore this place
      for (i = 0; i < 3; i++){
        s.put(char(0));
      }
      s.put(char(height_image % 256));
      s.put(char((height_image >> 8) % 256));
      s.put(char((height_image >> 16) % 256));
      s.put(char((height_image >> 24) % 256));

      s.put(char(width_image % 256));
      s.put(char((width_image >> 8) % 256));
      s.put(char((width_image >> 16) % 256));
      s.put(char((width_image >> 24) % 256));

      s.put(char(1));
      s.put(char(0));

      s.put(char(24));

      for (i = 0; i < 25; i++){
        s.put(char(0));
      }

      //white space - ignore this place
      for (i = 0; i < 3; i++){
        grey.put(char(0));
      }
      grey.put(char(height_image % 256));
      grey.put(char((height_image >> 8) % 256));
      grey.put(char((height_image >> 16) % 256));
      grey.put(char((height_image >> 24) % 256));

      grey.put(char(width_image % 256));
      grey.put(char((width_image >> 8) % 256));
      grey.put(char((width_image >> 16) % 256));
      grey.put(char((width_image >> 24) % 256));

      grey.put(char(1));
      grey.put(char(0));

      grey.put(char(24));

      for (i = 0; i < 25; i++){
        grey.put(char(0));
      }

      colour pxl_colour(0, 0, 0);
      colour greyscale_colour(0, 0, 0);
      colour black(0, 0, 0);
      colour white(254, 254, 254);
      for (i = (width_image - 1); i >= 0; i--){
        for (j = 0; j < height_image; j++){

          *(image + i*width_image + j) -= _min;
          //if this point is below the floodline...
          if (*(image + i*width_image + j) < flood)
          {
            pxl_colour = lerp(waterlow, waterhigh, *(image + i*width_image + j) / flood);
          }//if this is above the mountain line...
          else if (*(image + i*width_image + j) > mount)
            pxl_colour = lerp(mountlow, mounthigh, (*(image + i*width_image + j) - mount) / (diff - mount));
          //f this is above the rock line
          else if (*(image + i*width_image + j)>rock)
            pxl_colour = lerp(rocklow, rockhigh, (*(image + i*width_image + j) - rock) / (diff - flood));
          //if this is regular land
          else
            pxl_colour = lerp(grasslow, grasshigh, (*(image + i*width_image + j) - flood) / (mount - flood));

          //greyscale_colour = lerp(black, white, *(image + i*width_image + j));
          //pxl_colour.v[0] = *(image + i*width_image + j) * 255.0f;


          s.put(char(pxl_colour.v[0]));
          s.put(char(pxl_colour.v[1]));
          s.put(char(pxl_colour.v[2]));

          grey.put(char(greyscale_colour.v[0]));
          grey.put(char(greyscale_colour.v[0]));
          grey.put(char(greyscale_colour.v[0]));
        }
        //null terminate the buffer
        for (k = 0; k < (height_image % 4); k++)
          s.put(char(0));
      }

      for (k = 0; k < (height_image % 4); k++)
        grey.put(char(0));

      //end the file (close it)
      s.close();
      grey.close();
    }


    void write(float *image, float &_min, float &_max){
      std::string fname = "perlin_noise.ppm";
      std::ofstream input(fname.c_str(), std::ios::out | std::ios::binary);
      //set up some variables
      float diff = _max - _min,
        flood = 0.5f,//flood level
        rock = 0.7f, //rock level
        mount = 0.85f;//mountain level

      flood *= diff;
      mount *= diff;
      rock *= diff;

      int i, j, k;

      ////these can be changed for interesting results
      colour rocklow(0, 76, 153),
        rockhigh(0, 25, 51),
        grasslow(0, 64, 0),
        grasshigh(116, 182, 133),
        waterlow(55, 0, 0), //done
        waterhigh(106, 53, 0), //done
        mountlow(147, 157, 167), //done
        mounthigh(212, 240, 255); //done
      colour pxl_colour(0, 0, 0);
      colour greyscale_colour(0, 0, 0);
      colour black(0, 0, 0);
      colour white(254, 254, 254);

      
      if (input.is_open()){

        input << "P6\n";
        input << width;
        input << " ";
        input << height << "\n";
        input << max_col_val << "\n";

          
        char aux;
        for (int i = 0; i < size; ++i){


         // *(image + i) -= _min;
          //if this point is below the floodline...
          if (*(image + i) < flood)
          {
            pxl_colour = lerp(waterlow, waterhigh, *(image + i) / flood);
          }//if this is above the mountain line...
          else if (*(image + i) > mount)
            pxl_colour = lerp(mountlow, mounthigh, (*(image + i) - mount) / (diff - mount));
          //f this is above the rock line
          else if (*(image + i)>rock)
            pxl_colour = lerp(rocklow, rockhigh, (*(image + i) - rock) / (diff - flood));
          //if this is regular land
          else
            pxl_colour = lerp(grasslow, grasshigh, (*(image + i) - flood) / (mount - flood));


          //run for loop three times for the R, G, B of each pixel
            aux = (char)pxl_colour.v[0];
            input.write(&aux, 1);
            aux = (char)pxl_colour.v[1];
            input.write(&aux, 1);
            aux = (char)pxl_colour.v[2];
            input.write(&aux, 1);
        }
      }

      else {
        printf("Unable to open file: %s", fname);
      }
      input.close();
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
          printf("%i, %i", height, width);
        }
        catch (std::exception &e) {
          std::cout << "Header file format error. " << e.what() << std::endl;
          return;
        }

        std::getline(input, line);
        std::stringstream max_val(line);
        try {
          max_val >> max_col_val;
        }
        catch (std::exception &e) {
          std::cout << "Header file format error. " << e.what() << std::endl;
          return;
        }

        int size = width*height;

        r.resize(size);
        r.reserve(size);
        g.resize(size);
        g.reserve(size);
        b.resize(size);
        b.reserve(size);

        char aux;
        for (int i = 0; i < size; ++i) {
          input.read(&aux, 1);
          r[i] = (unsigned char)aux;
          input.read(&aux, 1);
          g[i] = (unsigned char)aux;
          input.read(&aux, 1);
          b[i] = (unsigned char)aux;
        }
      }
      else {
        printf("Error, unable to oepn %s", fname);
      }
      input.close();
    }
  };
}
#endif
