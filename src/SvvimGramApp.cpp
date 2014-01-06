#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SvvimGramApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void SvvimGramApp::setup()
{
}

void SvvimGramApp::mouseDown( MouseEvent event )
{
}

void SvvimGramApp::update()
{
}

void SvvimGramApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( SvvimGramApp, RendererGl )
