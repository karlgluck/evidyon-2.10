////------------------------------------------------------------------------------------------------
//// File:    aicharactertype.h
////
//// Desc:    
////
//// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
////------------------------------------------------------------------------------------------------
//#ifndef __AICHARACTERTYPE_H__
//#define __AICHARACTERTYPE_H__
//
//
///**
// * This is a version of ActorFaction that is valid to use with dc::dcEnum
// */
//enum EditorActorFaction
//{
//    EDITORACTORFACTION_ROGUE,
//    EDITORACTORFACTION_LAWFUL,
//    EDITORACTORFACTION_NEUTRAL,
//    EDITORACTORFACTION_CHAOTIC,
//    EDITORACTORFACTION_EVIL,
//
//    EDITORACTORFACTION_COUNT,
//    EDITORACTORFACTION_INVALID,
//};
//
//
///**
// * Defines a computer-controlled character
// */
//class AICharacterType : public dc::dcResource<AICharacterType>
//{
//    public:
//
//        /**
//         * Initializes this class
//         */
//        AICharacterType();
//
//        /**
//         * Compiles this type into a game-file representation for use by the server
//         */
//        //bool compileForServer(ServerAICharacterTypeDescription* description) const;
//
//        void setActorType(dcList<ActorType>::Element* actorType) { myActorType.setReferencedResource(actorType); }
//
//    public:
//
//        /**
//         * Returns the type name of this class
//         */
//        static std::string staticTypeName();
//
//
//    protected:
//
//        /// The actor that this character uses to represent itself in the world
//        dc::dcList<ActorType>::Reference myActorType;
//
//        /// Defines the kind of targets this actor should acquire
//        dc::dcEnum<EditorActorFaction> myFaction;
//
//        /// How much experience this character gives when killed by a member of the opposing faction
//        dc::dcInteger myExperienceOffset, myExperienceFactor;
//
//        dc::dcInteger myAttackDamageOffset, myAttackDamageFactor;
//        dc::dcFloat myMovementSpeedOffset, myMovementSpeedFactor;
//        dc::dcFloat myAttackDelayOffset, myAttackDelayFactor;
//        dc::dcInteger myHPOffset, myHPFactor;
//
//        dc::dcList<Scenery>::Reference myEquippedItems[ACTORATTACHMENTPOINT_COUNT];
//};
//
//
//#endif