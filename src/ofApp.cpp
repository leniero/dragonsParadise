#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofEnableSmoothing();
	ofSetVerticalSync(true);
	
    //USING MAC CAMERA
	video.setup(320, 240);
    
	finder.setup("haarcascade_frontalface_default.xml");
    
    
    //KINECT
    //finder.setup("haarcascade_frontalface_alt2.xml");
    //finder.setup("haarcascade_frontalface_alt_tree.xml");
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
   // kinects.open();
    
    
	usePreview = false;
    
    previewCamera.setDistance(3.0f);
    previewCamera.setNearClip(0.01f);
    previewCamera.setFarClip(500.0f);
    previewCamera.setPosition(10.0f, 0.0f, 0.0f);
    previewCamera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    
    headTrackedCamera.setNearClip(0.001f);
    headTrackedCamera.setFarClip(1000.0f);
    
    //defining the real world coordinates of the window which is being headtracked is important for visual accuracy
    windowWidth = 0.3f;
    windowHeight = 0.2f;
    
    windowTopLeft = glm::vec3(-windowWidth / 2.0f, +windowHeight / 2.0f, 0.0f);
    windowBottomLeft = glm::vec3(-windowWidth / 2.0f,
                               - windowHeight / 2.0f,
                               0.0f);
    windowBottomRight = glm::vec3(+windowWidth / 2.0f,
                                -windowHeight / 2.0f,
                                0.0f);
    
    //we use this constant since we're using a really hacky headtracking in this example
    //if you use something that can properly locate the head in 3d (like a kinect), you don't need this fudge factor
    viewerDistance = 0.2f;
    
    
    //SCENE
    ofPushStyle();
    ofDisableArbTex();
    ofEnableNormalizedTexCoords();
 
    //animationPosition = 1;
    
 
    keyLight.setDiffuseColor(ofFloatColor(1, 0, 0));
    keyLight.setSpecularColor(ofFloatColor(1, 0, 0));
    keyLight.setPointLight();
    keyLight.setPosition(vec3(0, 1, -10));
    
    model.loadModel("animation19.fbx", true);
    model.setScale(0.002,0.002,0.002);
    model.setRotation(0,90,0,1,0);
    model.setPosition(-15, -3, -8);
    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    model.playAllAnimations();
    
    modelTex.load("textures/texDemon.png");
    
    world.set(60, 30);
    world.rotate(90, ofVec3f(-1,0,0));
    world.setPosition(0, -3, -14);
    
    
    tail.set(60, 30);
    tail.rotate(90, ofVec3f(0,1,0));
    tail.rotate(90, ofVec3f(0,1,0));
    tail.rotate(180, ofVec3f(0,1,0));
    tail.setPosition(0, 6, -30);
    
    
    beach.setSpecularColor(ofFloatColor(0.1, 0.1, 0.1 ));
    beach.setAmbientColor(ofFloatColor(0.7, 0.7, 0.7 ));
    
    worldTexture.load("movies/wavesSand.mov");
    worldTexture.play();
    
    tailVid.load("movies/metBeach.mov");
    tailVid.play();
    
    ofPopStyle();
    
    mySound.load("paradise.mp3");
    mySound.setLoop(true);
    mySound.play();
    
    //TEXT
    nextLetterTime = ofGetElapsedTimeMillis();
    lineCount      = 0;
    letterCount    = 0;
    
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile("on the open seashore.txt");
    
    if(buffer.size()) {

        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {

            string line = *it;
            
            // copy the line to draw later
            // make sure its not a empty line
            if(line.empty() == false) {
                seussLines.push_back(line);
            }
            
            // print out the line
            cout << line << endl;
            
        }
        
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //USING MAC CAMERA
    video.update();
    finder.findHaarObjects(video.getPixels());
    
    //KINECT (change video to grayImage for kinect)
//    kinects.update();
//    if(kinects.isFrameNew()) {
//        grayImage.setFromPixels(kinects.getIRPixels());
//    }
    //grayImage.setup(320, 240);
	//finder.findHaarObjects(grayImage.getPixels());
	
	glm::vec3 headPosition(0,0,viewerDistance);
	
	if (finder.blobs.size() > 0) {
		//get the head position in camera pixel coordinates
		const ofxCvBlob & blob = finder.blobs.front();
		float cameraHeadX = blob.centroid.x;
		float cameraHeadY = blob.centroid.y;
		
		//do a really hacky interpretation of this, really you should be using something better to find the head (e.g. kinect skeleton tracking)
		
		//since camera isn't mirrored, high x in camera means -ve x in world
		float worldHeadX = ofMap(cameraHeadX, 0, video.getWidth(), windowBottomRight.x, windowBottomLeft.x);
		
		//low y in camera is +ve y in world
		float worldHeadY = ofMap(cameraHeadY, 0, video.getHeight(), windowTopLeft.y, windowBottomLeft.y);
		
		//set position in a pretty arbitrary way
		headPosition = glm::vec3(worldHeadX, worldHeadY, viewerDistance);
	} else {
		//if (!kinects.isFrameNew()) {
        if (!video.isInitialized()) {
			//if video isn't working, just make something up
			headPosition = glm::vec3(0.5f * windowWidth * sin(ofGetElapsedTimef()), 0.5f * windowHeight * cos(ofGetElapsedTimef()), viewerDistance);
		}
	}
	
	headPositionHistory.push_back(headPosition);
	while (headPositionHistory.size() > 50.0f){
		headPositionHistory.pop_front();
	}

	//these 2 lines of code must be called every time the head position changes
	headTrackedCamera.setPosition(headPosition);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);
    
    
    // Update the frames per second label in the GUI
    myFpsLabel = ofToString(ofGetFrameRate(), 2);
    
    
    
    //SCENE
    model.update();
    model.playAllAnimations();

    tailVid.update();
    tailVid.getCurrentFrame();
       if (tailVid.isFrameNew()){
           ofPixels pixelsT = tailVid.getPixels();
           tailTex.setFromPixels(pixelsT);
       }

    worldTexture.update();
    worldTexture.getCurrentFrame();
       if (worldTexture.isFrameNew()){
           ofPixels pixels = worldTexture.getPixels();
           waveSand.setFromPixels(pixels);
       }
    
    metVid.update();
    metVid.getCurrentFrame();
       
       if (metVid.isFrameNew()){
           ofPixels pixelsM = metVid.getPixels();
          metTex.setFromPixels(pixelsM);
       }
    
    //Move light
    static float phase = 0.;
    phase += 0.01; if(phase>1) phase-=1;
    float px = cos(phase * 2 * 3.1415926535897932384626433832795);
    float pz = sin(phase * 2 * 3.1415926535897932384626433832795);
    keyLight.setPosition(px*50, 20, pz*20);
    
    keyLight.setDiffuseColor(ofFloatColor(0.5, 0.5, 0.5));
    keyLight.setSpecularColor(ofFloatColor(0.1, 0.1, 0.1));
    
    if (ofGetElapsedTimef() > 60){
        keyLight.setDiffuseColor(ofFloatColor(ofRandom(0,1), 0.1, 0.1));
        keyLight.setSpecularColor(ofFloatColor(0.1, 0.1, 1));
    }
    
    if (ofGetElapsedTimef() > 240){
        keyLight.setDiffuseColor(ofFloatColor(ofRandom(0,1), 0.1, 0.1));
        keyLight.setSpecularColor(ofFloatColor(0.1, 0.1, ofRandom(0,1)));
    }
    
    if (ofGetElapsedTimef() > 480){
    keyLight.setDiffuseColor(ofFloatColor(ofRandom(0,1), 0.1, ofRandom(0,1)));
    keyLight.setSpecularColor(ofFloatColor(ofRandom(0,1)));
    }
    
    if (ofGetElapsedTimef() > 700){
    keyLight.setDiffuseColor(ofFloatColor(ofRandom(0,2), 0.1, ofRandom(0,2)));
    keyLight.setSpecularColor(ofFloatColor(ofRandom(0,2)));
    }
    
    if (ofGetElapsedTimef() > 820){
    keyLight.setDiffuseColor(ofFloatColor(ofRandom(0,3), 0.1, ofRandom(0,3)));
    keyLight.setSpecularColor(ofFloatColor(ofRandom(0,3)));
    }
    
}

//--------------------------------------------------------------
void ofApp::drawScene(bool isPreview){
	
	ofEnableDepthTest();

	if (isPreview) {
		ofPushStyle();
		ofSetColor(150, 100, 100);
		ofDrawGrid(1.0f, 5.0f, true);
		
		ofSetColor(255);
		
        
		//--
		//draw camera preview
		//
		headTrackedCamera.transformGL();
		
		ofPushMatrix();
		ofScale(0.002f, 0.002f, 0.002f);
		ofNode().draw();
		ofPopMatrix();
		
        ofMultMatrix(glm::inverse(headTrackedCamera.getProjectionMatrix()));
		
		ofNoFill();
		ofDrawBox(2.0f);
		
		headTrackedCamera.restoreTransformGL();
		
        ofPopStyle();
	}
    
    //SCENE
    ofEnableLighting();
    keyLight.enable();
   
    
    //keyLight.draw();
    
    modelTex.getTexture().bind();
    model.drawFaces();
    modelTex.getTexture().unbind();
    model.playAllAnimations();
  
    
    beach.begin();
    waveSand.getTexture().bind();
    world.drawFaces();
    waveSand.getTexture().unbind();
    
    tailTex.getTexture().bind();
    tail.drawFaces();
    tailTex.getTexture().unbind();
    
    beach.end();
    
    keyLight.disable();
  
    
    
    
	
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(ofColor(50), ofColor(0));
    
	if (usePreview){
		previewCamera.begin();
	}
	else{
		headTrackedCamera.begin();
	}
	
    
	drawScene(usePreview);
	
	if (usePreview){
		previewCamera.end();
	}
	else{
		headTrackedCamera.end();
	}
	//
	//------
    //USING MAC CAMERA
    ofPushMatrix();
    ofDisableNormalizedTexCoords();
    ofScale(0.5);
    video.draw((ofGetWidth()*2)-video.getWidth(), 0);
    ofPopMatrix();

    
    //KINECT
    //grayImage.draw(0, 0);
    ofEnableNormalizedTexCoords();
   


	ofPushStyle();
	ofNoFill();
	for(unsigned int i = 0; i < finder.blobs.size(); i++) {
		ofRectangle cur = finder.blobs[i].boundingRect;
        ofPushMatrix();
        ofScale(0.5);
        ofTranslate((ofGetWidth()*2)-video.getWidth(), 0);
		ofDrawRectangle(cur.x, cur.y, cur.width, cur.height);
        ofPopMatrix();
	}
	ofPopStyle();
	
	stringstream message;
	message << "[SPACE] = User preview camera [" << (usePreview ? 'x' : ' ') << "]";
    ofDrawBitmapString(message.str(), 20, 40);
    
    stringstream ss;
    ss << "FPS: " << ofToString(ofGetFrameRate(),0) <<endl<<endl;
    ofDrawBitmapString(ss.str().c_str(), 20, 20);
    
	
    //TEXT
    // the total width on the line
    int strWidth = (seussLines[lineCount].length()*8) + 5;
    
    // x and y for the drawing
    float x = (ofGetWidth()-(strWidth*1.5))/3;
    float y = (ofGetHeight()/4)*2.5;
    
    
    // we are slowy grabbing part of the line
    string typedLine = seussLines[lineCount].substr(0, letterCount);
    
    // draw the line
    ofPushMatrix();
    ofPushStyle();
    ofScale(1.5);
    ofSetColor(0);
//    ofSetRectMode(OF_RECTMODE_CENTER);
    ofDrawRectangle(x, y, strWidth, 15);
    ofSetColor(255);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofDrawBitmapString(typedLine, x+4, y+11);
    
    
    
    // this is our timer for grabbing the next letter
    float time = ofGetElapsedTimeMillis() - nextLetterTime;
    
    if(time > 9) {
        
        // increment the letter count until
        // we reach the end of the line
        if(letterCount < (int)seussLines[lineCount].size()) {
           
            // move on to the next letter
            letterCount ++;
            
            // store time for next letter type
            nextLetterTime = ofGetElapsedTimeMillis();
            
        }
        else {
            
            // wait just a flash then move on
            // to the next line...
            if(time > 300) {
                
                nextLetterTime = ofGetElapsedTimeMillis();
                letterCount = 0;
                lineCount ++;
                lineCount %= seussLines.size();
            }
            
        }
    }
    
    ofPopStyle();
    ofPopMatrix();
	
	if (usePreview){
		ofRectangle bottomLeft(0, ofGetHeight() - 200.0f, 300.0f, 200.0f);
		
		ofPushStyle();
		ofSetColor(0);
		ofDrawRectangle(bottomLeft);
		ofPopStyle();
		
		headTrackedCamera.begin(bottomLeft);
		drawScene(false);
		headTrackedCamera.end();
	}
	//
	//------
}

//--------------------------------------------------------------
void ofApp::exit(){
//    kinects.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ')
		usePreview = !usePreview;
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
