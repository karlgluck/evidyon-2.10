#ifndef __ACTORTYPEDESCRIPTION_H__
#define __ACTORTYPEDESCRIPTION_H__

#include <d3dx9.h>
#include "../shared/client-editor/client_gamefile.h"

struct UnseenSkeletalAnimation;
struct GameFileDataSkinnedMesh;

namespace Evidyon {

struct ActorTypeDescription {
  const GameFileDataSkinnedMesh* mesh;

  struct Animation {
    const UnseenSkeletalAnimation* animation;
    float speed_factor, speed_offset;
  } animations[ACTORANIMATION_COUNT];

  unsigned int texture_id;

  D3DXMATRIX root_transform;

  struct AttachmentPoint {
    D3DXMATRIX offset_matrix;
    unsigned int source_bone;
  } attachment_points[ACTORATTACHMENTPOINT_COUNT];

  unsigned int sounds[ACTORSOUND_COUNT];
};

}

#endif