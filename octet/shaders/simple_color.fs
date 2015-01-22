//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs
varying vec4 color_;
varying vec2 uv_;
uniform sampler2D diffuse_sampler;

void main() {
vec4 diffuse = texture2D(diffuse_sampler, uv_);
  gl_FragColor =  color_;
}

