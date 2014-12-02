#ifndef __SERVER_ATLAS_H__
#define __SERVER_ATLAS_H__

#include "../evidyon_map.h"

namespace Evidyon {

struct CompiledAtlasServerHeader
{
    size_t width, height;

    int spawnX, spawnY;
    float spawnRadius;

    MapNavigability defaultNavigability;
};

}

#endif