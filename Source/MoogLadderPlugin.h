//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// MoogLadderPlugin.h
//
// Code generation for function 'MoogLadderPlugin'
//

#ifndef MOOGLADDERPLUGIN_H
#define MOOGLADDERPLUGIN_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
struct MoogLadderPluginStackData;

// Type Definitions
class ParameterSmoother {
public:
  double step();
  ParameterSmoother();
  ~ParameterSmoother();
  int isInitialized;
  double smoothingFactor;
  double currentValue;
  double targetValue;
};

class derivedAudioPlugin {
public:
  derivedAudioPlugin();
  ~derivedAudioPlugin();
  boolean_T matlabCodegenIsDeleted;
  double PrivateSampleRate;
  int PrivateLatency;
  double Cutoff;
  double Resonance;
  double Gain;
  double tol;
  double smoothingRate;
  double kLimitingFactor;
  double fs;
  ParameterSmoother CutoffSmoother;
  ParameterSmoother ResonanceSmoother;
  ParameterSmoother GainSmoother;
  boolean_T bypass_flag;
  boolean_T ADAA_bypass_flag;
  double wnm1[8];
  double xnm1[8];
  double feedback[2];
  double drv_x_prev[2];
  double drv_F_prev[2];
  double fb_x_prev[2];
  double fb_F_prev[2];
};

// Function Declarations
extern void MoogLadderPlugin_initialize(MoogLadderPluginStackData *SD);

extern void MoogLadderPlugin_terminate(MoogLadderPluginStackData *SD);

extern void createPluginInstance(MoogLadderPluginStackData *SD,
                                 unsigned long long thisPtr);

extern int getLatencyInSamplesCImpl(MoogLadderPluginStackData *SD);

extern void onParamChangeCImpl(MoogLadderPluginStackData *SD, int paramIdx,
                               double b_value);

extern void processEntryPoint(MoogLadderPluginStackData *SD,
                              double samplesPerFrame, const double i1_data[],
                              const int i1_size[1], const double i2_data[],
                              const int i2_size[1], double o1_data[],
                              int o1_size[1], double o2_data[], int o2_size[1]);

extern void resetCImpl(MoogLadderPluginStackData *SD, double rate);

#endif
// End of code generation (MoogLadderPlugin.h)
