#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(false);

	videoGrab.setup(640, 480);
	videoGrab.setDesiredFrameRate(30);

	histogram.setup();

	// these are extra, do as you like
	//histogram.setIndexDrawStart(180); // suppose you want to watch reds and want to shift the drawing by 180 degrees so they dont split at 0/360
	//histogram.gui.panel.getGroup("flags").minimize();

}
//--------------------------------------------------------------
void ofApp::update(){
	videoGrab.update();
	if (videoGrab.isFrameNew())
	{
		histogram.update(videoGrab.getPixels());
		//histogram.setIndexDrawStart(size_t(ofGetElapsedTimef() / .333f) % histogram.getSize()); // slide for the fun of it
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(33);
	if (videoGrab.isInitialized()) {
		videoGrab.draw(50,50);
	}
	histogram.draw(50, 50, videoGrab.getWidth(), videoGrab.getHeight(), 20);
	histogram.gui.draw(50 + videoGrab.getWidth() + 20, 50);

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(), 1), 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
