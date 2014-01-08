//
//  SvvimScene.h
//  SvvimCommercial
//
//  Created by Matthew Owen on 8/6/13.
//
//

#pragma once
#ifndef _SvvimScene_h
#define _SvvimScene_h

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/qtime/QuickTime.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SvvimScene {
public:
  // Constructors
  SvvimScene() {}
  
  //
  // The usual collection
  virtual void setup()  {}
  virtual void update() {}
  virtual void draw()   {}
  
  // Access State
  virtual bool isLoaded() { return true;  };
  virtual bool isDone()   { return false; };
  
  
  
  // RENDER
  gl::Texture & render() {
    mRenderFbo.bindFramebuffer();
    gl::clear(Color(1, 1, 1));
    draw();
    mRenderFbo.unbindFramebuffer();
    return mRenderFbo.getTexture();
  }
  
  
  // Set Parameters
  void set(std::string key, float val) {
    mParams[key] = val;
  }
  
  float getElapsedSeconds() {
    if (mAppRef) {
      return mAppRef->getElapsedSeconds() - mStartTime;
    }
    return -1.f;
  }
protected:
  gl::Fbo mRenderFbo;
  
  App * mAppRef;
  bool mIsLoaded, mIsDone;
  float mAlpha, mBeta, mGamma;
  std::map <std::string, float> mParams;
  
  float mStartTime;
  
  gl::GlslProg mShader;
};

#endif
