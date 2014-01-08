//
//  InstagramProjectionScene.h
//  SvvimGram
//
//  Created by Matthew Owen on 1/7/14.
//  Based on ProjectionScene
//

#ifndef SvvimGram_InstagramProjectionScene_h
#define SvvimGram_InstagramProjectionScene_h

#include "cinder/ObjLoader.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Light.h"

#include "cinder/qtime/MovieWriter.h"

#include "svvim.h"
#include "SvvimScene.h"
#include "SodaCan.h"
#include "InstagramStream.h"

class InstagramProjectionScene : public SvvimScene {
public:
  
  // ...
  InstagramProjectionScene () {
    mParams["t"] = 0.f;
  }
  
  // ...
  void setup () {
    gl::Fbo::Format format;
    mRenderFbo = gl::Fbo(1000, 500, format);
    
    mLastUpdateTime = 0.f;;
    
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
    mTexture    = loadImage(loadResource("pizza.jpg"));
    mCurTexture = mTexture;
    
    setupCans();
    setupInstaStream();
  }
  
  // ...
  void update () {
    mProjector->setAmbient(   Color(0, 0, 0)   );
    mProjector->setDiffuse(   Color(.1, .1, .1)   );
    mProjector->setSpecular(  Color(.7, .7, .7)   );
    
    updateCans();
    updateInstaStream();
  }
  
  // RENDER
  gl::Texture render() {
    mRenderFbo.bindFramebuffer();
    gl::clear(Color(.8, .8, .8));
    draw();
    mRenderFbo.unbindFramebuffer();
    return mRenderFbo.getTexture();
  }
  
  // ...
  void draw () {
    gl::pushMatrices();
    {
      float t = mParams["t"] / 12.f;
      float t1 = t;
      float t2 = t + M_PI;
      
      Vec3d proj  = Vec3f( 11.f * cos(t1), 3,  5.f * sin(t1) );
      Vec3f eye   = Vec3f( 30.f * cos(t2), 3,  30.f * sin(t2) );

      mCam.point.lookAt(  eye,  mCam.dest, mCam.up);
      mProjector->lookAt( proj, mCam.dest);
      mProjector->update(mCam.point);
      
      glEnable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
      
      gl::enableDepthWrite();
      gl::enableDepthRead();
      
      gl::clear(Color(1, 1, 1));
      
      gl::setMatrices(mCam.point);
      
      if (mProjShader && mCurTexture) {
        Matrix44f shadows[2];
        shadows[0] = mProjector->getShadowTransformationMatrix(mCam.point);
        shadows[1] = mProjector->getShadowTransformationMatrix(mCam.point);
       
        mCurTexture.bind(0);
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
      
      // gl::color(1, 0, 1);
      // gl::drawFrustum(mProjector->getShadowCamera());
    }
    
    gl::popMatrices();
  }
  
private:
  
  // Can updating
  void setupCans();
  void updateCans();
  void drawCans();
  
  // InstaStream
  void setupInstaStream();
  void updateInstaStream();
  
  // ...
  void loadNew () {
    
    if (mSvvimStream && mSvvimStream->isConnected() && mSvvimStream->hasInstagramAvailable()) {
      mResInstagram = mSvvimStream->getNextInstagram();
      
      if (mResInstagram.hasVideoUrl()) {
        mInstaMovieLoader = qtime::MovieLoader(Url(mResInstagram.getVideoUrl()));
        
        app::console() << "Loading: " << mResInstagram.getVideoUrl() << "\n";
      }
    }
    
    else if (mInstaStream && mInstaStream->isConnected() && mInstaStream->hasInstagramAvailable()) {
      mResInstagram = mInstaStream->getNextInstagram();
      
      if (mResInstagram.hasVideoUrl()) {
        mInstaMovieLoader = qtime::MovieLoader(Url(mResInstagram.getVideoUrl()));
        
        app::console() << "Loading: " << mResInstagram.getVideoUrl() << "\n";
      }
      
    }
    else {
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
      app::console() << "No content available...\n";
    }
  }
  
  
  bool isReady () {
    float t = mParams["t"];

    if (t - mLastUpdateTime > 10.f) {

      mLastUpdateTime = t;
      return true;
    }
    
    return false;
  }
  
  Url mVideoUrl;
  qtime::MovieSurface mCurMovieSurface;
  ci::gl::Texture     mCurTexture;
  Instagram                   mResInstagram;
  shared_ptr<InstagramStream> mInstaStream;
  shared_ptr<InstagramStream> mSvvimStream;
  qtime::MovieLoader          mInstaMovieLoader;
  
  float mLastUpdateTime;
  
  // ------------------------------
  
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
  
  std::vector <svvim::SodaCan> mObjectList;
  
  gl::Texture mTexture;
  
  svvim::TimeCounter mTimer;
};

/**
 
 IMPLEMENTATION!
 
 */

void InstagramProjectionScene::setupInstaStream () {
  mSvvimStream = make_shared<InstagramStream>("video", "f7aa3779f33741b182e33cc28b40c474");
  mInstaStream = make_shared<InstagramStream>("video", "f7aa3779f33741b182e33cc28b40c474");
}

#define CAN_COUNT 250
void InstagramProjectionScene::setupCans() {
  mObjectList.resize(CAN_COUNT);
  for (int i = 0; i < CAN_COUNT; ++i) {
    svvim::SodaCan object;
    object.mesh     = gl::VboMesh::create(mMesh);
    float w = 5.;
    object.position = Vec3f(svv::random(-w, w),   svv::random(-w, w),   svv::random(-w, w));
    object.r0       = Vec3f(svv::random(-80, 80), svv::random(-80, 80), svv::random(-80, 80));
    object.r1       = Vec3f(svv::random(-80, 80), svv::random(-80, 80), svv::random(-80, 80));
    mObjectList[i]  = object;
  }
}

void InstagramProjectionScene::updateCans() {
  /*
   for (int i = 0; i < mObjectList.size(); ++i) {
   mObjectList[i].r1.x += 1.f;
   mObjectList[i].r1.y += 0.f;
   mObjectList[i].r1.z += 0.f;
   } */
}

void InstagramProjectionScene::updateInstaStream () {
  Surface surface;
  
  if (mCurMovieSurface) {
    Surface surf = mCurMovieSurface.getSurface();
    
    if (surf) {
      mCurTexture = gl::Texture(surf);
    }
    
  }
  
  if (mInstaMovieLoader && mInstaMovieLoader && mInstaMovieLoader.checkPlaythroughOk()) {
    mCurMovieSurface = ci::qtime::MovieSurface(mInstaMovieLoader);
    mCurMovieSurface.play();
    mCurMovieSurface.setVolume(0.f);
    mCurMovieSurface.setLoop();
    mInstaMovieLoader.reset();
  }
  
  if (isReady()) {
    loadNew();
  }
}

void InstagramProjectionScene::drawCans() {
  float t = mParams.count("t") ? mParams["t"] / 15.f : 0.f;
  // t = fmod(t, 1.f);
  for (int i = 0; i < mObjectList.size(); ++i) {
    drawSodaCan(mObjectList[i], t/4.f);
  }
}

#endif
