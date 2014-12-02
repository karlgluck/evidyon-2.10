#include "globalsoundmanager.h"
#include <dc/debug>
#include "../common/safemacros.h"


namespace Evidyon {

GlobalSoundManager::GlobalSoundManager() {
  direct_sound_ = NULL;
  client_x_ = 0.0f;
  client_y_ = 0.0f;
  for (int i = 0; i < NUMBER_OF_SOUND_BUFFERS; ++i) {
    sound_buffers_[i] = NULL;
  }
  background_sound_buffer_ = NULL;
}


bool GlobalSoundManager::create(HWND hMainWindow) {
  if (APP_ERROR(FAILED(DirectSoundCreate8(NULL, &direct_sound_, NULL)) ||
           FAILED(direct_sound_->SetCooperativeLevel(hMainWindow, DSSCL_NORMAL)))
    ("Couldn't initialize DirectSound"))
    return false;
  return true;
}


bool GlobalSoundManager::addSound(unsigned int index, const ClientSound& sound) {
  if (!direct_sound_) return true;  // Don't do anything if DSound doesn't exist
  CONFIRM(sounds_.size() == index) else return false;
  sounds_.push_back(sound);
  return true;
}


void GlobalSoundManager::destroy() {
  for (Sounds::iterator i = sounds_.begin(); i != sounds_.end(); ++i) {
    SAFE_DELETE_ARRAY(i->waveformData);
  }
  sounds_.clear();


  // Go through the sound buffer array and release everything
  for (int i = 0; i < NUMBER_OF_SOUND_BUFFERS; ++i) {
    if (sound_buffers_[i]) {
      sound_buffers_[i]->Stop();
      sound_buffers_[i]->Release();
      sound_buffers_[i] = NULL;
    }
  }

  if (background_sound_buffer_ != NULL) {
    background_sound_buffer_->Stop();
    background_sound_buffer_->Release();
    background_sound_buffer_ = NULL;
  }

  SAFE_RELEASE(direct_sound_);
}



bool GlobalSoundManager::playSound(SoundPriority priority, unsigned int index) {
  if (!direct_sound_) return true;
  if (index >= sounds_.size()) return false;

  int buffer_index = getAvailableBufferIndex(priority);
  if (buffer_index < 0) return false; // couldn't play this sound

  LPDIRECTSOUNDBUFFER8 sound_buffer = NULL;
  CONFIRM(fillSoundBuffer(&sounds_.at(index), &sound_buffer)) else return false;
  sound_buffer_priority_[buffer_index] = priority;

  // Start the sound playing
  sound_buffer->SetCurrentPosition(0);
  sound_buffer->SetVolume(-1500); // down 15 decibels
  sound_buffer->Play(0, 0, 0);

  // Success
  return true;
}



bool GlobalSoundManager::playSound(SoundPriority priority, float x, float y, unsigned int index) {
  if (!direct_sound_) return true;
  if (index >= sounds_.size()) return false;

  // Find distance to the client
  float dx = x - client_x_;
  float dy = y - client_y_;
  float dd_sq = dx*dx+dy*dy;
  static const float MAX_DISTANCE_SQ = 16.0f*16.0f;

  // If this distance is too high, we don't need to play it
  if (dd_sq > MAX_DISTANCE_SQ) return true; 

  int buffer_index = getAvailableBufferIndex(priority);
  if (buffer_index < 0) return false; // couldn't play this sound

  LPDIRECTSOUNDBUFFER8 sound_buffer = NULL;
  CONFIRM(fillSoundBuffer(&sounds_.at(index), &sound_buffer)) else return false;
  sound_buffer_priority_[buffer_index] = priority;

  // Start the sound playing
  sound_buffer->SetCurrentPosition(0);
  static const int DB_HIGH = -1300, DB_LOW = -3000;
  static const int DB_PAN = 1000;
  sound_buffer->SetVolume((DB_HIGH - DB_LOW) * (dd_sq / MAX_DISTANCE_SQ) + DB_LOW);
  sound_buffer->SetPan((long)(dx * DB_PAN));
  sound_buffer->Play(0, 0, 0);

  // Success
  return true;
}


bool GlobalSoundManager::loopBackgroundSound(unsigned int index) {
  if (!direct_sound_) return true;
  if (background_sound_buffer_ != NULL) {
    background_sound_buffer_->Stop();
    background_sound_buffer_->Release();
    background_sound_buffer_ = NULL;
  }
  if (index >= sounds_.size()) return true; // turn off sound
  CONFIRM(fillSoundBuffer(&sounds_.at(index), &background_sound_buffer_)) else return false;
  background_sound_buffer_->SetCurrentPosition(0);
  background_sound_buffer_->SetVolume(-1800);
  background_sound_buffer_->Play(0, 0, DSBPLAY_LOOPING);
  return true;
}

//
//int GlobalSoundManager::loopSound(SoundPriority priority, int unique_id, unsigned int index, int last_retval) {
//  if (enable_retval < 0 || enable_retval >= NUMBER_OF_SOUND_BUFFERS) {
//    if (index >= sounds_.size()) return -1; // can't ever play this sound
//    int buffer_index = getAvailableBuffer(priority);
//    if (buffer_index < 0) return -1; // don't loop the sound yet
//    LPDIRECTSOUNDBUFFER8 sound_buffer = NULL;
//    CONFIRM(fillSoundBuffer(&sounds_.at(index), &sound_buffer)) else return false;
//    looping_sound_unique_id[buffer_index] = unique_id; // now playing this sound
//    sound_buffer_priority_[buffer_index] = priority;
//    sound_buffer->SetCurrentPosition(0);
//    sound_buffer->SetVolume((DSBVOLUME_MAX - DSBVOLUME_MIN) * 0.5f + DSBVOLUME_MIN);
//    sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
//    return buffer_index;
//  } else {
//    if (looping_sound_unique_id[last_retval] != unique_id) {
//      return -1; // have to start playing again next time loopSound is called
//    } else {
//      return last_retval; // still playing this sound; all's good
//    }
//  }
//}
//
//void GlobalSoundManager::loopSoundDisable(int unique_id, int enable_retval) {
//  if (enable_retval < 0 || enable_retval >= NUMBER_OF_SOUND_BUFFERS) return;
//  
//}

void GlobalSoundManager::update(float client_x, float client_y) {
  client_x_ = client_x;
  client_y_ = client_y;
}


int GlobalSoundManager::getAvailableBufferIndex(SoundPriority priority) {
  int available_buffer = -1; // index of a buffer that could be overwritten if no
                             // empty one is found
  for (int buffer = 0; buffer < NUMBER_OF_SOUND_BUFFERS; ++buffer) {
    if (sound_buffers_[buffer] == NULL) return buffer;
    if (sound_buffer_priority_[buffer] <= priority) {
      available_buffer = buffer;
    }
  }

  if (available_buffer < 0) return -1;

  LPDIRECTSOUNDBUFFER8 buffer = sound_buffers_[available_buffer];
  buffer->Stop();
  buffer->Release();
  sound_buffers_[available_buffer] = NULL;
  sound_buffer_priority_[available_buffer] = SOUNDPRIORITY_AMBIENT;
  return available_buffer;
}








bool GlobalSoundManager::fillSoundBuffer(const ClientSound* sound, LPDIRECTSOUNDBUFFER8* buffer) {
  ASSERT(direct_sound_) else return false; // this should be checked elsewhere
  CONFIRM(sound && buffer) else return false;

  // Create the sound buffer description.  We want to be able to control the
  // frequency, volume and pan of this sound.
  DSBUFFERDESC dsbd;
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN; //  | DSBCAPS_CTRLFREQUENCY
  dsbd.dwBufferBytes = sound->header.waveformSize;
  dsbd.lpwfxFormat = const_cast<LPWAVEFORMATEX>(&sound->header.waveFormat);

  // Generate the sound buffer using a temporary interface
  IDirectSoundBuffer* pDSB;
  if (APP_ERROR(FAILED(direct_sound_->CreateSoundBuffer(&dsbd, &pDSB, NULL)))("Couldn't create sound buffer for playing"))
    return false;

  // Query the temporary interface for the version 8 sound buffer
  LPDIRECTSOUNDBUFFER8 internal_buffer = NULL;
  CONFIRM(SUCCEEDED(pDSB->QueryInterface(IID_IDirectSoundBuffer8, (VOID**)&internal_buffer))) else {
    pDSB->Release();
    return false;
  }

  // Release the newer interface
  pDSB->Release();

  // Copy data into the buffer
  BYTE* ptr1;
  DWORD size1;
  BYTE* ptr2;
  DWORD size2;
  if (APP_ERROR(FAILED(internal_buffer->Lock(0, sound->header.waveformSize,
            (VOID**)&ptr1, &size1, (VOID**)&ptr2, &size2, 0)))("Couldn't lock sound buffer"))
  {
    internal_buffer->Release();
    return false;
  }

  // Copy the data
  memcpy(ptr1, sound->waveformData, size1);
  if (ptr2 != NULL)
    memcpy(ptr2, &sound->waveformData[size1], size2);

  // Unlock the buffer
  internal_buffer->Unlock((VOID*)ptr1, size1, (VOID*)ptr2, size2);

  // Save the pointer
  *buffer = internal_buffer;

  // Success
  return true;
}

}