
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Mars HiRise Project - startup scene
// 
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from Mars
//  HiRis photographs taken the Mars Reconnaisance Orbiter
//
//  You will use this source file (and include file) as a starting point
//  to implement assignment 5  (Parts I and II)
//
//  Please do not modify any of the keymappings.  I would like 
//  the input interface to be the same for each student's 
//  work.  Please also add your name/date below.

//  Please document/comment all of your work !
//  Have Fun !!
//
//  Student Name:   Edmond Chew
//  Date: 4/19/18


#include "ofApp.h"
#include "Util.h"



//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	level_cap = 8;
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
	//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup gui
	gui.setup();
	gui.add(levels.setup("Box Depth", 3, 1, level_cap));

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/mars-low-v2.obj");
	mars.setScaleNormalization(false);

	boundingBox = meshBounds(mars.getMesh(0));

	//  Test Box Subdivide
	//
	subDivideBox8(boundingBox, level1);
	subDivideBox8(level1[0], level2);
	subDivideBox8(level2[0], level3);
	//create head TreeNode
	head = new TreeNode();
	head->parent = boundingBox;
	int indices = mars.getMesh(0).getNumVertices();
	for (int i = 0; i < indices; i++) {
		head->indices.push_back(i);
	}
	//apply octree
	octree(level_cap, head, mars.getMesh(0));
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {

}
//--------------------------------------------------------------
void ofApp::draw() {
	//	ofBackgroundGradient(ofColor(20), ofColor(0));   // pick your own backgroujnd
	ofBackground(ofColor::black);
	//	cout << ofGetFrameRate() << endl;
	ofFill();
	ofSetColor(ofColor::white);
	//draw gui
	gui.draw();

	cam.begin();

	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bRoverLoaded) {
			rover.drawWireframe();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();

		if (bRoverLoaded) {
			rover.drawFaces();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::red);
		ofDrawSphere(selectedPoint, .1);
	}

	//draw octree
	ofNoFill();
	ofSetColor(ofColor::white);
	//drawBox(boundingBox);
	drawOctreeBox(head, levels);


	//draw circle 
	if (drawCircle) {
		ofSetColor(ofColor::blue);
		ofFill();
		ofDrawSphere(circle_index, .1);
	}
	//ofSetColor(ofColor::red);
	//for (int i=0; i < level1.size(); i++)
	//	drawBox(level1[i]);

	//ofSetColor(ofColor::blue);
	//for (int i = 0; i < level2.size(); i++)
	//	drawBox(level2[i]);

	//ofSetColor(ofColor::yellow);
	//for (int i = 0; i < level3.size(); i++)
	//	drawBox(level3[i]);


	ofPopMatrix();
	cam.end();
}

// 

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	TreeNode* temp = head;
	int time = ofGetElapsedTimeMillis();
	intersection_test(ray, temp, mars.getMesh(0));
	time = ofGetElapsedTimeMillis() - time;
	printf("Time alloted: %i millis\n", time);
	//if (level3[1].intersect(ray, -100, 100)) cout << "intersects" << endl;
}

void ofApp::intersection_test(Ray r, TreeNode* n, const ofMesh &mesh) {
	//base case - no more nodes to traverse and still intersecting
	if (n->child.empty() && n->parent.intersect(r, -100, 100)) {
		drawCircle = true;
		circle_index = mars.getMesh(0).getVertex(n->indices[0]);
		return;
	}
	//checks for intersection with child
	for (TreeNode* node : n->child) {
		if (node->parent.intersect(r, -100, 100)) {
			intersection_test(r, node, mesh);
		}
	}

}
//draw the octree using drawBox
void ofApp::drawOctreeBox(TreeNode* t, int levels) {
	if (levels == 0) return;
	ofSetColor(255 % (levels * 3), 255 % (levels * 10), 255 % (levels * 5));
	drawBox(t->parent);
	for (TreeNode* temp : t->child)
		drawOctreeBox(temp, levels - 1);

}

//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}


// making an octree
void ofApp::octree(int level, TreeNode* n, const ofMesh &mesh) {
	//base case, once levels run out
	if (level <= 0 || n->indices.empty()) return;
	TreeNode* node = n;
	vector<Box> temp;
	subDivideBox8(node->parent, temp);
	TreeNode* child[8];
	//create the node for each child box
	for (int i = 0; i < 8; i++) {
		TreeNode* t = new TreeNode();
		t->parent = temp[i];
		child[i] = t;
	}
	//loop through all indices and assign them to their approiate boxes
	ofVec3f index;
	//printf("indices = %i\n", node->indices.size());

	for (int i = 0; i < node->indices.size(); i++) {
		index = mesh.getVertex(node->indices[i]);
		for (int j = 0; j < 8; j++) {
			if (in_box(index, child[j])) {
				child[j]->indices.push_back(node->indices[i]);
				break;
			}
		}
	}
	int drop = level;
	//add all boxes that have indices
	for (int i = 0; i < 8; i++) {
		if (!child[i]->indices.empty()) {
			node->child.push_back(child[i]);
			octree(drop - 1, child[i], mesh);
		}
	}

	//printf("level : %i children : %i\n", level, node->child.size());
}

//check if vertex is in the box
bool ofApp::in_box(ofVec3f vertex, TreeNode* node) {
	if (vertex.x > node->parent.min().x() && vertex.x < node->parent.max().x()
		&& vertex.y > node->parent.min().y() && vertex.y < node->parent.max().y()
		&& vertex.z > node->parent.min().z() && vertex.z < node->parent.max().z()) {
		return true;
	}
	return false;
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}


//
//  Select Target Point on Terrain by comparing distance of mouse to 
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//
bool ofApp::doPointSelection() {

	ofMesh mesh = mars.getMesh(0);
	int n = mesh.getNumVertices();
	float nearestDistance = 0;
	int nearestIndex = 0;

	bPointSelected = false;

	ofVec2f mouse(mouseX, mouseY);
	vector<ofVec3f> selection;

	// We check through the mesh vertices to see which ones
	// are "close" to the mouse point in screen space.  If we find 
	// points that are close, we store them in a vector (dynamic array)
	//
	for (int i = 0; i < n; i++) {
		ofVec3f vert = mesh.getVertex(i);
		ofVec3f posScreen = cam.worldToScreen(vert);
		float distance = posScreen.distance(mouse);
		if (distance < selectionRange) {
			selection.push_back(vert);
			bPointSelected = true;
		}
	}

	//  if we found selected points, we need to determine which
	//  one is closest to the eye (camera). That one is our selected target.
	//
	if (bPointSelected) {
		float distance = 0;
		for (int i = 0; i < selection.size(); i++) {
			ofVec3f point = cam.worldToCamera(selection[i]);

			// In camera space, the camera is at (0,0,0), so distance from 
			// the camera is simply the length of the point vector
			//
			float curDist = point.length();

			if (i == 0 || curDist < distance) {
				distance = curDist;
				selectedPoint = selection[i];
			}
		}
	}
	return bPointSelected;
}

// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);

	if (rover.loadModel(dragInfo.files[0])) {
		rover.setScaleNormalization(false);
		rover.setScale(.005, .005, .005);
		rover.setPosition(point.x, point.y, point.z);
		bRoverLoaded = true;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}