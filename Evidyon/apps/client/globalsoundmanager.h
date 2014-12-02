#ifndef __GLOBALSOUNDMANAGER_H__
#define __GLOBALSOUNDMANAGER_H__

#include <dsound.h>
#include <vector>
#include "../shared/client-editor/client_gamefile.h"

namespace Evidyon {

class Actor;

enum SoundPriority {
  SOUNDPRIORITY_AMBIENT,    // owl hooting, footsteps, sloshing through water
  SOUNDPRIORITY_NEARBY,     // ex: spell
  SOUNDPRIORITY_SELF,       // always played; ex: own attack
};

class GlobalSoundManager {
  static const int NUMBER_OF_SOUND_BUFFERS = 4;
  typedef std::vector<ClientSound> Sounds;

public:
  GlobalSoundManager();

  // Initializes this class
  bool create(HWND hMainWindow);

  // Called in sequence-order for each sound loaded from the resource file
  bool addSound(unsigned int index, const ClientSound& sound);

  void destroy();
  bool playSound(SoundPriority priority, unsigned int index);
  bool playSound(SoundPriority priority, float x, float y, unsigned int index);
  bool playSound(SoundPriority priority, float x, float y, float vx, float vy, unsigned int index);
  bool loopBackgroundSound(unsigned int index);

  // This does the same thing as playSound, except that sounds can be
  // both started and stopped.  This is used for things like actor
  // footstep sounds.  Pass the last returned value of loopSound (or
  // -1 the first time) as last_retval.
  //int loopSound(SoundPriority priority, int unique_id, unsigned int index, int last_retval);
  //void loopSoundDisable(int unique_id, int enable_retval);

  // Syncs the manager with the client's position so that spatial sounds can
  // be panned appropriately.
  void update(float client_x, float client_y);


private:
  // Obtains the index of a sound buffer that can be overwritten
  // in order to play a sound of the requested priority.  If the
  // return value is negative, no buffer could be found.
  int getAvailableBufferIndex(SoundPriority priority);

  // Loads the given sound into the buffer structure
  bool fillSoundBuffer(const ClientSound* sound, LPDIRECTSOUNDBUFFER8* buffer);


private:
  IDirectSound8* direct_sound_;

  // Sounds loaded from the resource file
  Sounds sounds_;

  // Array of sound buffers that are currently in use.  When
  // a sound buffer is inactive, its value is NULL.
  LPDIRECTSOUNDBUFFER8 sound_buffers_[NUMBER_OF_SOUND_BUFFERS];
  SoundPriority sound_buffer_priority_[NUMBER_OF_SOUND_BUFFERS];
  int looping_sound_unique_id[NUMBER_OF_SOUND_BUFFERS];
  LPDIRECTSOUNDBUFFER8 background_sound_buffer_;

  // This is the last-updated location of the client, used
  // to calculate how to play spatial sounds.
  float client_x_, client_y_;
};

}


#endif