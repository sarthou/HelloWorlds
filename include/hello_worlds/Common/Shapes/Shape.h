#ifndef HWS_COMMON_SHAPE_H
#define HWS_COMMON_SHAPE_H

#include <variant>

#include "hello_worlds/Common/Shapes/ShapeBox.h"
#include "hello_worlds/Common/Shapes/ShapeCapsule.h"
#include "hello_worlds/Common/Shapes/ShapeCustomMesh.h"
#include "hello_worlds/Common/Shapes/ShapeCylinder.h"
#include "hello_worlds/Common/Shapes/ShapeDummy.h"
#include "hello_worlds/Common/Shapes/ShapeSkybox.h"
#include "hello_worlds/Common/Shapes/ShapeSphere.h"

namespace hws {
  using Shape = std::variant<
    ShapeBox,
    ShapeCapsule,
    ShapeCustomMesh,
    ShapeCylinder,
    ShapeDummy,
    // ShapeSkybox,
    ShapeSphere>;
}

#endif // HWS_COMMON_SHAPE_H
