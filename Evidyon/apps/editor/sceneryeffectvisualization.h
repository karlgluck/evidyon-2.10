//------------------------------------------------------------------------------------------------
// File:    sceneryeffectvisualization.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SCENERYEFFECTVISUALIZATION_H__
#define __SCENERYEFFECTVISUALIZATION_H__



/**
 * 
 */
class SceneryEffectVisualization : public dc::dcResource<SceneryEffectVisualization,EffectVisualization>
{
    public:

        /**
         * Initializes this class
         */
        SceneryEffectVisualization();

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

        dc::dcList<Scenery>::Reference myScenery;
        dcxVector3<dcByte> myTranslation;
        dc::dcBoolean myRotateFlag;


    protected:

        static dc::dcClass<SceneryEffectVisualization>::Implements<EffectVisualization> implementsEffectVisualization;
};




#endif