#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "cinder/qtime/QuickTime.h"

#include "cinder/audio/Io.h"
#include "cinder/audio/Input.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"

#include "SvvimPost.h"
#include "AsynchMovieWriter.h"
#include "ProjectionScene.h"
#include "InstagramProjectionScene.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Rectf getCoveringRect(gl::Texture texture, Area containerArea) {
  float scale;
  Rectf bounds = texture.getBounds();
  bounds = bounds.getCenteredFit(containerArea, true);
  
  int container_h = containerArea.getHeight(),
  container_w = containerArea.getWidth(),
  texture_h = bounds.getHeight(),
  texture_w = bounds.getWidth();
  
  if (abs(container_h - texture_h) > abs(container_w - texture_w))
    scale = (float) container_h/texture_h;
  else
    scale = (float) container_w/texture_w;
  
  bounds.scaleCentered(scale + .05);
  
  return bounds;
}

class SvvimGramApp : public AppNative {
public:
  void prepareSettings(Settings *settings);
	void setup();
	void update();

 	void draw();
	void drawLogo();
  gl::Texture mSvvimLogo;
  
private:
  shared_ptr <AsynchMovieWriter> mWriter;

  struct {
    shared_ptr <InstagramProjectionScene> projection;
  } mScenes;
  
  // Audio
  struct {
    audio::Input input;
    std::shared_ptr<float> fft;
    audio::PcmBuffer32fRef pcmBuffer;
  } mAudio;
  void updateAudio();
  
  // Parameters
  struct {
    float a, b, c;
    float gray;
  } m___;
  
  int mStep;
  
  SvvimPost mPost;
};

void SvvimGramApp::prepareSettings(Settings *settings) {
  settings->setFullScreen();
}

void SvvimGramApp::setup() {
  
  mSvvimLogo = loadImage(loadResource("svvim-logo-white.png"));
  
  m___.a = m___.b = m___.c = 1.f;
  
  mPost.setup();
  mAudio.input = audio::Input();
	mAudio.input.start();
  
  mStep = 0;
  
  // Random Number Seed
  srand(time(NULL));
  
  // Movie safe-file path
  fs::path path = fs::path("./what.mp4");
	if( path.empty() )
		return;
  
  // Write screen to movie asynchornously
  qtime::MovieWriter::Format format;
  format.setCodec(qtime::MovieWriter::CODEC_RAW);
  
  // format.setCodec(qtime::MovieWriter::..Format::)
  // mWriter = make_shared <AsynchMovieWriter> (path, 800, 800, format);

  // Initialize scenes
  mScenes.projection = make_shared <InstagramProjectionScene> ();
  mScenes.projection->setup();
}

void SvvimGramApp::update() {
  
  updateAudio();
  
  mStep += 1;
  mScenes.projection->update();
  m___.a *= 1;
  m___.b *= 1;
  m___.c *= 1;
  
  if (m___.a < .075)
    m___.a = 0.f;

}

void SvvimGramApp::updateAudio() {
  mAudio.pcmBuffer = mAudio.input.getPcmBuffer();
  
  if (mAudio.pcmBuffer)
    mAudio.fft = audio::calculateFft(mAudio.pcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), 512);
  else
    return;
  
	uint16_t bandCount = 512;
	std::shared_ptr <float> fftRef = audio::calculateFft(mAudio.pcmBuffer->getChannelData(audio::CHANNEL_FRONT_LEFT), bandCount);
  
	if (!fftRef)
		return;
  
  float * fftBuffer = fftRef.get();
  
  int maxBand = bandCount / 5;
  
  for( int i = 0; i < maxBand; i++ )
    m___.a += fftBuffer[i] * pow(2, -2.) / maxBand;
  
  m___.a /= 1.07f;
  m___.a = max(m___.a, 0.f);
  m___.a = min(m___.a, 1.9f) / 1.75f;
}

#define TOTAL_FRAMES 70
void SvvimGramApp::draw() {
  // DRAW!!
  
  // float t = (mStep % TOTAL_FRAMES) / (float) TOTAL_FRAMES / 15.f;

  //if (mStep >= TOTAL_FRAMES)
  //  quit();
  
  mScenes.projection->set("t", getElapsedSeconds());

  gl::clear(Color(1, 1, 1));
  
  
  gl::enableAlphaBlending();
  gl::disableDepthRead();
  gl::disableDepthWrite();

  gl::Texture texture, blurred;
  
  texture = mScenes.projection->render();
  blurred = mPost.applyBlur(texture, m___.a);

  gl::enableAdditiveBlending();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  
  gl::color(ColorA(1, 1, 1, m___.a));
  gl::draw(texture, getWindowBounds());

  gl::color(ColorA(1, 1, 1, m___.a/3.f));
  gl::draw(blurred, getWindowBounds());
  
  drawLogo();
  
  //texture = mPost.applyBlur(texture, 3);

  //gl::color(Color(1, 1, 1));
  // gl::draw(texture, getWindowBounds());
  
  char path[256];
  sprintf(path, "%03d.png", mStep);
  // app::console() << path << "\n";
}
void SvvimGramApp::drawLogo() {
  if (mSvvimLogo) {
    gl::pushMatrices();
    gl::setMatricesWindow(getWindowSize());
    
    float w = 850;
    float h = w/mSvvimLogo.getAspectRatio();
    float x = (getWindowWidth() - w)/2.f;
    float y = (getWindowHeight() - h)/2.f;
    gl::color(1, 1, 1, .4f);
    gl::draw(mSvvimLogo, Rectf(x, y, x + w, y + h));
    
    gl::popMatrices();
  }
}



CINDER_APP_NATIVE( SvvimGramApp, RendererGl )

























