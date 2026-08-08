//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// MoogLadderPlugin_types.h
//
// Code generation for function 'onParamChangeCImpl'
//

#ifndef MOOGLADDERPLUGIN_TYPES_H
#define MOOGLADDERPLUGIN_TYPES_H

// Include files
#include "MoogLadderPlugin.h"
#include "rtwtypes.h"

// Type Definitions
struct MoogLadderPluginPersistentData {
  derivedAudioPlugin plugin;
  boolean_T plugin_not_empty;
};

struct MoogLadderPluginStackData {
  MoogLadderPluginPersistentData *pd;
};

#endif
// End of code generation (MoogLadderPlugin_types.h)
