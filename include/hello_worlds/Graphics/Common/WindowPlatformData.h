#ifndef HWS_GRAPHICS_COMMON_WINDOWPLATFORMDATA_H
#define HWS_GRAPHICS_COMMON_WINDOWPLATFORMDATA_H

namespace hws {
  class WindowPlatformData
  {
  public:
    void* native_display_type_;
    void* native_window_handle_;
  };
} // namespace hws

#endif // HWS_GRAPHICS_COMMON_WINDOWPLATFORMDATA_H
