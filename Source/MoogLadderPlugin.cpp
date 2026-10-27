//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// MoogLadderPlugin.cpp
//
// Code generation for function 'MoogLadderPlugin'
//

// Include files
#include "MoogLadderPlugin.h"
#include "MoogLadderPlugin_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// Function Declarations
namespace coder {
static double mpower(double b);

}
static derivedAudioPlugin *getPluginInstance(MoogLadderPluginStackData *SD);

static void getPluginInstance_delete(MoogLadderPluginStackData *SD);

static void getPluginInstance_init(MoogLadderPluginStackData *SD);

static void getPluginInstance_new(MoogLadderPluginStackData *SD);

static double onsetK();


// Function Definitions
double ParameterSmoother::step()
{
  double varargout_1;
  if (isInitialized != 1) {
    isInitialized = 1;
  }
  //  Perform the smoothing
  varargout_1 =
      smoothingFactor * currentValue + (1.0 - smoothingFactor) * targetValue;
  currentValue = varargout_1;
  return varargout_1;
}

namespace coder {
static double mpower(double b)
{
  double c;
  if (std::isnan(b)) {
    c = rtNaN;
  } else {
    c = std::abs(b);
    if (std::isinf(b)) {
      if (b > 0.0) {
        c = rtInf;
      } else {
        c = 0.0;
      }
    } else if (c == 0.0) {
      c = 1.0;
    } else if (c == 1.0) {
      if (b > 0.0) {
        c = 10.0;
      } else {
        c = 0.1;
      }
    } else if (b == 2.0) {
      c = 100.0;
    } else if (b == 0.5) {
      c = 3.1622776601683795;
    } else {
      c = std::pow(10.0, b);
    }
  }
  return c;
}

} // namespace coder
static derivedAudioPlugin *getPluginInstance(MoogLadderPluginStackData *SD)
{
  if (!SD->pd->plugin_not_empty) {
    double initialValue;
    double smoothingFactor;
    //  Pass constructor args to plugin.
    SD->pd->plugin.Cutoff = 2000.0;
    SD->pd->plugin.Resonance = 0.0;
    SD->pd->plugin.Gain = 0.0;
    SD->pd->plugin.tol = 1.0E-6;
    SD->pd->plugin.smoothingRate = 0.5;
    SD->pd->plugin.kLimitingFactor = 0.5;
    SD->pd->plugin.fs = 44100.0;
    SD->pd->plugin.bypass_flag = false;
    SD->pd->plugin.ADAA_bypass_flag = false;
    std::memset(&SD->pd->plugin.wnm1[0], 0, 8U * sizeof(double));
    std::memset(&SD->pd->plugin.xnm1[0], 0, 8U * sizeof(double));
    SD->pd->plugin.feedback[0] = 0.0;
    SD->pd->plugin.drv_x_prev[0] = 0.0;
    SD->pd->plugin.drv_F_prev[0] = 0.0;
    SD->pd->plugin.fb_x_prev[0] = 0.0;
    SD->pd->plugin.fb_F_prev[0] = 0.0;
    SD->pd->plugin.feedback[1] = 0.0;
    SD->pd->plugin.drv_x_prev[1] = 0.0;
    SD->pd->plugin.drv_F_prev[1] = 0.0;
    SD->pd->plugin.fb_x_prev[1] = 0.0;
    SD->pd->plugin.fb_F_prev[1] = 0.0;
    SD->pd->plugin.PrivateLatency = 0;
    initialValue = SD->pd->plugin.Cutoff;
    smoothingFactor = SD->pd->plugin.smoothingRate;
    SD->pd->plugin.CutoffSmoother.isInitialized = 0;
    SD->pd->plugin.CutoffSmoother.currentValue = initialValue;
    SD->pd->plugin.CutoffSmoother.targetValue = initialValue;
    SD->pd->plugin.CutoffSmoother.smoothingFactor = smoothingFactor;
    initialValue = SD->pd->plugin.Resonance;
    smoothingFactor = SD->pd->plugin.smoothingRate;
    SD->pd->plugin.ResonanceSmoother.isInitialized = 0;
    SD->pd->plugin.ResonanceSmoother.currentValue = initialValue;
    SD->pd->plugin.ResonanceSmoother.targetValue = initialValue;
    SD->pd->plugin.ResonanceSmoother.smoothingFactor = smoothingFactor;
    initialValue = SD->pd->plugin.Gain;
    smoothingFactor = SD->pd->plugin.smoothingRate;
    SD->pd->plugin.GainSmoother.isInitialized = 0;
    SD->pd->plugin.GainSmoother.currentValue = initialValue;
    SD->pd->plugin.GainSmoother.targetValue = initialValue;
    SD->pd->plugin.GainSmoother.smoothingFactor = smoothingFactor;
    SD->pd->plugin.matlabCodegenIsDeleted = false;
    SD->pd->plugin_not_empty = true;
  }
  return &SD->pd->plugin;
}

static void getPluginInstance_delete(MoogLadderPluginStackData *SD)
{
  if (!SD->pd->plugin.matlabCodegenIsDeleted) {
    SD->pd->plugin.matlabCodegenIsDeleted = true;
  }
}

static void getPluginInstance_init(MoogLadderPluginStackData *SD)
{
  SD->pd->plugin_not_empty = false;
}

static void getPluginInstance_new(MoogLadderPluginStackData *SD)
{
  SD->pd->plugin.matlabCodegenIsDeleted = true;
}

ParameterSmoother::ParameterSmoother() = default;

ParameterSmoother::~ParameterSmoother() = default;

derivedAudioPlugin::derivedAudioPlugin() = default;

derivedAudioPlugin::~derivedAudioPlugin() = default;

void MoogLadderPlugin_initialize(MoogLadderPluginStackData *SD)
{
  getPluginInstance_new(SD);
  getPluginInstance_init(SD);
}

void MoogLadderPlugin_terminate(MoogLadderPluginStackData *SD)
{
  getPluginInstance_delete(SD);
}

void createPluginInstance(MoogLadderPluginStackData *SD, unsigned long long)
{
  getPluginInstance(SD);
}

int getLatencyInSamplesCImpl(MoogLadderPluginStackData *SD)
{
  derivedAudioPlugin *plugin;
  plugin = getPluginInstance(SD);
  return plugin->PrivateLatency;
}

void onParamChangeCImpl(MoogLadderPluginStackData *SD, int paramIdx,
                        double b_value)
{
  derivedAudioPlugin *plugin;
  plugin = getPluginInstance(SD);
  switch (paramIdx) {
  case 0:
    plugin->Cutoff = b_value;
    plugin->CutoffSmoother.targetValue = b_value;
    break;
  case 1:
    plugin->Resonance = b_value;
    plugin->ResonanceSmoother.targetValue = b_value;
    break;
  case 2:
    plugin->Gain = b_value;
    plugin->GainSmoother.targetValue = b_value;
    break;
  case 3:
    plugin->bypass_flag = (b_value == 0.0);
    break;
  case 4:
    plugin->ADAA_bypass_flag = (b_value == 0.0);
    plugin->drv_x_prev[0] = 0.0;
    plugin->drv_F_prev[0] = 0.0;
    plugin->fb_x_prev[0] = 0.0;
    plugin->fb_F_prev[0] = 0.0;
    plugin->drv_x_prev[1] = 0.0;
    plugin->drv_F_prev[1] = 0.0;
    plugin->fb_x_prev[1] = 0.0;
    plugin->fb_F_prev[1] = 0.0;
    break;
  }
}

void processEntryPoint(MoogLadderPluginStackData *SD, double samplesPerFrame,
                       const double i1_data[], const int i1_size[1],
                       const double i2_data[], const int i2_size[1],
                       double o1_data[], int o1_size[1], double o2_data[],
                       int o2_size[1])
{
  derivedAudioPlugin *plugin;
  coder::array<double, 2U> out_wet;
  coder::array<double, 2U> t1;
  double wnm1_buffer[8];
  double xnm1_buffer[8];
  double drv_F_prev_buffer[2];
  double drv_x_prev_buffer[2];
  double fb_F_prev_buffer[2];
  double fb_x_prev_buffer[2];
  double feedback_buffer[2];
  double den1;
  double df;
  double drive_out;
  double fc;
  double fs_buffer;
  double gain;
  double k;
  double kLimit;
  double num0;
  double tol_buffer;
  int b_loop_ub;
  int loop_ub;
  boolean_T bypass_ADAA_buffer;
  boolean_T bypass_plugin_buffer;
  plugin = getPluginInstance(SD);
  loop_ub = i1_size[0];
  t1.set_size(i1_size[0], 2);
  if (loop_ub - 1 >= 0) {
    std::copy(&i1_data[0], &i1_data[loop_ub], &t1[0]);
  }
  b_loop_ub = i2_size[0];
  for (int n{0}; n < b_loop_ub; n++) {
    t1[n + t1.size(0)] = i2_data[n];
  }
  //  Initialize key properties and per buffer state variables
  fs_buffer = plugin->fs;
  fc = plugin->CutoffSmoother.step();
  drive_out = plugin->ResonanceSmoother.step();
  kLimit = plugin->kLimitingFactor;
  gain = coder::mpower(plugin->GainSmoother.step() / 20.0);
  for (int n{0}; n < 8; n++) {
    wnm1_buffer[n] = plugin->wnm1[n];
    xnm1_buffer[n] = plugin->xnm1[n];
  }
  feedback_buffer[0] = plugin->feedback[0];
  drv_x_prev_buffer[0] = plugin->drv_x_prev[0];
  drv_F_prev_buffer[0] = plugin->drv_F_prev[0];
  fb_x_prev_buffer[0] = plugin->fb_x_prev[0];
  fb_F_prev_buffer[0] = plugin->fb_F_prev[0];
  feedback_buffer[1] = plugin->feedback[1];
  drv_x_prev_buffer[1] = plugin->drv_x_prev[1];
  drv_F_prev_buffer[1] = plugin->drv_F_prev[1];
  fb_x_prev_buffer[1] = plugin->fb_x_prev[1];
  fb_F_prev_buffer[1] = plugin->fb_F_prev[1];
  tol_buffer = plugin->tol;
  //  moog_coeffs function + frequency-dependent k-limiting
  df = fs_buffer * 0.45;
  fc = std::fmin(fc, df);
  k = std::fmin(drive_out, 3.99 * (1.0 - kLimit * (fc / df)));
  fc = std::tan(3.1415926535897931 * fc / fs_buffer);
  num0 = fc / (fc + 1.0);
  den1 = (fc - 1.0) / (fc + 1.0);
  //  Define output and bypass checks
  out_wet.set_size(i1_size[0], 2);
  bypass_ADAA_buffer = plugin->ADAA_bypass_flag;
  bypass_plugin_buffer = plugin->bypass_flag;
  //  Buffer loop begins
  for (int n{0}; n < loop_ub; n++) {
    for (int ch{0}; ch < 2; ch++) {
      //  Drive stage
      fs_buffer = gain * t1[n + t1.size(0) * ch];
      //  Gain check (no drive saturation unless + gain)
      if (gain <= 1.0) {
        drive_out = fs_buffer;
        //  ADAA check (drive stage)
      } else if (bypass_ADAA_buffer) {
        drive_out = std::tanh(fs_buffer);
      } else {
        kLimit = drv_x_prev_buffer[ch];
        fc = fs_buffer - kLimit;
        df = std::log(std::cosh(fs_buffer));
        //  Tolerance check
        if (std::abs(fc) < tol_buffer) {
          drive_out = std::tanh(0.5 * (fs_buffer + kLimit));
        } else {
          drive_out = (df - drv_F_prev_buffer[ch]) / fc;
        }
        drv_F_prev_buffer[ch] = df;
      }
      drv_x_prev_buffer[ch] = fs_buffer;
      //  Ladder Filter
      fc = drive_out - k * feedback_buffer[ch];
      b_loop_ub = ch << 2;
      drive_out = (num0 * fc + num0 * xnm1_buffer[b_loop_ub]) -
                  den1 * wnm1_buffer[b_loop_ub];
      xnm1_buffer[b_loop_ub] = fc;
      wnm1_buffer[b_loop_ub] = drive_out;
      fc = drive_out;
      drive_out = (num0 * drive_out + num0 * xnm1_buffer[b_loop_ub + 1]) -
                  den1 * wnm1_buffer[b_loop_ub + 1];
      xnm1_buffer[b_loop_ub + 1] = fc;
      wnm1_buffer[b_loop_ub + 1] = drive_out;
      fc = drive_out;
      drive_out = (num0 * drive_out + num0 * xnm1_buffer[b_loop_ub + 2]) -
                  den1 * wnm1_buffer[b_loop_ub + 2];
      xnm1_buffer[b_loop_ub + 2] = fc;
      wnm1_buffer[b_loop_ub + 2] = drive_out;
      fc = drive_out;
      drive_out = (num0 * drive_out + num0 * xnm1_buffer[b_loop_ub + 3]) -
                  den1 * wnm1_buffer[b_loop_ub + 3];
      xnm1_buffer[b_loop_ub + 3] = fc;
      wnm1_buffer[b_loop_ub + 3] = drive_out;
      //  out_wet(n, ch) = output;
      //  ADAA check (feedback stage)
      if (bypass_ADAA_buffer) {
        kLimit = std::tanh(drive_out);
      } else {
        kLimit = fb_x_prev_buffer[ch];
        df = drive_out - kLimit;
        fc = std::log(std::cosh(drive_out));
        //  Tolerance check
        if (std::abs(df) < tol_buffer) {
          kLimit = std::tanh(0.5 * (drive_out + kLimit));
        } else {
          kLimit = (fc - fb_F_prev_buffer[ch]) / df;
        }
        fb_F_prev_buffer[ch] = fc;
      }
      fb_x_prev_buffer[ch] = drive_out;
      feedback_buffer[ch] = kLimit;
      out_wet[n + out_wet.size(0) * ch] = kLimit;
    }
  }
  //  Plugin bypass check
  if (!bypass_plugin_buffer) {
    t1.set_size(i1_size[0], 2);
    b_loop_ub = out_wet.size(0) << 1;
    if (b_loop_ub - 1 >= 0) {
      std::copy(&out_wet[0], &out_wet[b_loop_ub], &t1[0]);
    }
  }
  //  Write back all per buffer state variables
  for (int n{0}; n < 8; n++) {
    plugin->wnm1[n] = wnm1_buffer[n];
    plugin->xnm1[n] = xnm1_buffer[n];
  }
  plugin->drv_x_prev[0] = drv_x_prev_buffer[0];
  plugin->drv_F_prev[0] = drv_F_prev_buffer[0];
  plugin->fb_x_prev[0] = fb_x_prev_buffer[0];
  plugin->fb_F_prev[0] = fb_F_prev_buffer[0];
  plugin->feedback[0] = feedback_buffer[0];
  plugin->drv_x_prev[1] = drv_x_prev_buffer[1];
  plugin->drv_F_prev[1] = drv_F_prev_buffer[1];
  plugin->fb_x_prev[1] = fb_x_prev_buffer[1];
  plugin->fb_F_prev[1] = fb_F_prev_buffer[1];
  plugin->feedback[1] = feedback_buffer[1];
  if (samplesPerFrame < 1.0) {
    b_loop_ub = 0;
  } else {
    b_loop_ub = static_cast<int>(samplesPerFrame);
  }
  o1_size[0] = b_loop_ub;
  o2_size[0] = b_loop_ub;
  if (b_loop_ub - 1 >= 0) {
    std::copy(&t1[0], &t1[b_loop_ub], &o1_data[0]);
  }
  for (int n{0}; n < b_loop_ub; n++) {
    o2_data[n] = t1[n + t1.size(0)];
  }
}

void resetCImpl(MoogLadderPluginStackData *SD, double rate)
{
  derivedAudioPlugin *plugin;
  plugin = getPluginInstance(SD);
  plugin->PrivateSampleRate = rate;
  //  DSP state reset
  for (int i{0}; i < 8; i++) {
    plugin->wnm1[i] = 0.0;
    plugin->xnm1[i] = 0.0;
  }
  double b_rate;
  double smoothingFactor;
  plugin->drv_x_prev[0] = 0.0;
  plugin->drv_F_prev[0] = 0.0;
  plugin->fb_x_prev[0] = 0.0;
  plugin->fb_F_prev[0] = 0.0;
  plugin->feedback[0] = 0.0;
  plugin->drv_x_prev[1] = 0.0;
  plugin->drv_F_prev[1] = 0.0;
  plugin->fb_x_prev[1] = 0.0;
  plugin->fb_F_prev[1] = 0.0;
  plugin->feedback[1] = 0.0;
  //  Smoother reset
  b_rate = plugin->PrivateSampleRate;
  plugin->fs = b_rate;
  b_rate = plugin->Cutoff;
  smoothingFactor = plugin->smoothingRate;
  plugin->CutoffSmoother.currentValue = b_rate;
  plugin->CutoffSmoother.targetValue = b_rate;
  plugin->CutoffSmoother.smoothingFactor = smoothingFactor;
  b_rate = plugin->Resonance;
  smoothingFactor = plugin->smoothingRate;
  plugin->ResonanceSmoother.currentValue = b_rate;
  plugin->ResonanceSmoother.targetValue = b_rate;
  plugin->ResonanceSmoother.smoothingFactor = smoothingFactor;
  b_rate = plugin->Gain;
  smoothingFactor = plugin->smoothingRate;
  plugin->GainSmoother.currentValue = b_rate;
  plugin->GainSmoother.targetValue = b_rate;
  plugin->GainSmoother.smoothingFactor = smoothingFactor;
  const double lo = 20.0, hi = 0.45*rate;
  plugin->kOnsetLogLo = std::log(lo);
  plugin->kOnsetLogStep = (std::log(hi) - std::log(lo)) / (derivedAudioPlugin::kOnsetTableSize - 1);
  for (int a=0; a<2; ++a) {
    for (int i = 0; i < derivedAudioPlugin::kOnsetTableSize; ++i) {
      plugin->kOnsetTable[a][i] = onsetK(std::exp(plugin->kOnsetLogLo + i*plugin->kOnsetLogStep), rate, a == 1);
    }
  }
    
}

// End of code generation (MoogLadderPlugin.cpp)
