////------------------------------------------------------------------------------------------------
//// File:    avatarmodifier.h
////
//// Desc:    
////
//// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
////------------------------------------------------------------------------------------------------
//#ifndef __AVATARMODIFIER_H__
//#define __AVATARMODIFIER_H__
//
//
//
//
///**
// * 
// */
//class AvatarModifier : public dc::dcResource<AvatarModifier>
//{
//public:
//
//    /**
//     * Initializes the class
//     */
//    AvatarModifier();
//
//    /**
//     * Enters data into the server effect structure
//     */
//    void fillServerDescription(ServerAvatarModifierDescription* description) const;
//
//    /**
//     * Adds the client's description
//     */
//    void appendClientDescriptionString(std::string* string) const;
//
//
//public:
//
//    /**
//     * Returns the type name of this class
//     */
//    static std::string staticTypeName();
//
//
//protected:
//
//    dc::dcInteger myMinPhysicalDamage;
//    dc::dcInteger myMaxPhysicalDamage;
//    dc::dcInteger myMaxHP;
//    dc::dcInteger myMaxMP;
//    dc::dcInteger myDefense;
//    dc::dcFloat myMaxMovementSpeed;
//
//    dc::dcFloat myAttackSpeedPercent;
//    dc::dcFloat myRegenerateHP, myRegenerateMP;
//
//    dc::dcByte myAbilityModifiers[5];
//
//    dc::dcFloat myLuckinessPercentage;
//};
//
//
//
//
//
//#endif
//
//
