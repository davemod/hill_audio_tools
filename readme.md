# hill_audio_tools

hill_audio_tools will be a set of tools to simplify controlling audio files in juce. 

## Getting Started

### Installing

Download or clone the repository into your Juce user modules folder. You can define the path for user modules in the Projucer under "Projucer - Global Paths... - User Modules". 

### Create a project

Create a new project in Projucer and start with an Audio Application. Add the hill_audio_tools module to your project (go to modules, click the add button and juce "Add a module - Global user modules path - hill_audio_tools"). If you can't see the module, click Re-scan path. Create your Xcode (or whatever software you use) project.

### Add an audio file to the Projucer

Back in the projucer add an audio file to your project. Do this by clicking on "File Explorer", then drag and drop your audio file into the folder Source. Once you click save, the file is available as a binary source file in your project.


## Play a file

In Xcode got to your MainComponent.cpp file and find the Constructor. 

We now will initialize the AudioFilesPlayer singleton instance, load an audio file and play and loop it. We  will create a fade-in over 5 seconds from 0 gain to 0.8 gain. 

```c++
 auto background_music = AudioFilesPlayer::getInstance()->loadFile(BinaryData::test_wav, BinaryData::test:test_wavSize, false, "background_music");
    
    background_music->setGain(0);
    background_music->play();
    background_music->setGain(0.8,5);
    background_music->setLoop(true);
```

## Prepare to play and pass an audio buffer

In MainComponent.cpp find the method prepareToPlay(...) and call the process method in your AudioFilesPlayer.

```c++
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    AudioFilesPlayer::getInstance()->prepareToPlay(samplesPerBlockExpected, sampleRate);
}
```

Then go to the method getNextAudioBlock(...) and pass the audio buffer.

```c++
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    auto buffer = *bufferToFill.buffer;
    buffer.clear();
    AudioFilesPlayer::getInstance()->process(buffer);
  
}
```

## Control the file where ever you need

Now you can get the music file where ever you need it.

```c++
    auto background_music = AudioFilesPlayer::getInstance()->getFile("background_music");
    background_music->stop();
```


## Authors

* **David Hill** - *Initial work* - [davemod](https://github.com/davemod)

## License

This project is licensed under the MIT License - see the LICENSE.md file for details