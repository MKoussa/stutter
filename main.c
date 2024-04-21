#include "usermodfx.h"

static short int rate, depth;
static int32_t repeat;
static uint16_t sampleNum = 512;
static float _loopSamplesL[512], _loopSamplesR[512];

//Initialization process
void MODFX_INIT(uint32_t platform, uint32_t api)
{
  rate = 0;
  repeat = 0;
  for(size_t i = 0; i < sampleNum; i++)
  {
    _loopSamplesL[i] = 0;
    _loopSamplesR[i] = 0;
  }
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
          _loopSamplesL[rate] = main_xn[i * 2];
          _loopSamplesR[rate] = main_xn[i * 2 + 1];
      }
      main_yn[i * 2]     = _loopSamplesL[rate];
      main_yn[i * 2 + 1] = _loopSamplesR[rate];

      rate++;
      if(rate >= sampleNum)
      {
        rate = 0;
        repeat++;
      }
      if(repeat > depth)
      {
        repeat = 0;
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
      rate = (int)(valf * 511);
      break;
    //B Knob
    case k_user_modfx_param_depth:
      depth = (int)(valf * 50);
      break;
    default:
      break;
  }
}