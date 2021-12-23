/*
  ==============================================================================

    AudioFilesPlayer.cpp
    Created: 16 Jun 2019 12:17:38pm
    Author:  David Hill

  ==============================================================================
*/

#include "AudioFilesPlayer.h"

namespace hill_AudioTools
{

#if ENABLE_SINGLETON_PLAYER
JUCE_IMPLEMENT_SINGLETON(AudioFilesPlayer)
#endif

AudioFilesPlayer::AudioFilesPlayer()
{
}

AudioFilesPlayer::~AudioFilesPlayer()
{
#if ENABLE_SINGLETON_PLAYER
    clearSingletonInstance();
#endif
}

AudioFilePlayer* AudioFilesPlayer::loadFile(const void *sourceData, size_t sourceDataSize, bool keepInternalCopyOfData, String name)
{
    ScopedLock sl {cs};
    
    // call prepareToPlay() first
    jassert (sampleRate>0);
    
    auto file = audioFiles.add(new AudioFilePlayer(sourceData, sourceDataSize,keepInternalCopyOfData, name, audioFiles.size()));
    
    file->prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    return file;
}

AudioFilePlayer* AudioFilesPlayer::loadFile(const File& file, const String& name)
{
    ScopedLock sl {cs};
    
    // call prepareToPlay() first
    jassert (sampleRate>0);

    auto player = audioFiles.add (new AudioFilePlayer (file, name, audioFiles.size()));
    
    player->prepareToPlay (samplesPerBlockExpected, sampleRate);
    
    return player;
}

void AudioFilesPlayer::playFile(String name)
{
    if (auto file = getFile(name))
        file->play();
}

void AudioFilesPlayer::playFileWithId(int ID)
{
    if (auto file = getFileById(ID))
        file->play();
}

void AudioFilesPlayer::playFileWithIndex(int index)
{
    if (auto file = getFileByIndex(index))
        file->play();
}

void AudioFilesPlayer::stopFile(String name)
{
    if (auto file = getFile(name))
        file->stop();
}

void AudioFilesPlayer::stopFileWithId(int ID)
{
    if (auto file = getFileById(ID))
        file->stop();
}

void AudioFilesPlayer::stopFileWithIndex(int index)
{
    if (auto file = getFileByIndex(index))
        file->stop();
}

void AudioFilesPlayer::playAll()
{
    for (auto file : audioFiles)
        file->play();
}

void AudioFilesPlayer::stopAll()
{
    for (auto file : audioFiles)
        file->stop();
}

void AudioFilesPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    ScopedLock sl {cs};
    
    this->sampleRate = sampleRate;
    this->samplesPerBlockExpected = samplesPerBlockExpected;
    
    for (auto file : audioFiles)
    {
        file->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void AudioFilesPlayer::releaseResources()
{
    ScopedLock sl {cs};
    for (auto file : audioFiles)
    {
        file->releaseResources ();
    }
}

void AudioFilesPlayer::process(AudioBuffer<float> &buffer)
{
    if (cs.tryEnter())
    {
        for (auto file : audioFiles)
        {
            file->process(buffer);
        }
        
        cs.exit ();
    }
}

AudioFilePlayer* AudioFilesPlayer::getFile(String name)
{
    for (auto file : audioFiles)
    {
        if (file->getName() == name)
        {
            return file;
        }
    }
    return nullptr;
}

AudioFilePlayer* AudioFilesPlayer::getFileById(int ID)
{
    for (auto file : audioFiles)
    {
        if (file->getID() == ID)
        {
            return file;
        }
    }
    return nullptr;
}

AudioFilePlayer* AudioFilesPlayer::getFileByIndex(int index)
{
    return audioFiles [index];
}

}
