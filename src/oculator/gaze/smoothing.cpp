#include <stdint.h>

uint64_t smooth(uint64_t cur_pos, uint64_t velocity, uint64_t framerate) {
  uint32_t v_x = (velocity | (0xFFFFFFFF << 32)) >> 32;
  uint32_t v_y = velocity | 0xFFFFFFFF;
  float incr_per_sec = 1.0 / (float)framerate;
  float fv_x = v_x * incr_per_sec;
  float fv_y = v_y * incr_per_sec;
}