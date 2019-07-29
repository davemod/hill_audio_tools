/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:               hill_audio_tools
  vendor:           David Hill
  version:          0.1
  name:             Hill Audio Tools
  description:      Classes and wrappers to work with audio.
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
