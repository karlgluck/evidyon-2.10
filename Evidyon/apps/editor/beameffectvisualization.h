//------------------------------------------------------------------------------------------------
// File:    beameffectvisualization.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __BEAMEFFECTVISUALIZATION_H__
#define __BEAMEFFECTVISUALIZATION_H__



/**
 * 
 */
class BeamEffectVisualization : public dc::dcResource<BeamEffectVisualization,EffectVisualization>
{
    public:

        /**
         * Initializes this class
         */
        BeamEffectVisualization();

        /**
         * Enters data into the client effect structure
         */
        bool compileForClient(ClientEffectVisualizationDescription* description) const;


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The image for this particle
        dc::dcTable<Image>::Reference mySourceImage;
        dc::dcTable<Image>::Reference myTerminalImage;
        dc::dcTable<Image>::Reference myBeamFillImage;

        /// The width of the beam
        dc::dcFloat myBeamWidth;

        /// The size of the start/end images
        dc::dcFloat myEndsImageSize;

        /// The beam's distance from the ground
        dc::dcFloat myHeight;

        dc::dcBoolean myCastFromHandsFlag;


    protected:

        static dc::dcClass<BeamEffectVisualization>::Implements<EffectVisualization> implementsEffectVisualization;
};




#endif