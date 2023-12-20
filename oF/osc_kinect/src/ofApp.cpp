#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(30); // run at 60 fps
    ofDisableArbTex();
    ofSetWindowShape(640, 480);
    ofSetVerticalSync(false);
    ofSetCircleResolution(100);
    ofSetDepthTest(false); // disable this for smooth colour // enable for shards of colour

    kinect.open();
    kinect.initDepthSource();
    kinect.initColorSource();
    kinect.initBodyIndexSource();
    kinect.initBodySource();

    // Setup the parameters.
    nearThreshold.set("Near Threshold", 0.01f, 0.0f, 0.1f);
    farThreshold.set("Far Threshold", 0.02f, 0.0f, 0.1f);
    time1.set("Time 1", .011, .001, .3);
    time2.set("Time2", .01, 0, .3);
    r1a.set("R1A", 50, 0, 100);
    r1b.set("R1B", 100, 0, 100);

    // Setup the gui.
    guiPanel.setup("Depth Threshold", "settings.json");
    guiPanel.add(nearThreshold);
    guiPanel.add(farThreshold);
    guiPanel.add(r1a);
    guiPanel.add(r1b);
    guiPanel.add(time1);
    guiPanel.add(time2);

    //CV stuff
    thresholdImg.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    foregroundImg.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    bodyIndexImg.allocate(640, 480, OF_IMAGE_COLOR);

    contour.setMinAreaRadius(30);


    sender.init("of_Sender");

    laser.setCanvasSize(1600, 1600);
    //   sender.setup(HOST, PORT);

    fbo.allocate(1080, 1080, GL_RGBA);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();


}

//--------------------------------------------------------------
void ofApp::update() {
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());

    
    kinect.update();
    float time = ofGetElapsedTimef();

    if (kinect.isFrameNew()) {

        //depth
        std::shared_ptr<ofxKFW2::Source::Depth> depthSource = kinect.getDepthSource();
        ofFloatPixels depthPix = depthSource->getPixels();

        ofFloatPixels thresholdNear, thresholdFar, thresholdResult;
        ofxCv::threshold(depthPix, thresholdNear, nearThreshold);
        ofxCv::threshold(depthPix, thresholdFar, farThreshold, true);
        ofxCv::bitwise_and(thresholdNear, thresholdFar, thresholdResult);
        ofxCv::erode(thresholdResult, 2);
        ofxCv::dilate(thresholdResult);
        ofxCv::dilate(thresholdResult);
        ofxCv::blur(thresholdResult, 1);

        thresholdImg.setFromPixels(thresholdResult);
        contour.findContours(thresholdImg);
        poly = contour.getPolylines();

        bodyIndexImg.update();
    }

    for (int i = 0; i < poly.size(); i++) {
        poly[i] = poly[i].getSmoothed(2);
        poly[i].close();
    }
 
  
    sender.send(fbo.getTexture());
    laser.update();
}

void ofApp::scalePolyUp(ofPolyline poly, int count) {
    float time = ofGetElapsedTimef();
    //ofVec2f center = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
    ofVec2f center = ofVec2f(320, 240);
   
    for (int j = 0; j < count; j++) {
        poly.translate(-center);
        poly.scale(1.8, 1.8);
        poly.translate(center);
    
    //    poly.draw(); //normal draw
        if (j == count - 1) {
            for (int p = 0; p < 400; p++) {
                ofSetColor(ofColor::white);
                ofPoint point = poly.getPointAtPercent(p / 400.0);
                float val2 = sin(p * .38 + time + j);
                float val2Mapped = ofMap(val2, -1, 1, .1, 1);
                ofDrawCircle(point, 40 - 60 * val2Mapped);
            }
        }
        else {
            poly.draw(); //normal draw
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
  //  guiPanel.draw();
    
    fbo.begin();
    ofClear(0, 0, 0, 0);
    ofTranslate(250, 200);
    for (int i = 0; i < poly.size(); i++) {
        scalePolyUp(poly[i], 3);
      
    }
    fbo.end();
    fbo.draw(0, 0);

    ofVec2f center = ofVec2f(320, 280);
    for (int i = 0; i < poly.size(); i++) {
        for (int j = 0; j < 6; j++) {
            poly[i].translate(-center);
            poly[i].scale(1.25, 1.25);
            poly[i].translate(center);

            laser.drawPoly(poly[i], ofColor::green, OFXLASER_PROFILE_FAST);
        }
    }

    laser.send();
   laser.drawUI();
}
