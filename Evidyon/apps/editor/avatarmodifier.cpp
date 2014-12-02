////------------------------------------------------------------------------------------------------
//// File:    avatarmodifier.cpp
////
//// Desc:    
////
//// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
////------------------------------------------------------------------------------------------------
//#include "stdafx.h"
//#include "avatarmodifier.h"
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: AvatarModifier
//// Desc: Initializes the class
////------------------------------------------------------------------------------------------------
//AvatarModifier::AvatarModifier()
//{
//    member("+ Min Physical Damage", &myMinPhysicalDamage);
//    member("+ Max Physical Damage", &myMaxPhysicalDamage);
//    member("+ Max HP", &myMaxHP);
//    member("+ Max MP", &myMaxMP);
//    member("+ Defense", &myDefense);
//    member("+ Max Movement Speed", &myMaxMovementSpeed);
//
//    member("+ Attack Speed %", &myAttackSpeedPercent);
//    member("+ Regenerate HP", &myRegenerateHP);
//    member("+ Regenerate MP", &myRegenerateMP);
//    member("+ Strength", &myAbilityModifiers[0]);
//    member("+ Agility", &myAbilityModifiers[1]);
//    member("+ Constitution", &myAbilityModifiers[2]);
//    member("+ Intelligence", &myAbilityModifiers[3]);
//    member("+ Wisdom", &myAbilityModifiers[4]);
//    member("Luckiness", &myLuckinessPercentage);
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: fillServerDescription
//// Desc: Enters data into the server structure
////------------------------------------------------------------------------------------------------
//void AvatarModifier::fillServerDescription(ServerAvatarModifierDescription* description) const
//{
//    description->minPhysicalDamage = myMinPhysicalDamage.getValue();
//    description->maxPhysicalDamage = myMaxPhysicalDamage.getValue();
//    description->maxHP = myMaxHP.getValue();
//    description->maxMP = myMaxMP.getValue();
//    description->defense = myDefense.getValue();
//    description->maxMovementSpeed = myMaxMovementSpeed.getValue();
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: appendClientDescriptionString
//// Desc: Adds the client's description
////------------------------------------------------------------------------------------------------
//void AvatarModifier::appendClientDescriptionString(std::string* string) const
//{
//    char buildBuffer[128];
//
//    if (myMinPhysicalDamage.getValue() || myMaxPhysicalDamage.getValue())
//    {
//        int min = myMinPhysicalDamage.getValue();
//        int max = myMaxPhysicalDamage.getValue();
//        if (min < max && (min > 0))
//        {
//            sprintf_s(buildBuffer, sizeof(buildBuffer), "\n%i - %i damage", min, max);
//            string->append(buildBuffer);
//        }
//        else
//        {
//            if (min)
//            {
//                sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%i minimum damage", min);
//                string->append(buildBuffer);
//            }
//            if (max)
//            {
//                sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%i maximum damage", max);
//                string->append(buildBuffer);
//            }
//        }
//    }
//
//    if (myDefense.getValue()) 
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n%lu defense", myDefense.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myMaxHP.getValue()) 
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu max HP", myMaxHP.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myMaxMP.getValue()) 
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu max MP", myMaxHP.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myMaxMovementSpeed.getValue()) 
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%.02f movement speed", myMaxMovementSpeed.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAttackSpeedPercent.getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%.02f attack speed", myAttackSpeedPercent.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myRegenerateHP.getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%.0f regenerate hp", myRegenerateHP.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myRegenerateMP.getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%.0f regenerate mp", myRegenerateMP.getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAbilityModifiers[0].getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu strength", myAbilityModifiers[0].getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAbilityModifiers[1].getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu agility", myAbilityModifiers[1].getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAbilityModifiers[2].getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu constitution", myAbilityModifiers[2].getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAbilityModifiers[3].getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu intelligence", myAbilityModifiers[3].getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myAbilityModifiers[4].getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%lu wisdom", myAbilityModifiers[4].getValue());
//        string->append(buildBuffer);
//    }
//
//    if (myLuckinessPercentage.getValue())
//    {
//        sprintf_s(buildBuffer, sizeof(buildBuffer), "\n+%.02f luckiness", myLuckinessPercentage.getValue());
//        string->append(buildBuffer);
//    }
//}
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: staticTypeName
//// Desc: Returns the type name of this class
////------------------------------------------------------------------------------------------------
//std::string AvatarModifier::staticTypeName()
//{
//    return "AvatarModifier";
//}
//
