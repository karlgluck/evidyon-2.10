//------------------------------------------------------------------------------------------------
// File:  projectiles.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"

//
////------------------------------------------------------------------------------------------------
//// Name:  createProjectile
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::createProjectile(int projectileID, float x, float z, float vx, float vz)
//{
//  Projectile projectile = { x, z, vx, vz };
//  myProjectiles.insert(ProjectileMap::value_type(projectileID, projectile));
//}
//
////------------------------------------------------------------------------------------------------
//// Name:  updateProjectiles
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::updateProjectiles(double time)
//{
//  static double lastUpdateTime = time;
//  double deltaTime = time - lastUpdateTime;
//  lastUpdateTime = time;
//
//  for (ProjectileMap::iterator i = myProjectiles.begin(); i != myProjectiles.end(); ++i)
//  {
//    // Move the projectile
//    i->second.x += i->second.vx * deltaTime;
//    i->second.z += i->second.vz * deltaTime;
//  }
//}
//
////------------------------------------------------------------------------------------------------
//// Name:  destroyProjectile
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::destroyProjectile(int projectileID)
//{
//  myProjectiles.erase(projectileID);
//}
//
////------------------------------------------------------------------------------------------------
//// Name:  getProjectile
//// Desc:  
////------------------------------------------------------------------------------------------------
//VolucrisClient::Projectile* VolucrisClient::getProjectile(int projectileID)
//{
//  ProjectileMap::iterator i = myProjectiles.find(projectileID);
//  return i == myProjectiles.end() ? 0 : &i->second;
//}
