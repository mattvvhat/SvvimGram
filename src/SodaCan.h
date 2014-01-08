//
//  SodaCan.h
//  SvvimGram
//
//  Created by Matthew Owen on 1/7/14.
//
//

#ifndef SvvimGram_SodaCan_h
#define SvvimGram_SodaCan_h

namespace svvim {
  struct SodaCan {
    gl::VboMeshRef mesh;
    Vec3f position;
    Vec3f r0;
    Vec3f r1;
  };
  
  void drawSodaCan(const SodaCan & obj, float t = 0.f) {
    Vec3f rotation = obj.r0 + 360 * t * Vec3f(1, -1, 1);
    gl::pushMatrices();
    gl::translate(obj.position);
    gl::rotate(rotation);
    gl::draw(obj.mesh);
    gl::popMatrices();
  }
};


#endif
