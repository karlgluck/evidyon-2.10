//------------------------------------------------------------------------------------------------
// File:    particletraileffectvisualization.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PARTICLETRAILEFFECTVISUALIZATION_H__
#define __PARTICLETRAILEFFECTVISUALIZATION_H__



/**
 * 
 */
class ParticleTrailEffectVisualization : public dc::dcResource<ParticleTrailEffectVisualization,EffectVisualization>
{
    public:

        /**
         * Initializes this class
         */
        ParticleTrailEffectVisualization();

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
        dc::dcTable<Image>::Reference myImage;

        /// The different colors for the particles
        dcxVector3<dcByte> myParticleColor[3];

        /// How many particles to create each period
        dc::dcByte myParticlesPerPeriod;

        /// How quickly new particles are created
        dc::dcFloat mySpawnPeriod;

        /// How often particles are updated
        dc::dcFloat myUpdatePeriod;

        /// How quickly the particles drift upward (negative is downward)
        dc::dcFloat myRisingSpeed;

        /// Distance away in the ring from the source point
        dc::dcFloat myInitialDistanceMin, myInitialDistanceMax;

        /// How fast the particles move away from the center
        dc::dcFloat myRadialVelocity;

        /// How fast the particles move in the effect's linear direction
        dc::dcFloat myDirectedVelocity;

        /// How long each particle lasts
        dc::dcFloat myLifetime;

        /// How long it takes for a particle to decay once its lifetime is over
        dc::dcFloat myDeathTime;

        /// The initial Y-location of the particles
        dc::dcFloat myInitialHeightMin, myInitialHeightMax;

        /// The size of the particles
        dc::dcFloat myParticleSize;

        /// Whether or not to make these particles fall
        dc::dcFloat myGravityFactor;

    protected:

        static dc::dcClass<ParticleTrailEffectVisualization>::Implements<EffectVisualization> implementsEffectVisualization;
};




#endif