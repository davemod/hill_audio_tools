/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:               hill_AudioTools
  vendor:           David Hill
  version:          0.1
  name:             Hill Audio Tools
  description:      Some handy classes and methods to ease life
  website:          ...
  license:          MIT

  dependencies:     juce_core, juce_audio_utils

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

#include <iostream>

#include <juce_core/juce_core.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "sources/SineWave.h"
#include "sources/GainCtrl.h"
#include "sources/AudioFilePlayer.h"
#include "sources/AudioFilesPlayer.h"

/** Config: ENABLE_SINGLETON_PLAYER
 Enables the AudioFilesPlayer to act as a singleton. Do not use this if you want to run several instances of your application.
 */
#ifndef ENABLE_SINGLETON_PLAYER
#define ENABLE_SINGLETON_PLAYER 1
#endif

/** Config: ENABLE_USING_HILL_AUDIO_TOOLS
 Enables using the namespace hill_AudioTools
 */
#ifndef ENABLE_USING_HILL_AUDIO_TOOLS
#define ENABLE_USING_HILL_AUDIO_TOOLS 1
#endif

#if ENABLE_USING_HILL_AUDIO_TOOLS
using namespace hill_AudioTools;
#endif
