#ifndef HWS_CAMERAVIEW_H
#define HWS_CAMERAVIEW_H

namespace hws {
  /**
   * regular_view Will render the world with all the colors, textures and all the fancy visual details.
   * segmented_view Will randomly assign a color to every model & disable their textures.
   */
  enum class CameraView_e
  {
    regular_view,
    segmented_view
  };
} // namespace hws

#endif // HWS_CAMERAVIEW_H