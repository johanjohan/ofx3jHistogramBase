#pragma once

#include "ofMain.h"
#include "ofx3jHistogramBase.h"

/*
	example how to build your own derived class for special types
	this example takes ofPixels and shows the color hue angles in a histogram
*/
class HistogramPixels : public ofx3jHistogramBase {

public:
	HistogramPixels() : ofx3jHistogramBase(361) { // values from [0...360], a total of 361
		gui.setup(__func__, 200); // name and gui width
		gui.params.noiseThresh.setMax(0.05f); // adjust the slider max...
	}

	void setup() {}

	void update(const ofPixels &_new) {

		if (!gui.flags.update) {
			return;
		}

		const int w = _new.getWidth();
		const int h = _new.getHeight();
		const int step = gui.params.steps;
		assert(step > 0);

		zeroHistograms(getSize());

		// the main work is here: fille the bins of the histogram
		ofColor c;
		for (int y = 0; y < h; y += step) {
			for (int x = 0; x < w; x += step) {
				c = _new.getColor(x, y);
				size_t index = mapToIndex(c.getHueAngle(), 0.f, 360.f); // pass value and the range min/max
				hist.data[index]++;
			}
		}

		findMaxValue();

#if 0
		setValueLimit(getMaxValue()); // adaptive, grows on higher input values, is no more an abolsute % measure
#else
		setValueLimit(w / step * h / step, true); // absolute: max amount of vals that could end up into one single bin, true resets
#endif

		// create text for messsage
		stringstream ss;
		ss << "getIndexAtMaxValue(): " << getIndexAtMaxValue() << endl;
		ss << "getMaxValuePercent(): " << ofToString(getMaxValuePercent(), 2, 4, ' ') << "%";
		setMessage(ss.str());

		// update gui labels
		gui.labels.label1 = "max: " + ofToString(getMaxValuePercent(), 2) + " / 1.0";
		gui.labels.label2 = "fps: " + ofToString(ofGetFrameRate(), 1) + "Hz";
	}

};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void onChangeSteps(int & _value);
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofVideoGrabber videoGrab;
		HistogramPixels histogram;
		
};
