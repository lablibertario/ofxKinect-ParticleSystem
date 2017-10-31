#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	kinect.setRegistration(true);
    
	kinect.init();
	
	kinect.open();
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	

	ofSetFrameRate(60);

    bKinectOpen = true ;
    bThreshWithOpenCV=true;
    cursorXSensitivity=3;
    cursorYSensitivity=3;
    setup_ofxUI() ;
    
    int num = 20000;
    ofSetCircleResolution( 10 ) ; 
	p.assign(num, demoParticle());
	currentMode = PARTICLE_MODE_ATTRACT;
    
	currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
    
	resetParticles();
}


void testApp::resetParticles(){
    

	attractPoints.clear();
	for(int i = 0; i < 4; i++){
		attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
	}
	
	attractPointsWithMovement = attractPoints;
	
	for(int i = 0; i < p.size(); i++){
		p[i].setMode(currentMode);
		p[i].setAttractPoints(&attractPointsWithMovement);;
		p[i].reset();
	}
}

void testApp::setup_ofxUI()
{
    //Setup ofxUI
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    guiWidth = 210-xInit;
    
    gui = new ofxUICanvas(0, 0, guiWidth+xInit, ofGetHeight());
	gui->addWidgetDown(new ofxUILabel("KINECT", OFX_UI_FONT_MEDIUM ));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, 255.0, ThresholdLejos, ThresholdCerca, "DEPTH RANGE"));
    gui->addWidgetDown(new ofxUIRangeSlider(guiWidth, dim, 0.0, ((kinect.width * kinect.height ) / 2 ), minBlobSize , maxBlobSize, "BLOB SIZE"));
    
        gui->addWidgetDown(new ofxUIToggle("LIMITADOR" , bLimitadorCursor , dim , dim ) ) ;
    

    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/kinectSettings.xml") ;
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
    
	kinect.update();
	
	
	if(kinect.isFrameNew()) {
		
		
		grayImage.setFromPixels(kinect.getDepthPixels());
		
		
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(ThresholdCerca, true);
			grayThreshFar.threshold(ThresholdLejos);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
			
ofPixels pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < ThresholdCerca && pix[i] > ThresholdLejos) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		

		grayImage.flagImageChanged();
		
        
		contourFinder.findContours(grayImage, minBlobSize , maxBlobSize , 20, false );
        
        
        cursors.clear() ;
        
        
        for ( int i = 0 ; i < contourFinder.nBlobs ; i++ )
        {
            //obtener centroide de depthImage
            ofVec2f iCentroid = contourFinder.blobs[i].centroid ;
            
            
            iCentroid.x /= kinect.width ;
            iCentroid.y /= kinect.height ;
            
            
            iCentroid += ofVec2f( -0.5 , -0.5 ) ;
            
            iCentroid.x *= cursorXSensitivity ;
            iCentroid.y *= cursorYSensitivity ;
            
            iCentroid -= ofVec2f( -0.5 , -0.5 ) ;
            

            iCentroid.x *= ofGetWidth() ;
            iCentroid.y *= ofGetHeight() ;
            
            if ( bLimitadorCursor == true )
            {
                iCentroid.x = ofMap ( iCentroid.x , 0.0 , ofGetWidth() , cursorBorderPadding , ofGetWidth() - cursorBorderPadding , true ) ;
                iCentroid.y = ofMap ( iCentroid.y , 0.0 , ofGetHeight() , cursorBorderPadding , ofGetHeight() - cursorBorderPadding , true ) ;
            }
            cursors.push_back( iCentroid ) ;
        }
	}
	

    
    if ( cursors.size() > 0 )
    {
        for(int i = 0; i < p.size(); i++){
            p[i].setMode(currentMode);
            p[i].setAttractPoints( &cursors ) ; 
            p[i].update( cursors[0].x , cursors[0].y );
        }
    }
    else
    {
        ofPoint center = ofPoint ( ofGetWidth() / 2 , ofGetHeight() / 2 ) ;
        for(int i = 0; i < p.size(); i++){
            p[i].setMode(currentMode);
           // p[i].setAttractPoints( &cursors ) ;
            p[i].update( center.x , center.y );
        }
    }
	
	
	for(int i = 0; i < attractPointsWithMovement.size(); i++){
		attractPointsWithMovement[i].x = attractPoints[i].x + ofSignedNoise(i * 10, ofGetElapsedTimef() * 0.7) * 12.0;
		attractPointsWithMovement[i].y = attractPoints[i].y + ofSignedNoise(i * -10, ofGetElapsedTimef() * 0.7) * 12.0;
	}
}


void testApp::draw() {
	
	ofSetColor(255, 255, 255);
    ofPushMatrix() ; 
	ofTranslate( guiWidth + 10 , 0 ) ;
    
    kinect.drawDepth(10, 10, 400, 300);
    kinect.draw(420, 10, 400, 300);
		
    grayImage.draw(10, 320, 400, 300);
    contourFinder.draw(10, 320, 400, 300);
		

    
    ofPopMatrix() ;
	
    if ( cursors.size() > 0 )
    {

        for ( int i = 0 ; i < cursors.size() ; i++ )
        {

            float ratio = (float) i / ( float) cursors.size() ;
            ofSetColor( ofColor::fromHsb(225.0f * ratio , 255 , 255 ) ) ;
            ofDrawCircle( cursors[i] , 12 ) ;
        }
    }

    
	for(int i = 0; i < p.size(); i++){
		p[i].draw();
	}
	
	ofSetColor(190);
	if( currentMode == PARTICLE_MODE_NEAREST_POINTS ){
		for(int i = 0; i < attractPoints.size(); i++){
			ofNoFill();
			ofDrawCircle(attractPointsWithMovement[i], 10);
			ofFill();
			ofDrawCircle(attractPointsWithMovement[i], 4);
		}
	}
    
	ofSetColor(230);
	
}


void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
	if(name == "DEPTH RANGE")
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        ThresholdLejos = slider->getScaledValueLow() ;
        ThresholdCerca = slider->getScaledValueHigh() ;
	}
    
    if(name == "BLOB SIZE")
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        minBlobSize = slider->getScaledValueLow() ;
        maxBlobSize = slider->getScaledValueHigh() ;
	}
    
   
    
    if(name == "LIMITADOR" )
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        bLimitadorCursor = toggle->getValue() ;
	}

    
   
    gui->saveSettings("GUI/kinectSettings.xml") ; 
}


void testApp::exit() {
	kinect.close();
	

}


void testApp::keyPressed (int key) {
    
    if( key == '1'){
		currentMode = PARTICLE_MODE_ATTRACT;
		currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
	}
	
    
    
    
	if( key == ' ' ){
		resetParticles();
	}
    
}
 