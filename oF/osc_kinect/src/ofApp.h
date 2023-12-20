#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxSpout.h"
#include "ofxOsc.h"
#include "ofxLaserManager.h"

#define HOST "localhost"
#define PORT 12345

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void scalePolyUp(ofPolyline, int);


	vector<ofPolyline> poly;
	ofxCv::ContourFinder contour;
	ofxKFW2::Device kinect;

	ofImage thresholdImg; //threshold
	ofImage bodyIndexImg; //bodyindex
	ofImage foregroundImg; //foreground
	int numBodiesTracked;
	float val;
	ofParameter<float> nearThreshold;
	ofParameter<float> farThreshold;

	ofParameter<int> color1a;
	ofParameter<int> color1b;
	ofParameter<int> color1c;
	ofParameter<float> time1;

	ofParameter<int> r1a;
	ofParameter<int> r1b;
	ofParameter<float> time2;

	ofxPanel guiPanel;

	//shader jazz
	//ofShader shader;
	ofFbo fbo;

	//spout jazz
	ofxSpout::Sender sender;

	//OSC
	//ofxOscSender sender;
	ofxLaser::Manager laser;

};
