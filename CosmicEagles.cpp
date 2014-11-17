#include <math.h>
//all angles are in radians!!!!

float myPos[12];
float normalVec[3];
float poiPos[3];
float upload[3];
float diffVec[3];
int positionGoal;
int t;
int tempTime;
//-2 means the outer pic has been taken; -1 means both have been taken
int listOfPOIs[3];

void init(){
    game.turnOn();
    //INITIALIZING VARIABLES
    t = 0;
    tempTime = 0;
    myPos[12]={};
    normalVec[3]={};
    diffVec[3]={};
    listOfPOIs[0]=0;
    listOfPOIs[1]=1;
    listOfPOIs[2]=2;
    
    poiPos[3]={};
    upload[3]={};
    positionGoal=0;
    api.getMyZRState(myPos);
    //GETTING POI LOCATION
    game.getPOILoc(poiPos,0);
    //FINDING CLOSEST POI
    for (int i=1; i<game.numActivePOIs(); i++) {
        float tempPOIPos[3] = {};
        game.getPOILoc(tempPOIPos,i);
        float dis1=sqrt(mathSquare(tempPOIPos[0]-myPos[0])+mathSquare(tempPOIPos[1]-myPos[1]));
        float dis2=sqrt(mathSquare(poiPos[0]-myPos[0])+mathSquare(poiPos[1]-myPos[1]));
        if (dis1<dis2){
            game.getPOILoc(poiPos,i);
            positionGoal = i;
            DEBUG(("\nswitched to %d POI", i));
        }
    }
    
}

void loop(){
    api.getMyZRState(myPos);
    t = api.getTime();
    int numOfValPics = game.getMemoryFilled();
    bool shouldUpload = false;
    if (t%60 == 0) {
        listOfPOIs[0]=0;
        listOfPOIs[1]=1;
        listOfPOIs[2]=2;
        if (numOfValPics>0) {
            shouldUpload = true;
        }
    }
    //GOES TO .41 AWAY FROM CENTER IN FRONT OF POI
    toNewPosition(poiPos,positionGoal, .21);
    facePOI();
    changeDiffVec(poiPos);
    api.setVelocityTarget(diffVec);
    
    //MAIN STUFFS
    if (shouldUpload == true) {
        game.uploadPic();
    }
    else if (absolute(myPos[0]-poiPos[0])<.01 && absolute(myPos[1]-poiPos[1])<.01 && numOfValPics==0){
        //DEBUG(("\nHERE2 myPos: %f %f  poiPos: %f %f",myPos[0],myPos[1],poiPos[0],poiPos[1]));
        //DEBUG(("%f %f",absolute(myPos[0]-pos2[0]),absolute(myPos[1]-pos2[1])));
        numOfValPics = game.getMemoryFilled();
        //DEBUG(("\n%d is the num of valid pics of POI %d", numOfValPics, positionGoal));
        game.takePic(positionGoal);
        //DEBUG(("\npic taken1\n%d is the num of valid pics of POI %d\nmemory %d", game.getMemoryFilled(), positionGoal, game.getMemorySize()));
        tempTime=api.getTime();
        
        float dis = sqrt(mathSquare(myPos[0])+mathSquare(myPos[1]));
        //DEBUG(("\n%f is the distance", dis));
    }
    else {
        numOfValPics = game.getMemoryFilled();
        facePOI();
        if (numOfValPics!=0){
            if (numOfValPics==1)
                listOfPOIs[positionGoal]=-2;
            facePOI();
            //DEBUG(("\ntime after 2 is %d", tempTime));
            if (numOfValPics==1 && api.getTime()-tempTime>5) {
                game.takePic(positionGoal);
                listOfPOIs[positionGoal]=-1;
                DEBUG(("\npic taken2\n%d is the num of valid pics of POI %d\nmemory %d", game.getMemoryFilled(), positionGoal, game.getMemorySize()));   
            }
            toNewPosition(upload,positionGoal,.34);
            game.uploadPic();
            changeDiffVec(upload);
            api.setVelocityTarget (diffVec);
            float dis = sqrt(mathSquare(myPos[0])+mathSquare(myPos[1]));
            //DEBUG(("\n%f is the distance", dis));
        }
        else {
            //DEBUG(("\nlist of POIs: POI 0=%d, POI 1=%d, POI 2=%d", listOfPOIs[0], listOfPOIs[1], listOfPOIs[2]));
            if (listOfPOIs[positionGoal]==-1) {
                //find next closest POI
                //DEBUG(("\nfinding next closest POI"));
                
                for (int i=0; i<game.numActivePOIs(); i++) {
                    float tempPOIPos[3] = {};
                    if (listOfPOIs[i]>=0) {
                        //DEBUG(("\nin for loop if %d", i));
                        game.getPOILoc(tempPOIPos,i);
                        float dis1=sqrt(mathSquare(tempPOIPos[0]-myPos[0])+mathSquare(tempPOIPos[1]-myPos[1]));
                        float dis2=sqrt(mathSquare(poiPos[0]-myPos[0])+mathSquare(poiPos[1]-myPos[1]));
                        if (dis1<dis2){
                            poiPos[0] = tempPOIPos[0];
                            poiPos[1] = tempPOIPos[1];
                            poiPos[2] = tempPOIPos[2];
                            positionGoal = i;
                            //DEBUG(("\nswitched to %d POI", i));
                        }
                    }
                }
            }
            changeDiffVec(poiPos);
            api.setVelocityTarget(diffVec);
            //DEBUG(("\nnew POI is %d\nmoving to new poiPos at (%f, %f, %f)", positionGoal, poiPos[0], poiPos[1], poiPos[2]));
        }
    }
}

// NEW PAGE HERE!!!!!! NEW PAGE GRACE!!!!!
void toNewPosition (float oldPOIPos[], int poiNum, float newDistanceFromPOI) {
    game.getPOILoc(oldPOIPos, poiNum);
    toPolar(oldPOIPos);
    if(oldPOIPos[0]<0)
        oldPOIPos[0]-=newDistanceFromPOI;
    else
        oldPOIPos[0]+=newDistanceFromPOI;
    toRect(oldPOIPos);
    
    poiPos[0] = oldPOIPos[0];
    poiPos[1] = oldPOIPos[1];
    poiPos[2] = oldPOIPos[2];
}

void facePOI () {
    normalVec[0] = {-myPos[0]};
    normalVec[1] = {-myPos[1]};
    normalVec[2] = {-myPos[2]};
    mathVecNormalize(normalVec, 3);
    api.setAttitudeTarget(normalVec);
}

void changeDiffVec (float pos[]) {
    mathVecSubtract(diffVec,pos,myPos,3);
    diffVec[0]*=.2f;
    diffVec[1]*=.2f;
    diffVec[2]*=.2f; 
}

// ANOTHER NEW PAGE HERE  GRACE!!!

//Calculates absolute value
float absolute(float v){
    if(v>=0)
        return v;
    else
        return -1*v;
}

//Calculates the distance between 2 points
float distance(float v1[],float v2[]){
    return sqrt(mathSquare(v2[0]-v1[0])+mathSquare(v2[1]-v1[1])+mathSquare(v2[2]-v1[2]));
}

//Changes a rectangular vector into a polar
void toPolar(float vec[]){
    float r=sqrt(mathSquare(vec[0])+mathSquare(vec[1])+mathSquare(vec[2]));
    float theta=acos((vec[2])/(r));
    float phi = 0;
    if (vec[1] == 0) {
        r=sqrt(mathSquare(vec[0])+mathSquare(vec[2]));
        theta=atan(vec[2]/vec[0]);
        if(vec[0]<0)
            r*=-1;
        phi = 0;
    }
    else if (vec[0]<0 && vec[1]>0 && vec[2]<0) {
        phi= PI+atan(vec[1]/vec[0]);
    }
    else {
        phi= PI+atan(vec[1]/vec[0]);
    }
    /*
    else if (vec[0]<=0 && vec[1]>=0 && vec[2]>=0) {
        float phi= PI+atan(vec[1]/vec[0]);
    }
    else if (vec[0]<=0 && vec[1]<=0 && vec[2]>=0) {
        float phi= PI+atan(vec[1]/vec[0]);
    }
    else if (vec[0]<=0 && vec[1]<=0 && vec[2]<=0) {
        float phi= PI+atan(vec[1]/vec[0]);
    }*/
    
    vec[0] = r;
    vec[1] = theta;
    vec[2] = phi;
}

//Changes a polar vector into a rectangular
void toRect(float vec[]){
    float x=vec[0]*sin(vec[1])*cos(vec[2]);
    float y=vec[0]*sin(vec[1])*sin(vec[2]);
    float z=vec[0]*cos(vec[1]);
    
    if (vec[2] == 0) {
        x=vec[0]*cos(vec[1]);
        y=0;
        z=vec[0]*sin(vec[1]);
    }
    
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

