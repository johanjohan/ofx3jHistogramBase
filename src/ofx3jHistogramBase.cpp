#include "ofx3jHistogramBase.h"

size_t ofx3jHistogramBase::Gui::id = 0;

void  ofx3jHistogramBase::draw(
	const int		&_x,
	const int		&_y,
	const int		&_w,
	const int		&_h,
	const int		&_border,
	const ofColor	&_colorBin,
	const ofColor	&_colorFrame,
	const ofColor	&_colorGrid
) {

	if (!gui.flags.draw) {
		return;
	}

	int w = _w - 2 * _border;
	int h = _h - 2 * _border;
	int x = _x + _border;
	int y = _y + _border;
	assert(hist.data.size() > 0);
	float xStep = w / float(hist.data.size() - 0);

	ofColor c = ofColor::red;
	ofPushStyle();
	{
		if (gui.flags.background)
		{
			ofSetColor(ofColor::black); // clear
			ofDrawRectangle(_x, _y, _w, _h);

			c = ofColor::red;
			assert(hist.data.size() > 0);
			c.setHue(hist.indexAtMaxValue / float(hist.data.size() - 1) * 255.f);
			ofSetColor(c, 44);
			ofDrawRectangle(x, y, w, h);
		}

		if (gui.flags.frame)
		{
			ofPushStyle();
			{
				ofNoFill();
				ofSetColor(_colorFrame);
				ofDrawRectangle(x, y, w, h);
			}
			ofPopStyle();
		}

		if (gui.flags.grid)
		{
			ofPoint p0;
			ofPoint p1;
			int numSegments;
			ofSetColor(_colorGrid);

			// eight
			numSegments = 8;
			p0.set(x, y + h);
			p1.set(p0.x, p0.y - h / 6);
			for (size_t i = 0; i <= numSegments; ++i){
				if (i % 2){
					p0.x = p1.x = x + i * w / numSegments;
					ofDrawLine(p0, p1);
				}

			}

			// quart
			numSegments = 4;
			p0.set(x, y + h);
			p1.set(p0.x, p0.y - h / 3);
			for (size_t i = 0; i <= numSegments; ++i){
				p0.x = p1.x = x + i * w / numSegments;
				ofDrawLine(p0, p1);
			}
		}

		// plot bins
		float maxVal = 0;
		ofPoint p1(x, y + h);
		for (size_t i = 0; i < hist.data.size(); i++) // 256
		{
			p1.x = x + i * xStep; //p1.x += xStep;

			size_t iOff = wrap<size_t>(
				i + hist.indexDrawStart,
				0,
				hist.data.size()
				);

			if (gui.flags.hue) {
				c = ofColor::red;
				float hueAngle360 = ofMap(iOff, 0, hist.data.size() - 1, 0.f, 360.f);
				c.setHueAngle(hueAngle360);
			}
			else {
				c = _colorBin;
			}
			ofSetColor(c);

			// bin bars
			float val01 = ofMap(hist.data[iOff], 0, hist.maxValueLimit, 0, 1); // normalize
			if (val01 >= gui.params.noiseThresh) 
			{
				assert(gui.params.amplify.get() > 0);
				val01 = powf(val01, 1.f / gui.params.amplify.get()); // amplify
				float val = val01 * -h;
				ofDrawRectangle(p1, std::max(xStep, 0.5f), val);
				maxVal = std::max(maxVal, std::abs(val)); // for peakline
			}

			if (gui.flags.peakLine) { // vert
				if (iOff == getIndexAtMaxValue()) {
					ofSetColor(c, 0.9 * 255);
					ofDrawLine(ofPoint(p1.x, p1.y - h * 0.9), ofPoint(p1.x, p1.y - h));
				}
			}

		}

		if (gui.flags.peakLine) // horz
		{
			if (gui.flags.hue) {
				c = ofColor::red;
				c.setHue(ofMap(hist.indexAtMaxValue, 0, hist.data.size() - 1, 0.f, 255.f));
			}
			else {
				c = ofColor::white;
			}

			ofSetColor(c, 0.9 * 255);
			float yy = y + h - maxVal;
			ofDrawLine(
				ofPoint(
					x + 0.00 * w, yy
				),
				ofPoint(
					x + 0.10 * w, yy
				)
			);
			ofDrawLine(
				ofPoint(
					x + 0.90 * w, yy
				),
				ofPoint(
					x + 1.00 * w, yy
				)
			);
		}

		if (gui.flags.message && !msg.empty()) {
			stringstream ss;
			ss << msg;
			ofSetColor(_colorFrame);
			ofDrawBitmapStringHighlight(
				ss.str(),
				_x + 2 + 5 + _border,
				_y + 0 + 5 + _border + 12
			);
		}
	}
	ofPopStyle();
}
