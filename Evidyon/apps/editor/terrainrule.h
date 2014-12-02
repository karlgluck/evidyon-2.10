//------------------------------------------------------------------------------------------------
// File:    terrainrule.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __TERRAINRULE_H__
#define __TERRAINRULE_H__


/**
 *
 */
class TerrainRule : public dc::dcResource<TerrainRule>
{
    public:

        typedef dc::dcList<dcList<Terrain>::Reference> OptionList;

    public:
/*
        enum Direction
        {
            DIRECTION_NORTH,
            DIRECTION_EAST,
            DIRECTION_SOUTH,
            DIRECTION_WEST,

            DIRECTION_COUNT,
        };*/

        /**
         * Contains raw information needed to display a preview of this resource
         */
        struct Preview
        {
            /**
             * A list of terrain options for a given direction
             */
            struct TerrainArray
            {
                Terrain::Preview** terrainArray;
                size_t terrainArraySize;
            };

            /// The resource that created this preview
            TerrainRule* terrainRule;

            /// Own canvas types
            TerrainArray canvasTypes;

            // Arrays of terrain types in each direction, starting from the north-west
            // corner:
            //  012
            //  7 3
            //  654
            TerrainArray neighborOptions[8];

            /// Possible fill types
            TerrainArray fillTypes;
        };


    public:

        /**
         * Initializes this class
         */
        TerrainRule();

        /**
         * Fills in the preview structure for this resource
         */
        bool loadPreview(Terrain::Preview* terrainPreviewArray, size_t terrainPreviews, Preview* preview);

        /**
         * Compares this terrain rule with another rule (sorts by priority)
         */
        int compareTo(const dc::dcGenericResource* other) const;

        /**
         * Returns the name of this class type
         */
        static std::string staticTypeName();

        dc::dcInteger* getPriority() { return &myPriority; }
        OptionList* getCanvasTypes() { return &myCanvasTypes; }
        OptionList* getFillTypes() { return &myFillTypes; }

        OptionList* getNeighborTypes(int index) { return &myNeighborTypes[index]; }


    protected:

        /// The priority of this terrain rule.  Rules are applied in order, so the
        /// most specific rules should come last in the list.
        dc::dcInteger myPriority;

        /// The kind of terrain that has to be at this location for the rule to apply
        OptionList myNeighborTypes[8];

        OptionList myCanvasTypes;

        /// Which types are used to fill this layer
        OptionList myFillTypes;
};





#endif