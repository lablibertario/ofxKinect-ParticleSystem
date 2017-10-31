#pragma once


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "demoParticle.h"
#include "ofxUI.h"



class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
    
    
    void setup_ofxUI() ;
	
	ofxKinect kinect;
	
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    
	ofxCvContourFinder contourFinder;
	
	bool bThreshWithOpenCV;
	
	int ThresholdCerca;
	int ThresholdLejos;
	
	int angle;
	

    ofxUICanvas *gui;
    float guiWidth ; 
	void guiEvent(ofxUIEventArgs &e);
    
    float minBlobSize , maxBlobSize ;
    
    bool bKinectOpen ;
    

    vector<ofPoint> cursors ;
    float cursorXSensitivity ;
    float cursorYSensitivity ;
    bool bLimitadorCursor ;
    float cursorBorderPadding ;
    
    particleMode currentMode;
    string currentModeStr;
    
    void resetParticles();
    vector <demoParticle> p;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;
    
};
