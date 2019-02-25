#pragma once

/*
	3j // https://1001suns.com/

	20190218
		refactor
		integrate offsetPixelsThresh to not fill hist on paused input. it did fill up on the angleOffset without any motion

	20190221
		some clean up
		example with ofPixels
		commented out ofLog()
		#undef NDEBUG to keep asserts in release
		keep vertical peakline in center of bar

*/

/*
	TODO
		count inactive ones so we can see whether the movement comes from a small fraction of the image
			set a threshold01
				count the ones below that threshold01 and consider them being inactive


*/

#include "ofMain.h" 
#include "ofxGui.h"

#if 1					// keep assert() in release as well
#undef NDEBUG
#include <assert.h>		// reinclude the header to update the definition of assert()
#endif

class ofx3jHistogramBase {
protected:

	struct Hist {
		vector<int>				data;					// bins
		int						maxValueLimit = 1;		// all in 1 bin would amount to hist.maxValueLimit = w * h, for normalizing
		float					activePercent = 0;
		size_t					indexAtMaxValue = 0;	// index of max oeak in bins [0...hist.indexAtMaxValue...hist.data.size()-1]
		size_t					indexDrawStart = 0;		// may plot starting from green instead of red...

		struct InputRange {
			float min;
			float max;
		};
		InputRange inputRange;
	};
	Hist hist;

	string						msg = "no message";

	class Gui {
		static size_t id;		// init in .cpp

	public:
		Gui() { ++id; }

		ofxPanel				panel;

		struct Labels {
			ofParameter<string>	filename = ofParameter<string>("filename", "...");
			ofParameter<string>	activePercent = ofParameter<string>("activePercent", "...");
			ofParameter<string>	mappedIndex = ofParameter<string>("mappedIndex", "..."); // at max value
			ofParameter<string>	label1 = ofParameter<string>("label1", "...");
			ofParameter<string>	label2 = ofParameter<string>("label2", "...");
		};
		Labels labels;

		ofParameterGroup		gLabels{ "labels", // ctor not yet called for id++
			labels.filename, labels.activePercent, labels.mappedIndex, labels.label1, labels.label2
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

		ofParameterGroup		gFlags{ "flags",
			flags.update, flags.draw, flags.hue, flags.background, flags.frame, flags.message, flags.peakLine, flags.grid
		};

		struct Params {
			ofParameter<float>	activePercentThresh = ofParameter<float>("activePercentThresh", 0, 0, 1); // todo make useful
			ofParameter<int>	step = ofParameter<int>("step", 1, 1, 32);
			ofParameter<float>  amplify = ofParameter<float>("amplify", 1, 0.25, 10);
			ofParameter<float>  noiseThreshDrawing = ofParameter<float>("noiseThreshDrawing", 0, 0, 0.5); // in Drawing
		};
		Params params;

		ofParameterGroup		gParams{ "params",
			/*params.activePercentThresh,*/ params.step, params.amplify, params.noiseThreshDrawing
		};

		void setup(const string &_name = "HistogramBaseV2", const int &_width = 200) { // drawing artifacts below 200

			const string filename = _name + "_" + ofToString(id, 0, 3, '0') + ".xml";

			panel.clear();
			panel.setup("", filename, 10, 10); // bug ""
			panel.setName(_name);

			panel.maximizeAll();
			panel.setSize(_width, 0);
			panel.setDefaultWidth(_width);

			labels.filename = filename;

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

	// need to resetValueLimit after change of step, as the absolute limit has changed
	void onChangeStep(int &_value) {
		ofLogNotice(__FUNCTION__) << _value;
		resetValueLimit();
	}

public:

	ofx3jHistogramBase(const size_t &_cnt) {
		setSize(_cnt);
		gui.setup(__func__);

		gui.params.step.addListener(this, &ofx3jHistogramBase::onChangeStep);
	}

	~ofx3jHistogramBase() {}

	Gui gui;

	size_t getSize() { return hist.data.size(); }

	void setValueLimit(const int &_maxValueLimit /*, const bool &_bReset = false*/) {
		hist.maxValueLimit = _maxValueLimit; // absolute
		if (getValueLimit() == 0) {
			ofLogWarning(__FUNCTION__) << "getValueLimit() == 0: " << getValueLimit();
		}
	}

	int getValueLimit() { return hist.maxValueLimit; }

	void resetValueLimit() { hist.maxValueLimit = 1; } // 0 can trigger mistake in ofMap as inputMax

	int getMaxValue() {
		return getValueAtIndex(getIndexAtMaxValue());
	}

	float getMaxValuePercent() {
		return getValuePercentAtIndex(getIndexAtMaxValue());
	}

	size_t getIndexAtMaxValue() {
		return hist.indexAtMaxValue;
	}

	/*
		maps current IndexAtMaxValue to original input range
	*/
	float getMappedIndexAtMaxValue() {
		return mapIndexToValue(getIndexAtMaxValue());
	}

	float getIndexPercentAtMaxValue() {
		return getIndexAtMaxValue() / float(getSize() - 1);
	}

	int getValueAtIndex(size_t _index) {
		assert(_index >= 0 && _index < getSize());
		return hist.data.at(_index);
	}

	float getValuePercentAtIndex(size_t _index) {
		assert(getValueLimit() > 0);
		return getValueAtIndex(_index) / float(getValueLimit());
	}

	void setIndexDrawStart(const size_t &_startBinIndex) { // in case we want to start drawing from another value than 0
		assert(_startBinIndex < getSize());
		hist.indexDrawStart = _startBinIndex;
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
		zeroHistograms(_numBins);
		//cout << (__FUNCTION__) << ": hist.data.size(): " << hist.data.size() << endl;
	}

	size_t mapValueToIndex(float value, float inputMin, float inputMax) {
		//ofLogNotice(__FUNCTION__) << "mm: " << inputMin << " | " << inputMax;
		hist.inputRange.min = inputMin;
		hist.inputRange.max = inputMax;
		return roundf(ofMap(value, inputMin, inputMax, 0, getSize() - 1, true));
	}

	// and back
	float mapIndexToValue(const size_t &_index) {
		return ofMap(_index, 0, getSize() - 1, hist.inputRange.min, hist.inputRange.max, true);
	}

	void findMaxValue() {
		hist.indexAtMaxValue = 0; // could use as a low cut filter...
		for (size_t i = hist.indexAtMaxValue + 1; i < hist.data.size(); i++) {
			if (hist.data[i] > hist.data[hist.indexAtMaxValue]) {
				hist.indexAtMaxValue = i;
			}
		}

		gui.labels.mappedIndex = ofToString(getMappedIndexAtMaxValue(), 2);

		// find activePercent
		int activeThresh = gui.params.activePercentThresh * getValueLimit(); // absolute
		int activeValue = 0;
		for (size_t i = 0; i < hist.data.size(); i++)
		{
			if (hist.data[i] >= activeThresh) // > hist.data[i] > thresh
			{
				activeValue += hist.data[i] - activeThresh;
			}
		}
		hist.activePercent = activeValue / float(getValueLimit());
		gui.labels.activePercent = ofToString(hist.activePercent, 2, 4, ' ');
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

	// ofBitMapFont dims
	/*
		bitmap: 360 --> 22x9
		1 letter = 6x9
		space = 2x9
	*/
	int getBitmapFontWidth() { return 6; }
	int getBitmapFontHeight() { return 9; }
	int getBitmapFontSpace() { return 2; }

	int getBitmapFontWidth(const string &_s) {
		return _s.size() *  getBitmapFontWidth() + (_s.size() - 1) * getBitmapFontSpace();
	}



}; // c ofx3jHistogramBase
