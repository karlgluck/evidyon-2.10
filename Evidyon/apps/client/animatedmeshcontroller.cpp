//------------------------------------------------------------------------------------------------
// File:  animatedmeshcontroller.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"



//------------------------------------------------------------------------------------------------
// Name: AnimatedMeshController
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
AnimatedMeshController::AnimatedMeshController()
{
  zero();
}



//------------------------------------------------------------------------------------------------
// Name: create
// Desc: Initializes this controller to handle animations for the given skinned mesh
//------------------------------------------------------------------------------------------------
bool AnimatedMeshController::create(GameFileDataSkinnedMesh* skinnedMesh)
{
  // Make sure this class doesn't already have something in it
  destroy();

  // Save settings
  mySkinnedMesh = skinnedMesh;

  // Create the frame matrix array
  myFrameMatrices = new D3DXMATRIX[skinnedMesh->numberOfFrames];

  D3DXMATRIX matrix;
  D3DXMatrixIdentity(&matrix);

  // Initialize the frame matrix array
  for (DWORD m = 0; m < mySkinnedMesh->numberOfFrames; ++m)
    myFrameMatrices[m] = matrix;

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: destroy
// Desc: Frees memory used by this class
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::destroy()
{
  SAFE_DELETE_ARRAY(myFrameMatrices);
  zero();
}



//------------------------------------------------------------------------------------------------
// Name: isInitialized
// Desc: Determines whether or not this animation controller is valid
//------------------------------------------------------------------------------------------------
bool AnimatedMeshController::isInitialized() const
{
  return myFrameMatrices != 0;
}


void calculateFrameBlendingFactor(UnseenSkeletalAnimation* animation,
                  double currentTime,
                  bool loop,
                  double* blendFactor,
                  DWORD* lowerKeyframeIndex,
                  DWORD* upperKeyframeIndex) {
  DWORD number_of_keyframes = animation->info.numberOfKeyframes;
  DWORD highest_keyframe_index = number_of_keyframes - 1;
  DWORD time_tick_index = (DWORD)(currentTime / animation->info.secondsPerTick);
  DWORD tick_length = animation->keyframes[highest_keyframe_index].tick;


  DWORD ticks_per_frame = animation->info.averageTicksPerKeyframe;
  if (loop) time_tick_index %= tick_length + ticks_per_frame;
  double internal_blend_factor;
  DWORD lower, upper;
  if (time_tick_index > tick_length) {
    lower = highest_keyframe_index;
    if (loop) {
      upper = 0;
      internal_blend_factor = (time_tick_index - tick_length) / (double)ticks_per_frame;
    } else {
      upper = highest_keyframe_index;
      internal_blend_factor = 0.0;
    }
  } else {
    --highest_keyframe_index;
    lower = time_tick_index / ticks_per_frame;
    lower %= number_of_keyframes;
    while (animation->keyframes[lower].tick > time_tick_index &&
         lower < highest_keyframe_index) {
      ++lower;
    }
    upper = lower + 1;
    upper %= number_of_keyframes;
    DWORD frame_duration = animation->keyframes[upper].tick - animation->keyframes[lower].tick;
    DWORD frame_index = time_tick_index - animation->keyframes[lower].tick;
    frame_index %= frame_duration; // this should never have an effect, but is a safety measure
    internal_blend_factor = frame_index / (double)frame_duration;
  }
  internal_blend_factor = min(internal_blend_factor, 1.0);
  internal_blend_factor = max(internal_blend_factor, 0.0);
  *lowerKeyframeIndex = lower;
  *upperKeyframeIndex = upper;
  *blendFactor = internal_blend_factor;
}

//------------------------------------------------------------------------------------------------
// Name: advance
// Desc: Updates this mesh controller
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::advance(const D3DXMATRIX* rootMatrix,
                                     double currentFrameTime,
                                     double timeSinceLastUpdate)
{
  if (!mySkinnedMesh) return;

  // Advance the weighting value
  if (myPrimaryBaseAnimationWeight < 1.0f) myPrimaryBaseAnimationWeight += timeSinceLastUpdate * 3.0f;
  if (myPrimaryBaseAnimationWeight > 1.0f)
  {
    myPrimaryBaseAnimationWeight = 1.0f;
    mySecondaryBaseAnimationState.animation = 0;
  }

  // Advance the animation time indices
  myPrimaryBaseAnimationState.time += myPrimaryBaseAnimationState.speed * timeSinceLastUpdate;
  mySecondaryBaseAnimationState.time += mySecondaryBaseAnimationState.speed * timeSinceLastUpdate;
  myActionAnimationState.time += myActionAnimationState.speed * timeSinceLastUpdate;

  // Get the animation blending factors
  bool blendActionAnimation = myActionAnimationState.animation != 0;
  bool blendBaseAnimation[2] = { myPrimaryBaseAnimationState.animation != 0, mySecondaryBaseAnimationState.animation != 0 };

  // Make sure the animations exist
  if (!blendActionAnimation && !blendBaseAnimation[0] && !blendBaseAnimation[1]) return;

  struct { double factor; unsigned long lower, upper; } actionAnimation, baseAnimation[2];
  if (blendActionAnimation) {
    calculateFrameBlendingFactor(myActionAnimationState.animation,
                   myActionAnimationState.time,
                   false,
                   &actionAnimation.factor,
                   &actionAnimation.lower,
                   &actionAnimation.upper);
  }
  if (blendBaseAnimation[0]) {
    calculateFrameBlendingFactor(myPrimaryBaseAnimationState.animation,
                   myPrimaryBaseAnimationState.time,
                   true,
                   &baseAnimation[0].factor,
                   &baseAnimation[0].lower,
                   &baseAnimation[0].upper);
  }
  if (blendBaseAnimation[1]) {
    calculateFrameBlendingFactor(mySecondaryBaseAnimationState.animation,
                   mySecondaryBaseAnimationState.time,
                   true,
                   &baseAnimation[1].factor,
                   &baseAnimation[1].lower,
                   &baseAnimation[1].upper);
  }

  // Drop off the action animation
  bool blendActionAnimationWeighted = false;
  bool blendMax = false;
  if (blendActionAnimation) {
    
    if (actionAnimation.lower == actionAnimation.upper) {
      myActionAnimationWeight *= 0.9f;
      blendActionAnimationWeighted = myActionAnimationWeight > 0.01f;

      if (false == blendActionAnimationWeighted) {
        myActionAnimationWeight = 0.0f;
        myActionAnimationState.animation = NULL;
        myActionAnimationState.speed = 0.0f;
        blendActionAnimation = false;
      }
    } else {
      blendMax = actionAnimation.lower == actionAnimation.upper;
      blendActionAnimationWeighted = blendMax;
      if (myActionAnimationWeight < 1.0f && myActionAnimationWeight > 0.0f) {
        blendActionAnimationWeighted = true;
        blendMax = false;
        myActionAnimationWeight *= 0.9f;
      }
    }
  }
/*
  myActionAnimationWeight *= 0.9f;
  if (myActionAnimationWeight < 0.001f) {
    blendActionAnimation = false;
    blendActionAnimationWeighted = false;
    myActionAnimationWeight = 0.0f;
    myActionAnimationState.animation = NULL;
  }*/

  // Set the transform for each of the frames
  for (DWORD m = 0; m < mySkinnedMesh->numberOfFrames; ++m)
  {
    if (mySkinnedMesh->frameHierarchy[m].parentFrameIndex < mySkinnedMesh->numberOfFrames)
    {
      DWORD actionAnimatedOffset = blendActionAnimation ? myActionAnimationState.animation->frameIndexToAnimatedFrameTransformTable[m] : 0xFFFFFFFF;
      DWORD baseAnimatedOffset[2] = 
      {
        blendBaseAnimation[0] ? myPrimaryBaseAnimationState.animation->frameIndexToAnimatedFrameTransformTable[m] : 0xFFFFFFFF,
        blendBaseAnimation[1] ? mySecondaryBaseAnimationState.animation->frameIndexToAnimatedFrameTransformTable[m] : 0xFFFFFFFF
      };

      if (actionAnimatedOffset != 0xFFFFFFFF && !blendActionAnimationWeighted)
      {
        LPD3DXMATRIX lowerMatrix = &myActionAnimationState.animation->keyframes[actionAnimation.lower].animatedFrameTransforms[actionAnimatedOffset];
        LPD3DXMATRIX upperMatrix = &myActionAnimationState.animation->keyframes[actionAnimation.upper].animatedFrameTransforms[actionAnimatedOffset];

        //D3DXMatrixDecompose(&scale[0], &rotate, &translate, lowerMatrix

        // Interpolate
        D3DXMATRIXA16 interpolated;
        interpolated = upperMatrix->operator -(*lowerMatrix);
        interpolated *= actionAnimation.factor;
        interpolated += *lowerMatrix;

        // Set the matrix
        D3DXMatrixMultiply(&myFrameMatrices[m], &interpolated, &myFrameMatrices[mySkinnedMesh->frameHierarchy[m].parentFrameIndex]);
      }
      else if (baseAnimatedOffset[0] != 0xFFFFFFFF)
      {
        D3DXMATRIXA16 interpolated;
        {
        LPD3DXMATRIX lowerMatrix = &myPrimaryBaseAnimationState.animation->keyframes[baseAnimation[0].lower].animatedFrameTransforms[baseAnimatedOffset[0]];
        LPD3DXMATRIX upperMatrix = &myPrimaryBaseAnimationState.animation->keyframes[baseAnimation[0].upper].animatedFrameTransforms[baseAnimatedOffset[0]];

        // Interpolate
        interpolated = upperMatrix->operator -(*lowerMatrix);
        interpolated *= baseAnimation[0].factor;
        interpolated += *lowerMatrix;
        }

        // If we have a secondary matrix, blend it in
        if (baseAnimatedOffset[1] != 0xFFFFFFFF)
        {
          LPD3DXMATRIX lowerMatrix2 = &mySecondaryBaseAnimationState.animation->keyframes[baseAnimation[1].lower].animatedFrameTransforms[baseAnimatedOffset[1]];
          LPD3DXMATRIX upperMatrix2 = &mySecondaryBaseAnimationState.animation->keyframes[baseAnimation[1].upper].animatedFrameTransforms[baseAnimatedOffset[1]];

          // Interpolate
          D3DXMATRIXA16 interpolated2;
          interpolated2 = upperMatrix2->operator -(*lowerMatrix2);
          interpolated2 *= baseAnimation[1].factor;
          interpolated2 += *lowerMatrix2;

          // Blend between these entries
          D3DXMATRIXA16 interpolated3;
          interpolated3 = interpolated - interpolated2;
          interpolated3 *= myPrimaryBaseAnimationWeight;
          interpolated3 += interpolated2;
          interpolated = interpolated3;
        }

        // If we're blending a weighted action animation, blend it in
        if (blendActionAnimationWeighted && actionAnimatedOffset != 0xFFFFFFFF)
        {
          LPD3DXMATRIX actionFinalPoseMatrix = &myActionAnimationState.animation->keyframes[actionAnimation.lower].animatedFrameTransforms[actionAnimatedOffset];


          // Blend between these entries
          D3DXMATRIXA16 interpolated3;
          interpolated3 = actionFinalPoseMatrix->operator -(interpolated);
          //interpolated3 *= blendMax ? (cosf(myActionAnimationState.time)*.45f+.65f) : myActionAnimationWeight;
          interpolated3 *= myActionAnimationWeight;
          interpolated += interpolated3;
        }

        // Set the matrix
        D3DXMatrixMultiply(&myFrameMatrices[m], &interpolated, &myFrameMatrices[mySkinnedMesh->frameHierarchy[m].parentFrameIndex]);
      }
      else
      {
        D3DXMatrixMultiply(&myFrameMatrices[m], &mySkinnedMesh->frameHierarchy[m].offset, &myFrameMatrices[mySkinnedMesh->frameHierarchy[m].parentFrameIndex]);
      }

    }
    else
      D3DXMatrixMultiply(&myFrameMatrices[m], rootMatrix, &mySkinnedMesh->frameHierarchy[m].offset);
  }
}


//------------------------------------------------------------------------------------------------
// Name: setBoneMatrices
// Desc: Sets the mesh's bone matrices into the device using the camera
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::setBoneMatrices(LPDIRECT3DDEVICE9 d3dDevice, IsometricCamera* camera)
{
  for (DWORD i = 0; i < mySkinnedMesh->numberOfBones; ++i)
  {
    D3DXMATRIX combined;
    D3DXMatrixMultiply(&combined, &mySkinnedMesh->skeleton[i].inverseOffset, &myFrameMatrices[mySkinnedMesh->skeleton[i].frameIndex]);
    camera->setWorldMatrix(d3dDevice, &combined, i);
  }
}



//------------------------------------------------------------------------------------------------
// Name: changeBaseAnimation
// Desc: Sets the base animation.  The base animation loops, and should be something
//     like idling, walking, running or sleeping.
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::changeBaseAnimation(UnseenSkeletalAnimation* animation, float speed)
{
  // Make sure the animation is valid
  if (APP_ERROR(!mySkinnedMesh || animation && (animation->info.numberOfSkeletalFrames != mySkinnedMesh->numberOfFrames))
      ("Invalid animation for this skinned mesh; different number of skeletal frames"))
    return;

  // If this animation is already set, don't restart it
  if (animation != myPrimaryBaseAnimationState.animation) {
    //DEBUG_INFO("Changed");

    // Copy the current primary into the secondary
    memcpy(&mySecondaryBaseAnimationState, &myPrimaryBaseAnimationState, sizeof(AnimationState));

    // Blend into the new primary
    myPrimaryBaseAnimationWeight = 0.0f;

    // Set up the new primary base animation
    myPrimaryBaseAnimationState.animation = animation;
    myPrimaryBaseAnimationState.speed = speed;
    myPrimaryBaseAnimationState.time = 0.0; // todo: randomize this
  } else {
    myPrimaryBaseAnimationState.speed = speed;
  }

}



//------------------------------------------------------------------------------------------------
// Name: changeBaseAnimation
// Desc: Sets the base animation to be at a given time index with 0 speed.  Usually this is only invoked for death.
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::changeBasePose(UnseenSkeletalAnimation* animation, float posePercent)
{
  float time = 0.0f;

  if (animation) time = (animation->info.tickLength * animation->info.secondsPerTick) * posePercent;

  myPrimaryBaseAnimationWeight = 1.0f;
  myPrimaryBaseAnimationState.animation = animation;
  myPrimaryBaseAnimationState.speed = 0.0f;
  myPrimaryBaseAnimationState.time = time;
}



//------------------------------------------------------------------------------------------------
// Name: runAnimationOnce
// Desc: Executes a specific action animation.  This is intended to be used for things
//     like attacking, spells, emotes, etc.
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::runAnimationOnce(UnseenSkeletalAnimation* animation, float speed)
{
  // Make sure the animation is valid
  if (APP_ERROR(!mySkinnedMesh || (animation && animation->info.numberOfSkeletalFrames != mySkinnedMesh->numberOfFrames))
      ("Invalid animation for this skinned mesh; different number of skeletal frames"))
    return;

  if (!animation) {
    if (myActionAnimationWeight == 1.0f)
      myActionAnimationWeight = 0.99f;
  } else {
    myActionAnimationState.time = 0.0;
    myActionAnimationState.animation = animation;
    myActionAnimationState.speed = speed;
    myActionAnimationWeight = 1.0f;
  }
}




//------------------------------------------------------------------------------------------------
// Name: getBoneMatrix
// Desc: 
//------------------------------------------------------------------------------------------------
const D3DXMATRIX* AnimatedMeshController::getBoneMatrix(size_t boneIndex) {
  return &myFrameMatrices[mySkinnedMesh->skeleton[boneIndex].frameIndex];
}




//------------------------------------------------------------------------------------------------
// Name: zero
// Desc: Resets this class
//------------------------------------------------------------------------------------------------
void AnimatedMeshController::zero() {
  mySkinnedMesh = 0;
  myFrameMatrices = 0;
  ZeroMemory(&myPrimaryBaseAnimationState, sizeof(AnimationState));
  ZeroMemory(&mySecondaryBaseAnimationState, sizeof(AnimationState));
  myPrimaryBaseAnimationWeight = 1.0f;
  myActionAnimationWeight = 0.0f;
  ZeroMemory(&myActionAnimationState, sizeof(AnimationState));
}

