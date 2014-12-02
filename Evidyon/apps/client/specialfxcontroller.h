#ifndef __SPECIALFXCONTROLLER_H__
#define __SPECIALFXCONTROLLER_H__


class IsometricCamera;


namespace Evidyon {

class GlobalRenderingManager;
class GlobalSpecialFXManager;
class Actor;

class SpecialFXController {
public:

  // Use this to remove references to bindings and free allocated memory
  virtual void destroy(GlobalSpecialFXManager* manager);

  // Advance this FX controller.  Returns 'true' if the controller should
  // be erased.
  virtual bool update(double time,
            double time_since_last_update,
            GlobalSpecialFXManager* manager);

  // Render geometry using the specified texture ID during this method.  It
  // will be drawn automatically by the FX manager.
  virtual bool render(int texture_id,
            IsometricCamera* camera,
            GlobalSpecialFXManager* manager);

  // Can render geometry or particles during this call; however, both will use
  // the current image.
  virtual bool renderFX(int image_id,
            IsometricCamera* camera,
            GlobalSpecialFXManager* manager);
};



}


#endif