//------------------------------------------------------------------------------------------------
// File:  acquireaudio.cpp
//
// Desc:  Initializes the sound components
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: acquireAudio
// Desc: Initializes sound components
//------------------------------------------------------------------------------------------------
bool VolucrisClient::acquireAudio()
{
  // If the sound configuration option is turned off, don't do anything
  if (APP_INFO(!myConfig.getSetting("Audio", "Enable", 1))("Sound disabled by config file"))
    return true;

  return global_sound_manager_.create(myMainWindow);

  //// Create DirectSound
  //if (APP_ERROR(FAILED(DirectSoundCreate8(NULL, &myDirectSound, NULL)) ||
  //         FAILED(myDirectSound->SetCooperativeLevel(myMainWindow, DSSCL_NORMAL)))
  //  ("Couldn't initialize DirectSound"))
  //  return false;

  //// Success
  //return true;
}
//
////------------------------------------------------------------------------------------------------
//// Name: playSound
//// Desc: 
////------------------------------------------------------------------------------------------------
//bool VolucrisClient::playSound(unsigned int index)
//{
//  // Don't do anything if DSound doesn't exist
//  if (!myDirectSound) return true;
//
//  // If this index is too high, exit
//  if (index >= myResources.sounds.soundArraySize) return true;
//
//  // Get the sound
//  ClientSound* sound = &myResources.sounds.soundArray[index];
//
//  // Search for a free buffer
//  int freeBuffer;
//  for (freeBuffer = 0; freeBuffer < NUMBER_OF_SOUND_BUFFERS; ++freeBuffer)
//  {
//    // Check to see if there is anything here at all
//    if (mySoundBuffers[freeBuffer] == NULL)
//      break;
//
//    // Check the status of the buffer (it's 0 if it is done playing)
//    DWORD status;
//    mySoundBuffers[freeBuffer]->GetStatus(&status) ;
//    if (status == 0)
//    {
//      // Get rid of this buffer
//      mySoundBuffers[freeBuffer]->Stop();
//      mySoundBuffers[freeBuffer]->Release();
//      mySoundBuffers[freeBuffer] = NULL;
//
//      // Exit the loop so we can use it
//      break;
//    }
//  }
//
//  // If no free buffer was found, just pick the first one
//  if (freeBuffer == NUMBER_OF_SOUND_BUFFERS)
//  {
//    freeBuffer = 0;
//    mySoundBuffers[0]->Stop();
//    mySoundBuffers[0]->Release();
//    mySoundBuffers[0] = NULL;
//  }
//
//  // Create the sound buffer description.  We want to be able to control the
//  // frequency, volume and pan of this sound.
//  DSBUFFERDESC dsbd;
//  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
//  dsbd.dwSize = sizeof(DSBUFFERDESC);
//  dsbd.dwFlags = DSBCAPS_CTRLVOLUME;// | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
//  dsbd.dwBufferBytes = sound->header.waveformSize;
//  dsbd.lpwfxFormat = &sound->header.waveFormat;
//
//  // Generate the sound buffer using a temporary interface
//  IDirectSoundBuffer *pDSB;
//  if (APP_ERROR(FAILED(myDirectSound->CreateSoundBuffer(&dsbd, &pDSB, NULL)))("Couldn't create sound buffer for playing"))
//    return false;
//
//  // Query the temporary interface for the real sound buffer
//  if (APP_ERROR(FAILED(pDSB->QueryInterface(IID_IDirectSoundBuffer8, (VOID**)&(mySoundBuffers[freeBuffer]))))("Unable to convert DirectSound buffer to DX8 interface"))
//  {
//    // Release the temporary buffer
//    pDSB->Release();
//
//    // Return the error
//    return true;
//  }
//
//  // Release the newer interface
//  pDSB->Release();
//
//  // Copy data into the buffer
//  BYTE* ptr1;
//  DWORD size1;
//  BYTE* ptr2;
//  DWORD size2;
//  if (APP_ERROR(FAILED(mySoundBuffers[freeBuffer]->Lock(0, sound->header.waveformSize,
//            (VOID**)&ptr1, &size1, (VOID**)&ptr2, &size2, 0)))("Couldn't lock sound buffer"))
//  {
//    // Get rid of the buffer
//    mySoundBuffers[freeBuffer]->Release();
//    mySoundBuffers[freeBuffer] = NULL;
//
//    // Return error
//    return false;
//  }
//
//  // Copy the data
//  memcpy(ptr1, sound->waveformData, size1);
//  if (ptr2 != NULL)
//    memcpy(ptr2, &sound->waveformData[size1], size2);
//
//  // Unlock the buffer
//  mySoundBuffers[freeBuffer]->Unlock((VOID*)ptr1, size1, (VOID*)ptr2, size2);
//
//  // Now that we have a buffer (finally!) we can play it.  The only flag we can modify here
//  // is the last one, which makes stuff loop.  I don't think we really want to make a looping
//  // sound at this point.
//  mySoundBuffers[freeBuffer]->SetCurrentPosition(0);
//  mySoundBuffers[freeBuffer]->SetVolume(DSBVOLUME_MAX);//(DSBVOLUME_MAX*.25+DSBVOLUME_MIN*.75));
//  //mySoundBuffers[freeBuffer]->SetFrequency(10000 + rand() - RAND_MAX / 2); // heh...mess with freqs
//  mySoundBuffers[freeBuffer]->Play(0, 0, 0);
//
//  // Success
//  return true;
//}