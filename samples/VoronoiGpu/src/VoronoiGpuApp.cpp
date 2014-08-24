#include "cinder/app/AppBasic.h"
#include "cinder/ip/Hdr.h"
#include "cinder/gl/Texture.h"
#include "VoronoiGpu.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class VoronoiGpuApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings ) { settings->enableHighDensityDisplay( true ); }
	void setup();
	void calculateVoronoiTexture();
	
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	
	void draw();

	vector<ivec2>	mPoints;
	gl::Texture		mTexture;
	bool			mShowDistance;
};

void VoronoiGpuApp::setup()
{
	mShowDistance = true;
	// register window changed display callback
	getWindow()->getSignalDisplayChange().connect( [this] { calculateVoronoiTexture(); } );
	
	mPoints.push_back( ivec2( 100, 100 ) );
	mPoints.push_back( ivec2( 200, 120 ) );
	mPoints.push_back( ivec2( 130, 140 ) );	
	mPoints.push_back( ivec2( 200, 200 ) );
}

void VoronoiGpuApp::calculateVoronoiTexture()
{
	if( mShowDistance ) {
		Channel32f rawDistanceMap = calcDistanceMapGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the raw distances into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture( rawDistanceMap );
	}
	else {
		Surface32f rawDistanceMap = calcDiscreteVoronoiGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the site locations into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture( rawDistanceMap );
	}
}

void VoronoiGpuApp::mouseDown( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
	calculateVoronoiTexture();
}

void VoronoiGpuApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'd' ) {
		mShowDistance = ! mShowDistance;
		calculateVoronoiTexture();
	}
	else if( event.getChar() == 'x' ) {
		mPoints.clear();
		calculateVoronoiTexture();
	}
}

void VoronoiGpuApp::draw()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize() );
	
	gl::color( Color( 1, 1, 1 ) );
	if( mTexture ) {
		gl::draw( mTexture, toPoints( mTexture.getBounds() ) );
		mTexture.disable();
	}
	
	// draw the voronoi sites in yellow
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );	
	for( vector<ivec2>::const_iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		gl::drawSolidCircle( vec2( *ptIt ), 2.0f );
	
	gl::enableAlphaBlending();
	gl::drawStringRight( "Click to add a point", vec2( getWindowWidth() - toPixels( 10 ), getWindowHeight() - toPixels( 20 ) ), Color( 1, 0.3, 0 ), Font( "Arial", toPixels( 12 ) ) );
	gl::disableAlphaBlending();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( VoronoiGpuApp, RendererGl )
