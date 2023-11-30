#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"

using namespace glm;

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    void exit();
		void drawScene(bool isPreview);
		
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

        //USING MAC CAMERA
		ofVideoGrabber video;
    
		ofxCvHaarFinder finder;
	
		//the view window is defined by 3 corners
		glm::vec3 windowTopLeft;
		glm::vec3 windowBottomLeft;
		glm::vec3 windowBottomRight;
		ofCamera headTrackedCamera;
		ofEasyCam previewCamera;
	
		bool usePreview;
		float windowWidth;
		float windowHeight;
		float viewerDistance;
	
		deque<glm::vec3> headPositionHistory;

		ofVboMesh window;
    
    //KINECT
//    ofxKinectV2 kinects;
//    ofxCvGrayscaleImage grayImage;
//
    
    //SCENE
    ofxAssimpModelLoader model;
    ofPlanePrimitive world;
    ofPlanePrimitive tail;
    
    ofMesh mesh;

    ofLight light;
    ofLight keyLight;
    
    ofImage modelTex;

    
    ofVideoPlayer worldTexture;
    ofImage waveSand;
    ofMaterial beach;
    
    ofVideoPlayer tailVid;
    ofImage tailTex;
    
    ofxLabel myFpsLabel;
    ofxPanel myGui;
    
    bool bHelpText;
    bool bAnimate;
    bool bAnimateMouse;
    float animationPosition;
    
    ofSpherePrimitive metDome;
    ofVideoPlayer metVid;
    ofImage metTex;
    
    ofSoundPlayer   mySound;
    
    float           nextLetterTime;
    int             lineCount;
    int             letterCount;
    vector <string> seussLines;
};
