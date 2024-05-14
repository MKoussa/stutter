#include "usermodfx.h"
#include "buffer_ops.h"
#include <atomic>

#define BUFFER_LEN 1024

static uint16_t rate, depth, repeat;
static bool rateChange, depthChange;
static uint16_t rateLength;

static __sdram float stutterSample[BUFFER_LEN];
static std::atomic<uint16_t> rateVal(0);
static std::atomic<uint16_t> depthVal(0);


//Initialization process
void MODFX_INIT(uint32_t platform, uint32_t api)
{
  buf_clr_f32(stutterSample, BUFFER_LEN);
  rate = 0;
  depth = 0;
  repeat = 0;
  rateLength = 0;

  rateChange = false;
  depthChange = false;

  rateVal = 0;
  depthVal = 0;
}

/*
    Goal: Play a chunk of audio, its length defined by Rate, for a number of times defined by Depth.
    Rate: set how long the loops are
    Depth: how many loops until reset
*/
void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames)
{
    for(uint32_t i = 0; i < frames; i++)
    {
      if(repeat == 0)
      {
        stutterSample[rate * 2]     = main_xn[i * 2];
        stutterSample[rate * 2 + 1] = main_xn[i * 2 + 1];
      }
      main_yn[i * 2]     = stutterSample[rate * 2];
      main_yn[i * 2 + 1] = stutterSample[rate * 2 + 1];

      rate++;
      if(rate > rateLength)
      {
        rate = 0;
        repeat++;
        if(rateChange)
        {
            rateLength = rateVal;
            rateChange = false;
        }
        if(depthChange)
        {
            depth = depthVal;
            depthChange = false;
        }
        if(repeat > depth)
        {
            repeat = 0;
        }
      }
    }
}

void MODFX_PARAM(uint8_t index, int32_t value)
{
  //Convert fixed point q31 format to float
  const float valf = q31_to_f32(value);
  switch (index)
  {
    //A knob
    case k_user_modfx_param_time:
      rateVal = (uint16_t)(valf * 1023);
      rateChange = true;
      break;
    //B Knob
    case k_user_modfx_param_depth:
      depthVal = (uint16_t)(valf * 50);
      depthChange = true;
      break;
    default:
      break;
  }
}