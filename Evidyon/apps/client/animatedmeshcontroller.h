//------------------------------------------------------------------------------------------------
// File:  animatedmeshcontroller.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __ANIMATEDMESHCONTROLLER_H__
#define __ANIMATEDMESHCONTROLLER_H__


struct UnseenSkeletalAnimation;
struct D3DXMATRIX;
struct GameFileDataSkinnedMesh;
struct IDirect3DDevice9;
typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9;
class IsometricCamera;

/**
 * This class is used to manage animations
 */
class AnimatedMeshController {
  /**
   * Stores information about an animation's state
   */
  struct AnimationState
  {
    UnseenSkeletalAnimation* animation;
    double time;
    float speed;
  };

public:

  /**
   * Initializes this class
   */
  AnimatedMeshController();

  /**
   * Initializes this controller to handle animations for the given skinned mesh
   */
  // TODO: why isn't this const?
  bool create(GameFileDataSkinnedMesh* skinnedMesh);

  /**
   * Frees memory used by this class
   */
  void destroy();

  /**
   * Determines whether or not this animation controller is valid
   */
  bool isInitialized() const;

  /**
   * Updates this mesh controller
   */
  void advance(const D3DXMATRIX* rootMatrix, double currentFrameTime, double timeSinceLastUpdate);

  /**
   * Sets the mesh's bone matrices into the device using the camera
   */
  void setBoneMatrices(LPDIRECT3DDEVICE9 d3dDevice, IsometricCamera* camera);

  /**
   * Sets the base animation.  The base animation loops, and should be something
   * like idling, walking, running or sleeping.
   */
  // todo: why not const?
  void changeBaseAnimation(UnseenSkeletalAnimation* animation, float speed);

  /**
   * Sets the base animation to be at a given time index with 0 speed.  Usually this is only invoked for death.
   */
  void changeBasePose(UnseenSkeletalAnimation* animation, float posePercent);

  /**
   * Executes a specific action animation.  This is intended to be used for things
   * like attacking, spells, emotes, etc.
   */
  void runAnimationOnce(UnseenSkeletalAnimation* animation, float speed);

  /**
   *
   */
  const D3DXMATRIX* getBoneMatrix(size_t boneIndex);


protected:

  /**
   * Resets this class
   */
  void zero();


protected:

  /// The mesh that this animation controller manages
  GameFileDataSkinnedMesh* mySkinnedMesh;

  /// The set of matrices used to position this mesh
  D3DXMATRIX* myFrameMatrices;

  /// The current base animations
  AnimationState myPrimaryBaseAnimationState;
  AnimationState mySecondaryBaseAnimationState;

  /// The primary weight defines the amount of weight placed on the
  /// primary animation.  The secondary animation has 1.0 - this value
  /// as its weight.  During each advance(), this value is pushed
  /// toward 1.0 so that the secondary animation always goes away.
  float myPrimaryBaseAnimationWeight;

  /// The action animation weight.  This is 1.0 while the action
  /// animation is running, then drops off to 0.0 when it finishes.
  float myActionAnimationWeight;

  /// This animation gets full priority to run a single time
  AnimationState myActionAnimationState;

};



#endif