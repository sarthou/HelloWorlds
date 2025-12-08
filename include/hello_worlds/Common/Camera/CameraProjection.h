#ifndef HWS_CAMERAPROJECTION_H
#define HWS_CAMERAPROJECTION_H

namespace hws {
  /**
   * Its best for you to look online about the differences between perspective and orthographic.

   * But generally, as a rule of thumb:
   * - Use ePERSPECTIVE if you need 3D
   * - Use eORTHOGRAPHIC if you need 2D
   */
  enum class CameraProjection_e
  {
    perspective,
    orthographic
  };
} // namespace hws

#endif // HWS_CAMERAPROJECTION_H