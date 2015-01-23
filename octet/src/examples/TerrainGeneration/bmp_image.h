////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Main source is based off PPM.h by Sol of Solarian Programmer (https://solarianprogrammer.com/2011/12/16/cpp-11-thread-tutorial/)
//

#ifndef BMP_IMAGE_H_INCLUDED
#define BMP_IMAGE_H_INCLUDED

#include <iostream>
#include <fstream>

using namespace std;

namespace octet {
  /// Scene containing a box with octet.
  class bmp_image : public resource {
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    bmp_image() {
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

    //const varibles for height and width of bmp image
    static const unsigned height = 500, width = 500;

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
    void write(float image[][500], float &_min, float &_max){

      //set up some variables
      float diff = _max - _min,
        flood = 0.5f,//flood level
        mount = 0.85f;//mountain level

      flood *= diff;
      mount *= diff;

      int i, j, k;

      //these can be changed for interesting results
      colour landlow(0, 64, 0),
        landhigh(116, 182, 133),
        waterlow(55, 0, 0),
        waterhigh(106, 53, 0),
        mountlow(147, 157, 167),
        mounthigh(226, 223, 216);

      //creat a new stream and open the file
      //start writing to the file
      ofstream s;
      s.open("perlin_noise.bmp", ofstream::binary);
      if (!(s.is_open())){
        printf("Cannot open the file...");
        exit(0);
      }

      s.put(char(66));
      s.put(char(77));

      for (i = 0; i < 8; i++){
        s.put(char(0));
      }
      s.put(char(54));

      for (i = 0; i < 3; i++){
        s.put(char(0));
      }

      s.put(char(40));

      //white space - ignore this place
      for (i = 0; i < 3; i++){
        s.put(char(0));
      }
      s.put(char(height % 256));
      s.put(char((height >> 8) % 256));
      s.put(char((height >> 16) % 256));
      s.put(char((height >> 24) % 256));

      s.put(char(width % 256));
      s.put(char((width >> 8) % 256));
      s.put(char((width >> 16) % 256));
      s.put(char((width >> 24) % 256));

      s.put(char(1));
      s.put(char(0));

      s.put(char(24));

      for (i = 0; i < 25; i++){
        s.put(char(0));
      }

      colour newcolor(0, 0, 0);
      for (i = (width - 1); i >= 0; i--){
        for (j = 0; j < height; j++){
          image[j][i] -= _min;
          //if this point is below the floodline...
          if (image[j][i]<flood)
            newcolor = lerp(waterlow, waterhigh, image[j][i] / flood);

          //if this is above the mountain line...
          else if (image[j][i]>mount)
            newcolor = lerp(mountlow, mounthigh, (image[j][i] - mount) / (diff - mount));

          //if this is regular land
          else
            newcolor = lerp(landlow, landhigh, (image[j][i] - flood) / (mount - flood));

          s.put(char(newcolor.v[0]));
          s.put(char(newcolor.v[1]));
          s.put(char(newcolor.v[2]));
        }
        //null terminate the buffer
        for (k = 0; k < (height % 4); k++)
          s.put(char(0));
      }

      //end the file (close it)
      s.close();
    }
  };
}
#endif
