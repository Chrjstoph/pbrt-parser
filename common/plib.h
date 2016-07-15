// ======================================================================== //
// Copyright 2015 Ingo Wald
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

// embree
#include "embree-common/sys/platform.h"
#include "embree-common/sys/ref.h"
#include "embree-common/sys/filename.h"
#include "embree-common/math/vec3.h"
#include "embree-common/math/affinespace.h"

namespace plib {

  typedef embree::RefCount RefCounted;
  using embree::Ref;
  using embree::FileName;

  typedef embree::Vec2f vec2f;
  typedef embree::Vec3i vec3i;
  typedef embree::Vec3f vec3f;
  typedef embree::AffineSpace3f affine3f;
}
