#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "box.h"
#include "ray.h"

class TreeNode {
public:
	vector<TreeNode*> child; //up to 8 child nodes
	vector<int> indices;
	Box parent;
	TreeNode() {}
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
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();
	void toggleSelectTerrain();
	void setCameraTarget();
	void octree(int, TreeNode*, const ofMesh&);
	void drawOctreeBox(TreeNode*, int);
	void intersection_test(Ray, TreeNode*, const ofMesh&);
	bool in_box(ofVec3f, TreeNode*);
	bool doPointSelection();
	void drawBox(const Box &box);
	Box meshBounds(const ofMesh &);
	void subDivideBox8(const Box &b, vector<Box> & boxList);

	bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);

	ofEasyCam cam;
	ofxAssimpModelLoader mars, rover;
	ofLight light;
	Box boundingBox;
	TreeNode* head;
	vector<Box> level1, level2, level3;

	ofxIntSlider levels;
	ofxPanel gui;

	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;
	bool bPointSelected;
	bool drawCircle;
	ofVec3f circle_index;
	bool bRoverLoaded;
	bool bTerrainSelected;

	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	int level_cap;

	const float selectionRange = 4.0;
};

