//
//  ProjectionScene.h
//  SvvimGram
//
//  Created by Matthew Owen on 1/6/14.
//
//

#ifndef SvvimGram_ProjectionScene_h
#define SvvimGram_ProjectionScene_h

#include "cinder/ObjLoader.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Light.h"

#include "svvim.h"
#include "SvvimScene.h"
#include "SodaCan.h"
#include <time.h>

class ProjectionScene : public SvvimScene {
public:
  
  // ...
  ProjectionScene () {
    mParams["t"] = 0.f;
  }
  
  // ...
  void setup () {
    mProjector = make_shared<gl::Light>(gl::Light::DIRECTIONAL, 0);
    
    ObjLoader loader((DataSourceRef)loadResource("Soda-Can.obj"));
    loader.load(&mMesh);
    mVbo = gl::VboMesh(mMesh);
    
    mCam.eye  = Vec3f(10, 0, 0);
    mCam.dest = Vec3f(0, 0, 0);
    mCam.up   = Vec3f(0, 1, 0);
    
    try {
      mProjShader = gl::GlslProg(loadResource("project.vert"), loadResource("project.frag"));
    }
    catch (const std::exception & e) {
      app::console() << e.what() << "\n";
    }
    
    // Texture
    mTexture = loadImage(loadResource("pizza.jpg"));
    
    setupCans();
  }
  
  // ...
  void update () {
    mProjector->setAmbient(   Color(.8, .8, .8)   );
    mProjector->setDiffuse(   Color(.1, .1, .1)   );
    mProjector->setSpecular(  Color(.7, .7, .7)   );
    
    updateCans();
  }
  
  // ...
  void draw () {
    gl::pushMatrices();
    {
      float t = mParams.count("t") ? mParams["t"] : 0.f;
      t = 2.f * M_PI * fmod(t, 1.f);
      float x = 13.f * sin(t);
      float z = 13.1f * cos(t);

      mCam.point.lookAt(  Vec3f(x, 0, z), mCam.dest, mCam.up);
      mProjector->lookAt( Vec3f(17, 0, 3), Vec3f(0, 0, 0));
      mProjector->update(mCam.point);

      glEnable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
      
      gl::enableDepthWrite();
      gl::enableDepthRead();
      
      gl::clear(Color(1, 1, 1));
      
      gl::setMatrices(mCam.point);
      
      if (mProjShader) {
        Matrix44f shadows[2];
        shadows[0] = mProjector->getShadowTransformationMatrix(mCam.point);
        shadows[1] = mProjector->getShadowTransformationMatrix(mCam.point);
        
        mTexture.bind(0);
        mProjShader.bind();
        mProjShader.uniform("proj", 0);
        mProjShader.uniform("shadowMatrices", shadows, 2);
        
        float no_mat[4]       = { 0.0f, 0.0f, 0.0f, 1.0f };
        float mat_diffuse[4]  = { 0.5f, 0.5f, 0.5f, 1.0f };
        float mat_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(  GL_FRONT, GL_AMBIENT,   no_mat);
        glMaterialfv(  GL_FRONT, GL_DIFFUSE,   mat_diffuse);
        glMaterialfv(  GL_FRONT, GL_SPECULAR,  mat_specular);
        glMaterialf (  GL_FRONT, GL_SHININESS, 4.f);
        glMaterialfv(  GL_FRONT, GL_EMISSION,  no_mat);
        drawCans();
        mProjShader.unbind();
      }
      
      gl::color(1, 0, 1);
      // gl::drawFrustum(mProjector->getShadowCamera());
    }
    
    gl::popMatrices();
  }
  
private:
  
  // Can updating
  void updateCans();
  void setupCans();
  void drawCans();
  
  // The rest
  TriMesh       mMesh;
  gl::VboMesh   mVbo;
  gl::GlslProg  mProjShader;
  shared_ptr <gl::Light> mProjector;
  
  struct {
    CameraPersp point;
    Vec3f eye;
    Vec3f dest;
    Vec3f up;
  } mCam;
  
  std::vector <svv::SodaCan> mObjectList;
  
  gl::Texture mTexture;
  
  svvim::TimeCounter mTimer;
};

/**
 
  IMPLEMENTATION!
 
 */

#define CAN_COUNT 200
void ProjectionScene::setupCans() {
  mObjectList.resize(CAN_COUNT);
  for (int i = 0; i < CAN_COUNT; ++i) {
    svvim::SodaCan object;
    object.mesh     = gl::VboMesh::create(mMesh);
    object.position = Vec3f(svv::random(-7, 7),   svv::random(-7, 7),   svv::random(-4, 4));
    object.r0       = Vec3f(svv::random(-80, 80), svv::random(-80, 80), svv::random(-80, 80));
    object.r1       = Vec3f(svv::random(-80, 80), svv::random(-80, 80), svv::random(-80, 80));
    mObjectList[i]  = object;
  }
}

void ProjectionScene::updateCans() {
  /*
  for (int i = 0; i < mObjectList.size(); ++i) {
    mObjectList[i].r1.x += 1.f;
    mObjectList[i].r1.y += 0.f;
    mObjectList[i].r1.z += 0.f;
  } */
}

void ProjectionScene::drawCans() {
  float t = mParams.count("t") ? mParams["t"] : 0.f;
  t = fmod(t, 1.f);
  for (int i = 0; i < mObjectList.size(); ++i) {
    drawSodaCan(mObjectList[i], t);
  }
}


#endif
