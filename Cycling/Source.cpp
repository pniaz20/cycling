/***************************************************************
 * Cycling animation of skeleton
 * Pouya Pourakbarian Niaz
 * email: pniaz20@ku.edu.tr
 ***************************************************************/

 //File Input Headers for reading from VRML file
#include <Inventor/SoInput.h>

 
 // SoWin headers for Scene Viewer:
#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/Win/SoWinRenderArea.h>

// Node headers for creating/manipulating nodes:
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoMaterial.h>


// Action headers for creating/manipulating actions e.g. writing to a file:
#include <Inventor/actions/SoWriteAction.h>

// Event headers for creating/manipulating events e.g. pressing a keybord button:
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

// Sensor headers for creating/manipulating sensors e.g. a timer sensor for animating objects:
#include <Inventor/sensors/SoSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>

// Basic C++ headers:
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

// Numerical Recepies headers
#include "nrutil.h"

// Defining namespace to be used throughout the program, e.g. for 'cout' commands:
using namespace std;

// Global definitions:
float rate = 0.0;                                           // Angular velocity coefficient of animation
float targetRate = 0.0;                                     // Target angular velocity coefficient
int accel = 0;                                              // Angular acceleration determiner of animation
bool killSwitch = FALSE;                                    // Key to determine sudden stop
float accelRate = 0.0;                                      // Angular acceleration aoefficient
float diff = 0.0;                                           // Difference btw current & desired angular velocities

class angles {
public:
    float Rcrank = 0.0;
    float Lcrank = Rcrank + M_PI;
    SoTransform* RfemurXform = new SoTransform;
    SoTransform* RtibiaXform = new SoTransform;
    SoTransform* RfootXform = new SoTransform;
    SoTransform* LfemurXform = new SoTransform;
    SoTransform* LtibiaXform = new SoTransform;
    SoTransform* LfootXform = new SoTransform;
    void setAngles(float Rcrank,float Lcrank)
    {
        RfemurXform->rotation.setValue(SbVec3f(-1, 0, 0), (50.0 + 20.0 * cosf(Rcrank)) * M_PI / 180.0);
        RtibiaXform->rotation.setValue(SbVec3f(1, 0, 0), (80.0 + 35.0 * cosf(Rcrank + M_PI / 6.0)) * M_PI / 180.0);
        RfootXform->rotation.setValue(SbVec3f(-1, 0, 0), (-10.0 - 10.0 * cosf(Rcrank - M_PI / 6.0)) * M_PI / 180.0);
        LfemurXform->rotation.setValue(SbVec3f(-1, 0, 0), (50.0 + 20.0 * cosf(Lcrank)) * M_PI / 180.0);
        LtibiaXform->rotation.setValue(SbVec3f(1, 0, 0), (80.0 + 35.0 * cosf(Lcrank + M_PI / 6.0)) * M_PI / 180.0);
        LfootXform->rotation.setValue(SbVec3f(-1, 0, 0), (-10.0 - 10.0 * cosf(Lcrank - M_PI / 6.0)) * M_PI / 180.0);
    }
    void setCenters(float** Rcenters,float** Lcenters)
    {
        RfemurXform->center.setValue(Rcenters[1][1], Rcenters[1][2], Rcenters[1][3]);
        RtibiaXform->center.setValue(Rcenters[2][1], Rcenters[2][2], Rcenters[2][3]);
        RfootXform->center.setValue(Rcenters[3][1], Rcenters[3][2], Rcenters[3][3]);
        LfemurXform->center.setValue(Lcenters[1][1], Lcenters[1][2], Lcenters[1][3]);
        LtibiaXform->center.setValue(Lcenters[2][1], Lcenters[2][2], Lcenters[2][3]);
        LfootXform->center.setValue(Lcenters[3][1], Lcenters[3][2], Lcenters[3][3]);
    }
};

// User-Defined Global Parameters:
float slow = 0.05;                                          // Slow Acceleration Coefficient
float fast = 0.5;                                           // Fast Acceleration Coefficient
int fps = 30;                                               // Animation frame rate (Frames Per Second)
float rateIncr = 2.0;                                       // Target Angular Velocity Increment at Key Press


/***************************************  Describing Key Press Event Callback Function:  *******************************************/

void myKeyPressCB(void* userData, SoEventCallback* eventCB)
{
    void rotSensorCB(void* data, SoSensor*);                // Function prototype for SOTIMERSENSOR callback function
    SoTimerSensor* tSensor = (SoTimerSensor*)userData;      // Treat USERDATA like an SOTIMERSENSOR
    const SoEvent* event = eventCB->getEvent();             // Find out how event is defined

    tSensor->setInterval(1.0 / fps);                        // Maintain FPS

    if (SO_KEY_PRESS_EVENT(event, UP_ARROW))                // If UP ARROW KEY is pressed
    {
        tSensor->unschedule();                              // Reset timer
        killSwitch = FALSE;                                 // Continue animation
        targetRate += rateIncr;                             // Increase turn rate by one increment
        cout << ">>> Speed set to: " << (targetRate * fps / 6) << " RPM." << endl;
        accelRate = slow;                                   // Set acceleration rate to slow
        tSensor->setFunction(rotSensorCB);                  // Set timer sensor callback function
        tSensor->schedule();                                // Schedule timer sensor
        eventCB->setHandled();                              // Event was handled
    }
    else if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW))         
    {
        tSensor->unschedule();                              
        killSwitch = FALSE;
        targetRate -= rateIncr;                             // Decrease turn rate by one increment
        cout << ">>> Speed set to: " << (targetRate * fps / 6) << " RPM." << endl;
        accelRate = slow;
        tSensor->setFunction(rotSensorCB);                  
        tSensor->schedule();                                
        eventCB->setHandled();	                            
    }
    else if (SO_KEY_PRESS_EVENT(event, LEFT_ARROW))         
    {
        tSensor->unschedule();                              
        killSwitch = FALSE;
        targetRate = 0.0;                                   // Stop the model
        cout << ">>> Speed set to: " << (targetRate * fps / 6) << " RPM." << endl;
        accelRate = fast;                                   // Set acceleration rate to FAST
        tSensor->setFunction(rotSensorCB);                  
        tSensor->schedule();                                
        eventCB->setHandled();	                            
    }
    else if (SO_KEY_PRESS_EVENT(event, RIGHT_ARROW))        
    {
        tSensor->unschedule();                              
        killSwitch = TRUE;                                  // Kill animation
        targetRate = 0;                                     // Reset TARGETRATE to zero
        cout << ">>> Animation stopped. Model position reset." << endl;
        tSensor->setFunction(rotSensorCB);                  
        tSensor->schedule();                                
        eventCB->setHandled();	                            
    }
    /* Note that you cannot set the function for the timer sensor 
    and set the event to 'handled' here. It has to be done within each case of the IF ELSE block.*/
}

/**********************************  Describing Rotation Timer Sensor Callback Function:  ********************************/

void rotSensorCB(void* data, SoSensor*)
{
    angles* rots = (angles*)data;                           // treat DATA like ANGLES class

    diff = targetRate - rate;                               // Evaluate velocity deficit
    accel = (diff > 0.01) ? 1 : (diff < -0.01) ? -1 : 0;    // Decide if acceleration or decelaration should happen
    rate += accel * accelRate;                              // Change velocity value according to acceleration rate

    if (killSwitch == FALSE) //keep going
    {
        rots->Rcrank += rate * M_PI / 180.0;
        rots->Lcrank += rate * M_PI / 180.0;
        rots->setAngles(rots->Rcrank,rots->Lcrank);
    }
    else //stop and reset model
    {
        // Return to base:
        rots->Rcrank = 0.0;
        rots->Lcrank = M_PI;
        rots->setAngles(rots->Rcrank, rots->Lcrank);
        // Reset velocity coefficient
        rate = 0.0;                                        
    }
}


/***************************************************  The Main Function:  ****************************************************
******************************************************************************************************************************/

int main(int, char** argv)
{
    float** Rcenters = matrix(1, 3, 1, 3);
    float** Lcenters = matrix(1, 3, 1, 3);

    Rcenters[1][1] = -0.40 / 1000;
    Rcenters[1][2] = 0.11 / 1000;
    Rcenters[1][3] = -0.12 / 1000;

    Rcenters[2][1] = -0.32 / 1000;
    Rcenters[2][2] = -1.66 / 1000;
    Rcenters[2][3] = -0.18 / 1000;

    Rcenters[3][1] = -0.24 / 1000;
    Rcenters[3][2] = -3.51 / 1000;
    Rcenters[3][3] = -0.21 / 1000;

    Lcenters[1][1] = 0.40 / 1000;
    Lcenters[1][2] = 0.11 / 1000;
    Lcenters[1][3] = -0.12 / 1000;

    Lcenters[2][1] = 0.32 / 1000;
    Lcenters[2][2] = -1.66 / 1000;
    Lcenters[2][3] = -0.18 / 1000;

    Lcenters[3][1] = 0.24 / 1000;
    Lcenters[3][2] = -3.51 / 1000;
    Lcenters[3][3] = -0.21 / 1000;

    // Initializing the Viewer Window:
    HWND window = SoWin::init(argv[0]);                     /*'HWND' is SoWin equivalent of
                                                            'Widget' in SoXt-using programs.*/
    if (window == NULL)
    {
        cerr << ">>> Viewer window failed to initialize. Exiting." << endl;
        exit(1);

    }
    else
        cout << ">>> Viewer Window Initialized." << endl;

    // Initializing Viewer:
    SoWinExaminerViewer* viewer =                           /*The scene viewer object is what will be used for
                                                            rendering the scene graph. */
        new SoWinExaminerViewer(window);

    // Generate and Refer to Root Node:
    SoSeparator* root = new SoSeparator;
    root->setName("Root Node");
    root->ref();                                            /* The root node had better be manually referenced in order
                                                            not to leave it unreferenced, since it has no parent node
                                                            referring to it. */
    cout << ">>> Root node referenced." << endl;

   /***************************************  Separator and Group Declarations  **********************************************/

    SoSeparator* rightSide = new SoSeparator;               rightSide->setName("Right Side Separator Node");
    SoSeparator* leftSide = new SoSeparator;                leftSide->setName("Left Side Separator Node");

    SoSeparator* RpelvisSep = new SoSeparator;              RpelvisSep->setName("Right Pelvis Separator Node");
    SoSeparator* Rpelvis = new SoSeparator;                 Rpelvis->setName("Right Pelvis Shape Node");
    SoSeparator* RfemurSep = new SoSeparator;               RfemurSep->setName("Right Femur Separator Node");
    SoSeparator* Rfemur = new SoSeparator;                  Rfemur->setName("Right Femur Shape Node");
    SoSeparator* RtibiaSep = new SoSeparator;               RtibiaSep->setName("Right Tibia Separator Node");
    SoSeparator* Rtibia = new SoSeparator;                  Rtibia->setName("Right Tibia Shape Node");
    SoSeparator* RfootSep = new SoSeparator;                RfootSep->setName("Right Foot Separator Node");
    SoSeparator* Rfoot = new SoSeparator;                   Rfoot->setName("Right Foot Shape Node");

    SoSeparator* LpelvisSep = new SoSeparator;              LpelvisSep->setName("Left Pelvis Separator Node");
    SoSeparator* Lpelvis = new SoSeparator;                 Lpelvis->setName("Left Pelvis Shape Node");
    SoSeparator* LfemurSep = new SoSeparator;               LfemurSep->setName("Left Femur Separator Node");
    SoSeparator* Lfemur = new SoSeparator;                  Lfemur->setName("Left Femur Shape Node");
    SoSeparator* LtibiaSep = new SoSeparator;               LtibiaSep->setName("Left Tibia Separator Node");
    SoSeparator* Ltibia = new SoSeparator;                  Ltibia->setName("Left Tibia Shape Node");
    SoSeparator* LfootSep = new SoSeparator;                LfootSep->setName("Left Foot Separator Node");
    SoSeparator* Lfoot = new SoSeparator;                   Lfoot->setName("Left Foot Shape Node");

    SoGroup* viewGroup = new SoGroup;                       viewGroup->setName("Viewing Group Node");


    /********************************************************  NODES  *******************************************************/
    /*Open Inventor and Coin3D have VRML 1.0 and VRML 2.0 support. */

    //VRML Geometries
    SoInput RpelvisInput;
    if (!RpelvisInput.openFile("right_pelvis.wrl")) {  // Open VRML file 
        cout << "\n>>> Cannot open Right Pelvis VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Rpelvis = SoDB::readAll(&RpelvisInput);
        cout << "\n>>> Right Pelvis successfully imported from VRML file." << endl;
    }
    RpelvisInput.closeFile();

    SoInput RfemurInput;
    if (!RfemurInput.openFile("right_femur.wrl")) {
        cout << "\n>>> Cannot open Right Femur VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Rfemur = SoDB::readAll(&RfemurInput);
        cout << ">>> Right Femur successfully imported from VRML file." << endl;
    }
    RfemurInput.closeFile();

    SoInput RtibiaInput;
    if (!RtibiaInput.openFile("right_tibia.wrl")) {
        cout << "\n>>> Cannot open Right Tibia VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Rtibia = SoDB::readAll(&RtibiaInput);
        cout << ">>> Right Tibia successfully imported from VRML file." << endl;
    }
    RtibiaInput.closeFile();

    SoInput RfootInput;
    if (!RfootInput.openFile("right_foot.wrl")) {
        cout << "\n>>> Cannot open Right Foot VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Rfoot = SoDB::readAll(&RfootInput);
        cout << ">>> Right Foot successfully imported from VRML file.\n" << endl;
    }
    RfootInput.closeFile();

    SoInput LpelvisInput;
    if (!LpelvisInput.openFile("left_pelvis.wrl")) {  // Open VRML file 
        cout << "\n>>> Cannot open Left Pelvis VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Lpelvis = SoDB::readAll(&LpelvisInput);
        cout << "\n>>> Left Pelvis successfully imported from VRML file." << endl;
    }
    LpelvisInput.closeFile();

    SoInput LfemurInput;
    if (!LfemurInput.openFile("left_femur.wrl")) {
        cout << "\n>>> Cannot open Left Femur VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Lfemur = SoDB::readAll(&LfemurInput);
        cout << ">>> Left Femur successfully imported from VRML file." << endl;
    }
    LfemurInput.closeFile();

    SoInput LtibiaInput;
    if (!LtibiaInput.openFile("left_tibia.wrl")) {
        cout << "\n>>> Cannot open Left Tibia VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Ltibia = SoDB::readAll(&LtibiaInput);
        cout << ">>> Left Tibia successfully imported from VRML file." << endl;
    }
    LtibiaInput.closeFile();

    SoInput LfootInput;
    if (!LfootInput.openFile("left_foot.wrl")) {
        cout << "\n>>> Cannot open Left Foot VRML file, replacing it with empty scene." << endl;
    }
    else
    {
        Lfoot = SoDB::readAll(&LfootInput);
        cout << ">>> Left Foot successfully imported from VRML file.\n" << endl;
    }
    LfootInput.closeFile();

    //Window Camera
    SoPerspectiveCamera* cam = new SoPerspectiveCamera;     /* This camera is used for getting a better, slightly
                                                            zoomed-out view of the model. */

    // Transformations:
    SoTransform* globalTrans = new SoTransform;             // The global transformation, just in case the whole model needs to be turned
    globalTrans->setName("Global Transformation");
    globalTrans->rotation.setValue(SbVec3f(0, 1, 0), M_PI_2);

    SoTransform* RpelvisTrans = new SoTransform;              
    RpelvisTrans->setName("Right Pelvis Transformation");

    SoTransform* LpelvisTrans = new SoTransform;
    LpelvisTrans->setName("Left Pelvis Transformation");

    // Materials:
    SoMaterial* gold = new SoMaterial;
    gold->setName("Gold Material Node");
    gold->shininess = 1.0;
    gold->diffuseColor.setValue(218.0/255.0, 165.0/255.0, 32.0/255.0);

    SoMaterial* red = new SoMaterial;
    red->setName("Red Material Node");
    red->diffuseColor.setValue(0.5, 0.0, 0.0);
    red->shininess = 1.0;

    SoMaterial* blue = new SoMaterial;
    blue->setName("Blue Material Node");
    blue->diffuseColor.setValue(0.0, 0.0, 0.5);
    blue->shininess = 1.0;

    cout << ">>> Model components built." << endl;

    angles* myRots = new angles;
    myRots->Rcrank = 0.0;
    myRots->Lcrank = M_PI;
    myRots->setAngles(myRots->Rcrank,myRots->Lcrank);
    myRots->setCenters(Rcenters,Lcenters);
    myRots->RfemurXform->setName("Right Femur Transformation");
    myRots->RtibiaXform->setName("Right Tibia Transformation");
    myRots->RfootXform->setName("Right Foot Transformation");
    myRots->LfemurXform->setName("Left Femur Transformation");
    myRots->LtibiaXform->setName("Left Tibia Transformation");
    myRots->LfootXform->setName("Left Foot Transformation");
    
    // Declare the Timer Sensor:
    SoTimerSensor* rotatingSensor = new SoTimerSensor();    /* Note that CALLBACK FUNCTION and USERDATA to be used
                                                            could also have been declared inline here along with timer
                                                            sensor itself, but that would not help us with our goals. */

    rotatingSensor->setData(myRots);                        /* Set USERDATA that will be used/manipulated/affected
                                                            by this particular timer sensor's callback function. */

    
    // Declare the Key Press Event Callback:
    SoEventCallback* myEventCB = new SoEventCallback;
    myEventCB->setName("My Event Callback");
    myEventCB->                                             // Add event callback function, object, and user data.
        addEventCallback(
            SoKeyboardEvent::getClassTypeId(),              // Automatically get class type of keyboard event.
            myKeyPressCB,                                   // Event Callback function to be used for this event node.
            rotatingSensor);                                /* User data that will be used/affected/manipulated by this event's
                                                            callback function. */

    /*********************************************  Setting up the Scene Graph  *********************************************/
    
    root->addChild(viewGroup);
    root->addChild(rightSide);
    root->addChild(leftSide);

    viewGroup->addChild(cam);
    viewGroup->addChild(globalTrans);
    viewGroup->addChild(myEventCB);

    rightSide->addChild(RpelvisTrans);
    rightSide->addChild(RpelvisSep);
    rightSide->addChild(myRots->RfemurXform);
    rightSide->addChild(RfemurSep);
    rightSide->addChild(myRots->RtibiaXform);
    rightSide->addChild(RtibiaSep);
    rightSide->addChild(myRots->RfootXform);
    rightSide->addChild(RfootSep);

    leftSide->addChild(LpelvisTrans);
    leftSide->addChild(LpelvisSep);
    leftSide->addChild(myRots->LfemurXform);
    leftSide->addChild(LfemurSep);
    leftSide->addChild(myRots->LtibiaXform);
    leftSide->addChild(LtibiaSep);
    leftSide->addChild(myRots->LfootXform);
    leftSide->addChild(LfootSep);

    RpelvisSep->addChild(Rpelvis);
    RfemurSep->addChild(Rfemur);
    RtibiaSep->addChild(Rtibia);
    RfootSep->addChild(Rfoot);
    LpelvisSep->addChild(Lpelvis);
    LfemurSep->addChild(Lfemur);
    LtibiaSep->addChild(Ltibia);
    LfootSep->addChild(Lfoot);

    Rpelvis->addChild(gold);
    Lpelvis->addChild(gold);
    Rfemur->addChild(red);
    Rtibia->addChild(red);
    Rfoot->addChild(red);
    Lfemur->addChild(blue);
    Ltibia->addChild(blue);
    Lfoot->addChild(blue);

    cout << ">>> Scene graph generated." << endl;

    /************************************************  Setting up the viewer ************************************************/

    cout << ">>> Setting up the viewer ..." << endl;

    viewer->setSize(SbVec2s(750, 750));                     // Set size of viewer window in pixels
    SbViewportRegion myRegion(viewer->getSize());           // Get camera viewport region.
    cam->viewAll(root, myRegion);                           // Set camera to view the whole region.
    SbVec3f initialPos;
    initialPos = cam->position.getValue();                  // Get current view point position.
    float x, y, z;
    initialPos.getValue(x, y, z);                           // Decompose vector to its components
    cam->position.setValue(x*5.0, y*1.5, z);                // Zoom back a little to see the whole model better

    viewer->setSceneGraph(root);                            // Set the (part of the) scene graph to be viewed
    viewer->setTitle("Pouya Pourakbarian Niaz - Cycling");
    float a, b, c;
    a = 207.0 / 255.0;
    b = 231.0 / 255.0;
    c = 253.0 / 255.0;
    viewer->setBackgroundColor(SbColor(a, b, c));           // Set viewer background color to light blue

    cout << ">>> Viewer set up and ready.\n" << endl;
    cout << ">>> Wait for a few seconds, then press Enter to launch the viewer: ";
    cin.get(); // press enter

    viewer->show();                                         

    SoWin::show(window);                                    
    SoWin::mainLoop();

    // Once the viewer's window is manually closed by the user...
    cout << "\n>>> Viewer window manually closed by the user." << endl;

    /*******************************************  Writing to an IV file  *****************************************************/

    cout << ">>> Generating IV file..." << endl;
    SoWriteAction myAction;
    myAction.getOutput()->openFile("Cycling.iv");
    myAction.getOutput()->setBinary(FALSE);
    myAction.apply(root);
    myAction.getOutput()->closeFile();
    cout << endl << ">>> Output file generated as Cycling.iv\n" << endl;

    cout << ">>> The Program is terminated. GOOD BYE!\n" << endl;
    return 0;
}
