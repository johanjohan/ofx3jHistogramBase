#pragma once

#include "ofMain.h"
#include "ofx3jHistogramBase.h"

/*
	example how to build your own derived class for special data types
	this example takes ofPixels and shows the color hue angles
*/
class HistogramPixels : public ofx3jHistogramBase {

public:
	// 361, num of bins, values from [0...360] (361 elements)
	// can as well use lower or higher values, ie for creating bands, try value 12+1...
	HistogramPixels() : ofx3jHistogramBase(12 + 1) {
		gui.setup(__func__, 200); // name and gui width
		gui.params.noiseThreshDrawing.setMax(0.3); // example setting a var: adjust the slider max
	}

	void setup() {}

	void update(const ofPixels &_new) {

		if (!gui.flags.update) {
			return;
		}

		const int w = _new.getWidth();
		const int h = _new.getHeight();
		const int step = gui.params.step;
		assert(step > 0);

		zeroHistograms(getSize());

		// the main work is here: fille the bins of the histogram
		ofColor c;
		for (int y = 0; y < h; y += step) {
			for (int x = 0; x < w; x += step) {
				c = _new.getColor(x, y);
				size_t index = mapValueToIndex(c.getHueAngle(), 0.f, 360.f); // pass value and the range min/max
				hist.data[index]++;
			}
		}

		findMaxValue();
		setValueLimit(w / step * h / step); // absolute: max amount of vals that could end up into one single bin

		// create text for messsage
		stringstream ss;
		ss << "getIndexAtMaxValue()       : " << getIndexAtMaxValue() << endl;
		ss << "getMappedIndexAtMaxValue() : " << ofToString(getMappedIndexAtMaxValue(), 2, 4, ' ') << "" << endl;
		ss << "getIndexPercentAtMaxValue(): " << ofToString(getIndexPercentAtMaxValue(), 2, 4, ' ') << "%" << endl;
		ss << "getMaxValuePercent()       : " << ofToString(getMaxValuePercent(), 2, 4, ' ') << "%";
		setMessage(ss.str());

		// example: update gui labels
		gui.labels.label1 = "max: " + ofToString(getMaxValuePercent(), 2) + " of 1.0";
		gui.labels.label2 = "fps: " + ofToString(ofGetFrameRate(), 1) + "Hz";
	}

};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
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
