#pragma once

/*
	3j // https://1001suns.com/

	20190218
		refactor
		integrate threshOffsetPx to not fill hist on paused input. it did fill up on the angleOffset without any motion

	20190221
		some clean up
		example with ofPixels
		no abstract class template yet, but maybe not necessary

	todo
		have to get rid of some sliders that dont belong here
		improve grid drawing, take off the loop
*/

#include "ofMain.h"
#include "ofxGui.h"

class ofx3jHistogramBase {
protected:

	struct Hist {
		vector<int>				data;					// bins
		int						maxValueLimit = 0;		// all in 1 bin would amount to hist.maxValueLimit = w * h, for normalizing
		size_t					indexAtMaxValue = 0;	// index of max oeak in bins [0...hist.indexAtMaxValue...hist.data.size()-1]
		size_t					indexDrawStart = 0;		// may plot starting from green instead of red...
	};
	Hist hist;

	string						msg = "no message";

	class Gui {
		static size_t id;		// init in .cpp

	public:
		Gui() { ++id; }

		ofxPanel				panel;

		struct Labels {
			ofParameter<string>	label1 = ofParameter<string>("label1");
			ofParameter<string>	label2 = ofParameter<string>("label2");
		};
		Labels labels;

		ofParameterGroup		gLabels{ "labels", // ctor not yet called for id++
			labels.label1, labels.label2
		};

		struct Flags {
			ofParameter<bool>	update = ofParameter<bool>("update", true);
			ofParameter<bool>	draw = ofParameter<bool>("draw", true);
			ofParameter<bool>	hue = ofParameter<bool>("hue", false);
			ofParameter<bool>	background = ofParameter<bool>("background", false);
			ofParameter<bool>	frame = ofParameter<bool>("frame", true);
			ofParameter<bool>	message = ofParameter<bool>("message", true);
			ofParameter<bool>	peakLine = ofParameter<bool>("peakLine", true);
			ofParameter<bool>	grid = ofParameter<bool>("grid", true);
		};
		Flags flags;

		ofParameterGroup		gFlags{ "flags", // ctor not yet called for id++
			flags.update, flags.draw, flags.hue, flags.background, flags.frame, flags.message, flags.peakLine, flags.grid
		};

		struct Params {
			ofParameter<int>	steps = ofParameter<int>("steps", 1, 1, 32);
			ofParameter<float>  amplify = ofParameter<float>("amplify", 1, 0.25, 10);
			ofParameter<float>  noiseThresh = ofParameter<float>("noiseThresh", 0, 0, 0.5);
		};
		Params params;

		ofParameterGroup		gParams{ "params",
			params.steps, params.amplify, params.noiseThresh
		};

		void setup(const string &_name = "HistogramBaseV2", const int &_width = 200) { // drawing artifacts below 200

			const string filename = _name + "_" + ofToString(id, 0, 3, '0') + ".xml";

			panel.clear();
			panel.setup("", filename, 10, 10); // bug ""
			panel.setName(_name);

			panel.maximizeAll();
			panel.setSize(_width, 0);
			panel.setDefaultWidth(_width);

			panel.add(gLabels);
			panel.add(gFlags);
			panel.add(gParams);

			panel.setWidthElements(_width);

			if (ofFile::doesFileExist(filename)) panel.loadFromFile(filename);
		}

		void draw(int x, int y) {
			panel.setPosition(x, y);
			panel.draw();
		}
	};

public:

	ofx3jHistogramBase(const size_t &_cnt) {
		setSize(_cnt);
		gui.setup(__func__);
	}

	~ofx3jHistogramBase() {}

	Gui gui;

	size_t getSize() { return hist.data.size(); }

	void setValueLimit(const int &_maxBinValue, const bool &_bReset = false) {
		if (_bReset) resetValueLimit();
		hist.maxValueLimit = std::max(hist.maxValueLimit, _maxBinValue); // adpative
	}

	void resetValueLimit() { hist.maxValueLimit = 0; }

	int getValueLimit() { return hist.maxValueLimit; }

	int getMaxValue() {
		return getValueAtIndex(getIndexAtMaxValue());
	}

	float getMaxValuePercent() {
		return getValuePercentAtIndex(getIndexAtMaxValue());
	}

	size_t getIndexAtMaxValue() {
		return hist.indexAtMaxValue;
	}

	float getIndexAtMaxValuePercent() {
		return getIndexAtMaxValue() / float(getSize() - 1);
	}

	int getValueAtIndex(size_t _index) {
		assert(_index >= 0 && _index < getSize());
		return hist.data.at(_index);
	}

	float getValuePercentAtIndex(size_t _index) {
		return getValueAtIndex(_index) / float(getValueLimit());
	}

	void setIndexDrawStart(const size_t &_startBinIndex) { // incase we want to start drawing from another value than 0
		assert(_startBinIndex < getSize());
		hist.indexDrawStart = _startBinIndex;
		ofLogNotice(__FUNCTION__) << hist.indexDrawStart;
	}

	void setMessage(const string &_msg) {
		msg = _msg;
	}

	void  draw(
		const int		&_x,
		const int		&_y,
		const int		&_w,
		const int		&_h,
		const int		&_border = 0,
		const ofColor	&_colorBin = ofColor::white, // otherwise
		const ofColor	&_colorFrame = ofColor(ofColor::white, 128),
		const ofColor	&_colorGrid = ofColor(ofColor::white, 128)
	);

	void  draw(
		const ofRectangle	&_rect,
		const int			&_border = 0,
		const ofColor		&_colorBin = ofColor::white, // otherwise
		const ofColor		&_colorFrame = ofColor(ofColor::white, 128),
		const ofColor		&_colorGrid = ofColor(ofColor::white, 128)
	) {
		draw(
			_rect.x,
			_rect.y,
			_rect.width,
			_rect.height,
			_border,
			_colorBin,
			_colorFrame,
			_colorGrid
		);
	}

protected:
	void zeroHistograms(const size_t &_numBins) {
		assert(_numBins > 0);
		hist.data.clear();
		hist.data.assign(_numBins, 0); // alloc and zero hist
	}

	void setSize(const size_t &_numBins) {
		assert(_numBins > 0);
		zeroHistograms(_numBins);
		cout << (__FUNCTION__) << ": hist.data.size(): " << hist.data.size() << endl;
	}

	size_t mapToIndex(float value, float inputMin, float inputMax) {
		return roundf(ofMap(value, inputMin, inputMax, 0, getSize() - 1, true));
	}

	void findMaxValue() {
		hist.indexAtMaxValue = 0; // could use as a low cut filter...
		for (size_t i = hist.indexAtMaxValue + 1; i < hist.data.size(); i++) {
			if (hist.data[i] > hist.data[hist.indexAtMaxValue]) {
				hist.indexAtMaxValue = i;
			}
		}
	}

	template <typename T>
	inline T wrap(T value, T from, T to) { // from ofWrap()  // algorithm from http://stackoverflow.com/a/5852628/599884

		if (from > to) {
			swap(from, to);
		}
		T cycle = to - from;
		if (cycle == T(0)) {
			return to;
		}

		if (std::is_same<T, double>::value) {
			return value - cycle * floor((value - from) / cycle);
		}
		else {
			return value - cycle * floorf((value - from) / cycle);
		}
	}


}; // c ofx3jHistogramBase
