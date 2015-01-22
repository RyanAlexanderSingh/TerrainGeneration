////////////////////////////////////////////////////////////////////////////////
//
// (C) Ryan Singh and Himanshu Chablani
//
// Parts of this class are a direct translation to C++11 from Ken Perlin's improved perlin function (http://mrl.nyu.edu/~perlin/noise/)
//
#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

#include <random>
#include <numeric>
#include <ctime>

namespace octet {
  /// Scene containing a box with octet.
  class perlin : public resource {
    // scene for drawing box
    ref<visual_scene> app_scene;

  public:
    /// this is called when we construct the class before everything is initialised.
    perlin() {
    }

    static uint8_t permutation[256];

    float fade(float t){
      return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float lerp(float t, float a, float b){
      //return a + t * (b - a);
      return (t + b * a * (1 - b)); //check this 
    }

    float dotproduct(float grad[], float x, float y){
      return ((grad[0] * x) + (grad[1] * y)); //check this
    }

    int floor(float x){
      return (x >= 0 ? int(x) : int(x) - 1);
    }
      
    float grad(int hash, float x, float y, float z){
      int h = hash & 15;
      float u = h < 8 ? x : y,
        v = h < 4 ? y : h == 12 || h == 14 ? x : z;
      return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    //function responsible for generating the perlin noise
    float noise(float x, float y, float z) {

      // Find the unit cube that contains the point
      int X = (int)floor(x) & 255;
      int Y = (int)floor(y) & 255;
      int Z = (int)floor(z) & 255;

      // Find relative x, y,z of point in cube
      x -= floor(x);
      y -= floor(y);
      z -= floor(z);

      // Compute fade curves for each of x, y, z
      float u = fade(x);
      float v = fade(y);
      float w = fade(z);

      // Hash coordinates of the 8 cube corners
      int A = permutation[X] + Y;
      int AA = permutation[A] + Z;
      int AB = permutation[A + 1] + Z;
      int B = permutation[X + 1] + Y;
      int BA = permutation[B] + Z;
      int BB = permutation[B + 1] + Z;

      // Add blended results from 8 corners of cube
      float res = lerp(w, lerp(v, lerp(u, grad(permutation[AA], x, y, z), grad(permutation[BA], x - 1, y, z)), lerp(u, grad(permutation[AB], x, y - 1, z), grad(permutation[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(permutation[AA + 1], x, y, z - 1), grad(permutation[BA + 1], x - 1, y, z - 1)), lerp(u, grad(permutation[AB + 1], x, y - 1, z - 1), grad(permutation[BB + 1], x - 1, y - 1, z - 1))));
      return (res + 1.0f) / 2.0f;
    }

    float random(float max){
      int r;
      float s;

      r = rand();
      s = (float)(r & 0x7fff) / (float)0x7fff;

      return (s * max);
    }


    //generates the wholer perlin noise given a certain number of octaves
    float generate_noise(float x, float y, float z, int _octaves, bool random_table){

      //generate random permutation table for perlin noise
      if (random_table){

        for (int i = 0; i < 256; ++i){
          permutation[i] = i;//put each number in once
        }
        //randomize the random numbers table
        for (int i = 0; i < 256; ++i)
        {
          int j = (int)random(256);
          int k = permutation[i];
          permutation[i] = permutation[j];
          permutation[j] = k;
        }
      }


      float result = 0.0f;
      float gain = 0.65f;
      float amp = 1.0f;
      float frequency = 1.0f / 500.0f;
      float lacunarity = 2.0f;
      
      int i = _octaves;
      while (i--){
        result += noise(x * frequency, y * frequency, z * frequency) * amp;
        amp *= gain;
        frequency *= lacunarity;
      }
      printf("%f", result);
      return result;
    }
  };

  uint8_t perlin::permutation[] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
    35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
    55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
    18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
    250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
    189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
    172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
    107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };
}

#endif