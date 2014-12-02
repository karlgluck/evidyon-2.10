#ifndef __EVIDYON_COMMON_WORLD_H__
#define __EVIDYON_COMMON_WORLD_H__


namespace Evidyon {

namespace World {

typedef unsigned long RegionID;
const static int REGION_SIZE = 16;
const static int REGION_BORDER_SIZE = 5;
const static float REGION_RADIUS = REGION_SIZE / 2.0f + REGION_BORDER_SIZE;
const static float REGION_RADIUS_SQ = REGION_RADIUS*REGION_RADIUS;


RegionID regionIDFromPoint(float x, float y);
RegionID regionIDFromPoint(float x, float y, short* region_x, short* region_y);
RegionID regionIDFromPoint(int x, int y);
RegionID regionIDFromPoint(int x, int y, short* region_x, short* region_y);
RegionID regionIDFromRegionCoordinates(short region_x, short region_y);

}

}

#endif