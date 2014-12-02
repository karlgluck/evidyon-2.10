////------------------------------------------------------------------------------------------------
//// File:  specialfx.cpp
////
//// Desc:  
////
//// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
////------------------------------------------------------------------------------------------------
//#include "stdafx.h"
//
//
//#define SPECIAL_EFFECT_POINT_CAPACITY   1000
//
//
//struct PointVertex
//{
//  D3DXVECTOR3 posit;
//  float     size;
//  D3DCOLOR  color;
//
//  enum FVF
//  {
//    FVF_Flags = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE
//  };
//};
//
//
////----------------------------------------------------------------------------------------------
//// Name: createSpecialEffectBuffers
//// Desc: Generates the D3D objects for special effects
////------------------------------------------------------------------------------------------------
//bool VolucrisClient::createSpecialEffectBuffers()
//{
//  // Create a vertex buffer
//  if (FAILED(myD3DDevice->CreateVertexBuffer(
//          sizeof(PointVertex) * SPECIAL_EFFECT_POINT_CAPACITY,
//          D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|(myUsingSoftwareDeviceFlag ? D3DUSAGE_SOFTWAREPROCESSING : 0),
//          PointVertex::FVF_Flags,
//          D3DPOOL_DEFAULT,
//          &mySpecialEffectParticlesVB, 0)))
//  {
//    return false;
//  }
//
//  return true;
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: Erases special effects
//// Desc: destroySpecialEffectBuffers
////------------------------------------------------------------------------------------------------
//void VolucrisClient::destroySpecialEffectBuffers()
//{
//  SAFE_RELEASE(mySpecialEffectParticlesVB);
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  addSpecialEffect
//// Desc:  Creates a special effect
////------------------------------------------------------------------------------------------------
//EffectVisualizationInstance* VolucrisClient::addSpecialEffect(int effectID, float srcX, float srcY, int srcActor,
//                     float tgtX, float tgtY, int tgtActor, int tgtProjectile)
//{
//  EffectVisualizationInstance* newInstance = new EffectVisualizationInstance;
//  newInstance->description = &myResources.effects.effectVisualizationArray[effectID];
//
//  // Activate immediately
//  newInstance->nextActivationTime = 0.0;
//  newInstance->nextUpdateTime = 0.0;
//
//  newInstance->expirationTime = dcxWin32Clock::getAccurateSystemTime() + newInstance->description->duration;
//  newInstance->srcX = srcX;
//  newInstance->srcY = srcY;
//  newInstance->srcActor = srcActor;
//  newInstance->tgtX = tgtX;
//  newInstance->tgtY = tgtY;
//  newInstance->tgtActor = tgtActor;
//  newInstance->tgtProjectile = tgtProjectile;
//
//  //newInstance->particles[0] = 0;
//  newInstance->numberOfParticles = 0;
//
//  // Add to special effects
//  myEffectVisualizations.push_back(newInstance);
//  return newInstance;
//}
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  updateSpecialEffects
//// Desc:  
////------------------------------------------------------------------------------------------------
//void VolucrisClient::updateSpecialEffects(double time)
//{
//  static float lastUpdateTime = time;
//  float timeDelta = time - lastUpdateTime;
//  lastUpdateTime = time;
//
//  // Update each entry in the list
//  for (EffectVisualizationListIterator i = myEffectVisualizations.begin(); i != myEffectVisualizations.end();)
//  {
//    EffectVisualizationInstance* instance = (*i);
//    const ClientEffectVisualizationDescription* description = instance->description;
//
//    // Get actors.  If the actors are valid but can't be found, erase this effect
//    Actor* srcActor = getActor(instance->srcActor);
//    Actor* tgtActor = getActor(instance->tgtActor);
//
//    // If this effect has expired and all of its particles are gone, remove it
//    bool effectExpired = time > instance->expirationTime || (instance->srcActor && !srcActor) || (instance->tgtActor && !tgtActor);
//    if (effectExpired)
//    {
//      //if (instance->particles.empty())
//      if (!instance->numberOfParticles)
//      {
//        EffectVisualizationListIterator next = i; ++next;
//        myEffectVisualizations.erase(i);
//        i = next;
//      }
//      else
//        ++i;
//    }
//    else
//      ++i;
//
//    // The location parameters
//    float srcX, srcY;
//    float tgtX, tgtY;
//    float srcTgtAngle, srcTgtDistance;
//    
//    // Get the source location
//    if (srcActor)
//    {
//      srcX = srcActor->getX();
//      srcY = srcActor->getY();
//    }
//    else
//    {
//      srcX = instance->srcX;
//      srcY = instance->srcY;
//    }
//
//    // Get the target location
//    if (tgtActor)
//    {
//      tgtX = tgtActor->getX();
//      tgtY = tgtActor->getY();
//    }
//    else
//    {
//      // Check for projectile
//      Projectile* tgtProjectile = getProjectile(instance->tgtProjectile);
//
//      if (tgtProjectile)
//      {
//        tgtX = tgtProjectile->x;
//        tgtY = tgtProjectile->z;
//      }
//      else
//      {
//        tgtX = instance->tgtX;
//        tgtY = instance->tgtY;
//      }
//    }
//
//    // Calculate the distance and angle from the source to target
//    if ((srcActor /*|| tgtActor*/) && (srcActor == tgtActor))
//    {
//      srcTgtAngle = srcActor->getAngle();
//      srcTgtDistance = 0.0f;
//    }
//    else
//    {
//      float dx = tgtX - srcX, dy = tgtY - srcY;
//      srcTgtDistance = sqrtf(dx*dx+dy*dy);
//      srcTgtAngle = ((srcTgtDistance > 0.0f) || (!srcActor)) ? atan2(dy, dx) : srcActor->getAngle();
//    }
//
//    if (description->type == CEVISUALIZATION_SCENERY)
//    {
//      instance->tgtX = tgtX;
//      instance->tgtY = tgtY;
//      instance->srcX = srcX;
//      instance->srcY = srcY;
//
//      HSCENERY hScenery;
//
//      // Have to check for expiration first, otherwise we get doubly generated scenery
//      if (effectExpired)
//      {
//        hScenery = instance->particles[0]->Scenery.hScenery;
//        if (instance->particles[0]->Scenery.valid)
//        {
//          mySceneryRenderer.removeScenery(hScenery);
//        }
//
//        instance->particles[0]->Scenery.valid = false;
//        myPointParticlePool.release(instance->particles[0]);
//        instance->numberOfParticles = 0;
//      }
//      else if (!instance->numberOfParticles)
//      {
//        instance->particles[0] = myPointParticlePool.acquire();
//        instance->numberOfParticles = 1;
//
//        if (mySceneryRenderer.addScenery(0, &myResources.scenery.sceneryArray[description->Scenery.sceneryIndex], &hScenery))
//        {
//          instance->particles[0]->Scenery.valid = true;
//          instance->particles[0]->Scenery.hScenery = hScenery;
//        }
//        else
//        {
//          instance->particles[0]->Scenery.valid = false;
//        }
//
//      } 
//      else
//        hScenery = instance->particles[0]->Scenery.hScenery;
//
//      if (instance->particles[0]->Scenery.valid)
//      {
//        // Update
//        D3DXMATRIXA16 transform;
//        D3DXMatrixTranslation(&transform, tgtX + description->Scenery.x, description->Scenery.y, tgtY + description->Scenery.z);
//        if (description->Scenery.rotate && srcActor)
//        {
//          D3DXMATRIXA16 rotation;
//          D3DXMatrixRotationY(&rotation, srcActor->getAngle());
//          D3DXMatrixMultiply(&transform, &rotation, &transform);
//        }
//        mySceneryRenderer.updateSceneryTransform(hScenery, &transform);
//      }
//    } else
//
//    if (description->type == CEVISUALIZATION_BEAM)
//    {
//      instance->tgtX = tgtX;
//      instance->tgtY = tgtY;
//      instance->srcX = srcX;
//      instance->srcY = srcY;
//
//      if (effectExpired)
//      {
//        myPointParticlePool.release(instance->particles[0]);
//        instance->numberOfParticles = 0;
//      } else if (!instance->numberOfParticles)
//      {
//        // Add a fake particle
//        instance->particles[0] = myPointParticlePool.acquire();
//        instance->numberOfParticles++;
//      }
//    } else
//
//
//    // Update this effect
//    if (description->type == CEVISUALIZATION_PARTICLESWIRL)
//    {
//      size_t numberOfParticles = instance->numberOfParticles;
//
//      // Erase invalid particles; update particle positions
//      if (instance->nextUpdateTime < time)
//      {
//        float adjustedTimeDelta = max(description->ParticleSwirl.updatePeriod, timeDelta);
//
//        for (size_t particleIndex = 0; particleIndex < numberOfParticles;)
//        {
//          PointParticle* particle = instance->particles[particleIndex];
//
//          double timeUntilDying = particle->dyingTime - time;
//          bool dying = timeUntilDying < 0.0;
//          if (dying && (particle->pointSize <= 0.0f))
//          {
//            myPointParticlePool.release(particle);
//
//            // Swap down into this location
//            --numberOfParticles;
//            instance->particles[particleIndex] = instance->particles[numberOfParticles];
//            instance->particles[numberOfParticles] = 0;
//
//            continue;
//          }
//          else
//          {
//            // Advance the particle's features
//            float newAngle = particle->ParticleSwirl.angleFromSource + description->ParticleSwirl.orbitSpeed * adjustedTimeDelta;
//            float newHeight = particle->y + description->ParticleSwirl.risingSpeed * adjustedTimeDelta;
//            float newDistance = particle->ParticleSwirl.distanceFromSource + description->ParticleSwirl.spreadingSpeed * adjustedTimeDelta;
//
//            // Move this particle
//            particle->y = newHeight;
//            particle->x = tgtX + cosf(newAngle) * newDistance;
//            particle->z = tgtY + sinf(newAngle) * newDistance;
//            particle->ParticleSwirl.angleFromSource = newAngle;
//            particle->ParticleSwirl.distanceFromSource = newDistance;
//
//            // Shrink the particle if it is dying.  Since timeUntilDying is negative, we add the rate-scaled value (which will be negative)
//            // to the particle size.
//            if (dying)
//              particle->pointSize = description->ParticleSwirl.particleSize + description->ParticleSwirl.decayRate * timeUntilDying;
//
//            // Advance to the next particle index
//            ++particleIndex;
//          }
//        }
//
//        // Save the new number of particles
//        instance->numberOfParticles = numberOfParticles;
//
//        // Update the update time
//        instance->nextUpdateTime = time + description->ParticleSwirl.updatePeriod;
//      }
//
//      // Skip particle generation if this effect has expired or it doesn't need to be activated
//      if (effectExpired || time < instance->nextActivationTime)
//        continue;
//      else
//        instance->nextActivationTime = time + description->ParticleSwirl.spawnPeriod;
//
//      // Generate particles
//      for (int j = 0; j < description->ParticleSwirl.particlesPerPeriod &&
//                numberOfParticles < MAX_PARTICLES_PER_VISUALIZATION; ++j)
//      {
//        //PointParticle* particle = new PointParticle;
//        PointParticle* particle = myPointParticlePool.acquire();
//        CONFIRM(particle) else break;
//
//        float particleAngle = (rand()%RAND_MAX)/((float)RAND_MAX) * 2 * D3DX_PI;
//        float initialDistance = description->ParticleSwirl.initialDistance;
//
//        //float dist = fmod(rand()/1000.0f, distSrcTarget);
//        particle->x = tgtX + cosf(particleAngle) * initialDistance;
//        particle->y = description->ParticleSwirl.initialHeight;
//        particle->z = tgtY + sinf(particleAngle) * initialDistance;
//        particle->color = description->ParticleSwirl.particleColors[rand()%3];
//        particle->dyingTime = time + description->ParticleSwirl.lifetime;
//        particle->ParticleSwirl.angleFromSource = particleAngle;
//        particle->ParticleSwirl.distanceFromSource = initialDistance;
//        particle->pointSize = description->ParticleSwirl.particleSize;
//
//        // Add to particle list
//        instance->particles[numberOfParticles++] = particle;
//        //instance->particles.push_back(particle);
//      }
//
//      // Save the new number of particles
//      instance->numberOfParticles = numberOfParticles;
//
//    } else if (description->type == CEVISUALIZATION_PARTICLETRAIL)
//    {
//      size_t numberOfParticles = instance->numberOfParticles;
//
//      // Erase invalid particles; update particle positions
//      if (instance->nextUpdateTime < time)
//      {
//        float adjustedTimeDelta = max(description->ParticleTrail.updatePeriod, timeDelta);
//
//        for (size_t particleIndex = 0; particleIndex < numberOfParticles;)
//        /*for (EffectVisualizationInstance::ParticleList::iterator j = instance->particles.begin();
//          j != instance->particles.end();)*/
//        {
//          PointParticle* particle = instance->particles[particleIndex];
//          //PointParticle* particle = *j;
//          double timeUntilDying = particle->dyingTime - time;
//          bool dying = timeUntilDying < 0.0;
//          if (dying && (particle->pointSize <= 0.0f))
//          {
//            myPointParticlePool.release(particle);
//
//            // Swap down into this location
//            --numberOfParticles;
//            instance->particles[particleIndex] = instance->particles[numberOfParticles];
//            instance->particles[numberOfParticles] = 0;
//
//            continue;
//          }
//          else
//          {
//            // Move this particle
//            particle->x += particle->ParticleTrail.vx * adjustedTimeDelta;
//            particle->y += particle->ParticleTrail.vy * adjustedTimeDelta;
//            particle->z += particle->ParticleTrail.vz * adjustedTimeDelta;
//
//            // Add to gravity
//            particle->ParticleTrail.vy += description->ParticleTrail.gravityFactor * adjustedTimeDelta;
//
//            // Shrink the particle if it is dying.  Since timeUntilDying is negative, we add the rate-scaled value (which will be negative)
//            // to the particle size.
//            if (dying)
//              particle->pointSize = description->ParticleTrail.particleSize + description->ParticleTrail.decayRate * timeUntilDying;
//
//            // Advance to the next particle index
//            ++particleIndex;
//          }
//        }
//
//        // Save the new number of particles
//        instance->numberOfParticles = numberOfParticles;
//
//        // Update the update time
//        instance->nextUpdateTime = time + description->ParticleTrail.updatePeriod;
//      }
//
//      // Skip particle generation if this effect has expired or it doesn't need to be activated
//      if (effectExpired || time < instance->nextActivationTime)
//        continue;
//      else
//        instance->nextActivationTime = time + description->ParticleTrail.spawnPeriod;
//
//      // Generate particles
//      for (int j = 0; j < description->ParticleTrail.particlesPerPeriod &&
//                numberOfParticles < MAX_PARTICLES_PER_VISUALIZATION; ++j)
//      {
//        //PointParticle* particle = new PointParticle;
//        PointParticle* particle = myPointParticlePool.acquire();
//        CONFIRM(particle) else break;
//
//        float particleAngle = (rand()%RAND_MAX)/((float)RAND_MAX) * 2 * D3DX_PI;
//        float dh = description->ParticleTrail.initialHeightMax - description->ParticleTrail.initialHeightMin;
//        float initialHeight = dh * (rand()%RAND_MAX)/((float)RAND_MAX) + description->ParticleTrail.initialHeightMin;
//        float dd = description->ParticleTrail.initialDistanceMax - description->ParticleTrail.initialDistanceMin;
//        float initialDistance = dd * (rand()%RAND_MAX)/((float)RAND_MAX) + description->ParticleTrail.initialDistanceMin;
//
//        float vx = cosf(srcTgtAngle) * description->ParticleTrail.directedVelocity + cosf(particleAngle) * description->ParticleTrail.radialVelocity;
//        float vy = description->ParticleTrail.risingSpeed;
//        float vz = sinf(srcTgtAngle) * description->ParticleTrail.directedVelocity + sinf(particleAngle) * description->ParticleTrail.radialVelocity;
//
//        //float dist = fmod(rand()/1000.0f, distSrcTarget);
//        particle->x = tgtX + cosf(particleAngle) * initialDistance;
//        particle->y = description->ParticleSwirl.initialHeight;
//        particle->z = tgtY + sinf(particleAngle) * initialDistance;
//        particle->color = description->ParticleTrail.particleColors[rand()%3];
//        particle->dyingTime = time + description->ParticleTrail.lifetime;
//        particle->ParticleTrail.vx = vx;
//        particle->ParticleTrail.vy = vy;
//        particle->ParticleTrail.vz = vz;
//        particle->pointSize = description->ParticleTrail.particleSize;
//
//        // Add to particle list
//        //instance->particles.push_back(particle);
//        instance->particles[numberOfParticles++] = particle;
//      }
//
//      // Save the new number of particles
//      instance->numberOfParticles = numberOfParticles;
//    }
//  }
//}
//
//
//
//
//// Helper function to stuff a FLOAT into a DWORD argument
//inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
//
//
////------------------------------------------------------------------------------------------------
//// Name:  renderSpecialEffects
//// Desc:  
////------------------------------------------------------------------------------------------------
//bool VolucrisClient::renderSpecialEffects(float pointScalingFactor)
//{
//  CONFIRM(myD3DDevice) else return false;
//
//  // Reset the world matrix
//  D3DXMATRIXA16 mat;
//  D3DXMatrixIdentity(&mat);
//  myIsometricCamera.setWorldMatrix(myD3DDevice, &mat);
//
//  // Set up for rendering point particles
//  myD3DDevice->SetFVF(PointVertex::FVF_Flags);
//  myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//  myD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//  myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
//  myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//
//  myD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);     // Turn on point sprites
//  myD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  TRUE);     // Allow sprites to be scaled with distance
//  myD3DDevice->SetRenderState(D3DRS_POINTSIZE,   FtoDW(0.1f)); // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
//  myD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.7f));  // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
//  myD3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(128.0f));
//  myD3DDevice->SetRenderState(D3DRS_POINTSCALE_A,  FtoDW(0.0f));  // Default 1.0
//  myD3DDevice->SetRenderState(D3DRS_POINTSCALE_B,  FtoDW(0.0f));  // Default 0.0
//  myD3DDevice->SetRenderState(D3DRS_POINTSCALE_C,  FtoDW(1.0f));  // Default 0.0
//
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//
//  myD3DDevice->SetStreamSource(0, mySpecialEffectParticlesVB, 0, sizeof(PointVertex));
//
//  // Render the particles
//  for (EffectVisualizationListIterator i = myEffectVisualizations.begin(); i != myEffectVisualizations.end(); ++i)
//  {
//    EffectVisualizationInstance* instance = (*i);
//    CONFIRM(instance) else continue;
//
//    // Get the image from the effect
//    size_t imageIndex;
//    switch(instance->description->type)
//    {
//      case CEVISUALIZATION_PARTICLESWIRL: imageIndex = instance->description->ParticleSwirl.particleImageIndex; break;
//      case CEVISUALIZATION_PARTICLETRAIL: imageIndex = instance->description->ParticleTrail.particleImageIndex; break;
//      default: continue;  // dont process anyo ther type
//    }
//
//    // Set the particle texture
//    if (imageIndex < myResources.images.d3dTextureArraySize)
//      global_rendering_manager_.changeCurrentD3DStage0Texture(myResources.images.d3dTextureArray[imageIndex]);
//
//    //EffectVisualizationInstance::ParticleList::iterator end = instance->particles.end();
//    //EffectVisualizationInstance::ParticleList::iterator particleIterator = instance->particles.begin();
//    int consumedVertices = 0;
//    int particlesWritten = 0;
//    PointVertex* lockedVertices;
//    //while(particleIterator != end)
//    int particleIndex = 0;
//    int numberOfParticles = instance->numberOfParticles;
//    while(particleIndex < numberOfParticles)
//    {
//      //int particlesToWrite = instance->particles.size() - particlesWritten;
//      int particlesToWrite = numberOfParticles - particlesWritten;
//      particlesToWrite = min(particlesToWrite, SPECIAL_EFFECT_POINT_CAPACITY - consumedVertices);
//
//      HRESULT hr = mySpecialEffectParticlesVB->Lock(
//        consumedVertices * sizeof(PointVertex),
//        particlesToWrite * sizeof(PointVertex),
//        (void**)&lockedVertices,
//        consumedVertices ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
//      CONFIRM(SUCCEEDED(hr)) else return false;
//
//      // Write all of the particles
//      //for (int i = 0; i < particlesToWrite && particleIterator != end; ++particleIterator, ++i)
//      for (int i = 0; i < particlesToWrite && particleIndex < numberOfParticles; ++particleIndex, ++i)
//      {
//        //PointParticle* particle = *particleIterator;
//        PointParticle* particle = instance->particles[particleIndex];
//        CONFIRM(particle) else continue;
//
//        lockedVertices->posit = D3DXVECTOR3(particle->x, particle->y, particle->z);
//        lockedVertices->size = particle->pointSize * pointScalingFactor;
//        lockedVertices->color = particle->color;
//        ++lockedVertices;
//      }
//
//      // Add to the consumed vertices
//      mySpecialEffectParticlesVB->Unlock();
//      consumedVertices += particlesToWrite;
//      particlesWritten += particlesToWrite;
//
//      // Render
//      // TODO: batch between effect instances.  Right now it's cleared each loop no matter what
//      myD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, consumedVertices);
//      consumedVertices = 0;
//    }
//  }
//
//
//  myD3DDevice->SetFVF(D3DFVF_GEOMETRYVERTEX);
//  myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//
//  // Render the beams
//  for (EffectVisualizationListIterator i = myEffectVisualizations.begin(); i != myEffectVisualizations.end(); ++i)
//  {
//    EffectVisualizationInstance* instance = (*i);
//    CONFIRM(instance) else continue;
//
//    if (instance->description->type != CEVISUALIZATION_BEAM) continue;
//
//    float height = instance->description->Beam.height;
//    float size = instance->description->Beam.sourceTerminalImageSize;
//    float width = instance->description->Beam.beamWidth;
//
//    // Render a plane at src
//    GeometryVertex plane[] = {
//      { -0.5f, height, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
//      { +0.5f, height, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
//      { -0.5f, height, +0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
//      { +0.5f, height, +0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f }
//    };
//
//    //changeCurrentD3DStage0Texture(
//
//    Actor* src_actor = getActor(instance->srcActor);
//    float arm_radius = 1.0f;//instance->description->Beam.cast_from_hands ? sqrtf(src_actor->myDescription->sizeRadiusSq)/2.0f : 0.0f;
//    float dx = instance->tgtX - instance->srcX;
//    float dy = instance->tgtY - instance->srcY;
////    src_actor->renderAngle = atan2f(dy,dx); // force the actor to render toward the target
//    float actor_angle = src_actor->getAngle();
//    float dist_to_target = sqrtf(dx*dx+dy*dy);
//    float hands_x = instance->srcX + cosf(actor_angle) * arm_radius;
//    float hands_y = instance->srcY + sinf(actor_angle) * arm_radius;
//    float hands_dx = instance->tgtX - hands_x;
//    float hands_dy = instance->tgtY - hands_y;
//    float dist_from_hands = sqrtf(hands_dx*hands_dx+hands_dy*hands_dy);
//    float angle_to_target = atan2f(hands_dy,hands_dx);
//
//    // Set the ends texture
//    size_t imageIndex = instance->description->Beam.sourceImageIndex;
//    if (imageIndex < myResources.images.d3dTextureArraySize)
//      global_rendering_manager_.changeCurrentD3DStage0Texture(myResources.images.d3dTextureArray[imageIndex]);
//
//    // Source
//    D3DXMATRIX scaling, mat;
//    D3DXMatrixScaling(&scaling, size, 1.0f, size);
//    D3DXMatrixTranslation(&mat, hands_x, 0.0f, hands_y);
//    D3DXMatrixMultiply(&mat, &scaling, &mat);
//    myIsometricCamera.setWorldMatrix(myD3DDevice, &mat);
//    myD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, plane, sizeof(GeometryVertex));
//
//    imageIndex = instance->description->Beam.terminalImageIndex;
//    if (imageIndex < myResources.images.d3dTextureArraySize)
//      global_rendering_manager_.changeCurrentD3DStage0Texture(myResources.images.d3dTextureArray[imageIndex]);
//
//    // Target
//    D3DXMatrixTranslation(&mat, instance->tgtX, 0.0f, instance->tgtY);
//    D3DXMatrixMultiply(&mat, &scaling, &mat);
//    myIsometricCamera.setWorldMatrix(myD3DDevice, &mat);
//    myD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, plane, sizeof(GeometryVertex));
//
//    imageIndex = instance->description->Beam.beamImageIndex;
//    if (imageIndex < myResources.images.d3dTextureArraySize)
//      global_rendering_manager_.changeCurrentD3DStage0Texture(myResources.images.d3dTextureArray[imageIndex]);
//
//    // Beam
//    //float dx = instance->tgtX - instance->srcX;
//    //float dy = instance->tgtY - instance->srcY;
//    //float dist = sqrtf(dx*dx+dy*dy);
//    D3DXMatrixTranslation(&mat, 0.5f, 0.0f, 0.0f);
//    D3DXMatrixScaling(&scaling, dist_from_hands, 1.0f, width);
//    D3DXMatrixMultiply(&mat, &mat, &scaling); // move into +x, then scale so that one end is at 0 and the other at dist
//    D3DXMATRIX rotation, trans;
//    D3DXMatrixRotationY(&rotation, -angle_to_target);
//    D3DXMatrixTranslation(&trans, hands_x, 0.0f, hands_y);
//    D3DXMatrixMultiply(&mat, &mat, &rotation);
//    D3DXMatrixMultiply(&mat, &mat, &trans);
//    myIsometricCamera.setWorldMatrix(myD3DDevice, &mat);
//    myD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, plane, sizeof(GeometryVertex));
//  }
//
//  // Reset
//  myD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
//  myD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
//  myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//  myD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//  myD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
//
//  return true;
//}