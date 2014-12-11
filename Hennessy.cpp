//NOTE TO SELF:
//  Maybe add something that resets picture counter when hit by flare.



//--------------------------------------VARIABLES--------------------------------------//
//Positioning
ZRState myState;
int teamColor;
int side;
float myPos[3];
float zero[3];
float earth[3];
float darkZone[3];
int targetPOI;
float poiPos[3];
float posTarget[3];

//Counters
int time;
float targetScore;

void init()
{
    //----Coordinates & Positioning----//
    zero[0]=0.0;
	zero[1]=0.0;
	zero[2]=0.0;
	
	earth[0]=0.64;
	earth[1]=0.0;
	earth[2]=0.0;
	
	darkZone[0]=0.55;
	darkZone[1]=0.1;
	darkZone[2]=-0.15;
	
	api.getMyZRState(myState);
	if (myState[1] > 0)
	{
	    teamColor = 0;
	    side = 1; //Y-coord multiplier
	    DEBUG(("We are Blue"));
	}
	else
	{
	    teamColor = 1;
	    side = -1; //Y-coord multiplier
	    DEBUG(("We are Red"));
	}
	
	for (int i = 0; i<3; i++) {
        myPos[i] = myState[i];
    }
	
	//----Targeting----//
    
	targetPOI = teamColor;
	
	//----Counters----//
	time = 0;
	targetScore = 30;
}

void loop()
{
    DEBUG(("\n  targetPOI: %d", targetPOI));
    
    //----Base Functions----//
    time = api.getTime();
    stayInBounds();
    
    api.getMyZRState(myState);
    for (int i = 0; i<3; i++) {
        myPos[i] = myState[i];
    }
    
    if (game.getFuelRemaining() <= 0)
    {
        game.turnOff();
        DEBUG(("\nGame over man, game over!"));
    }
    else if (game.getNextFlare() < 27 && game.getNextFlare() !=-1)
	{
	    game.turnOn();
	    api.setPositionTarget(darkZone);
        upload();
        game.getPOILoc(poiPos, targetPOI);
	}
    else
    {
        game.turnOn();
        game.getPOILoc(poiPos, targetPOI);
        
        float predictedPOIPos[3] = {-0.05, poiPos[1], -sqrtf(0.04 - powf(poiPos[1], 2.0))}; 
    
        for (int i = 0; i < 3; i++)
        {
           posTarget[i] = predictedPOIPos[i] * 2.3;
        }
        
        api.setPositionTarget(posTarget);

        facePos(predictedPOIPos, myPos);
    }
    
    //----TargetPOI----//
    /*float predictedPOIPos[3] = {0.0, poiPos[1], sqrtf(0.04 - powf(poiPos[1], 2.0))}; 
    
    for (int i = 0; i < 3; i++)
    {
        posTarget[i] = predictedPOIPos[i] * 1.95;
    }

    facePos(predictedPOIPos, myPos);*/

    
    if (game.getMemoryFilled() == game.getMemorySize()) 
    {  //If SPHERE memory is full with valid pictures
        DEBUG(("\n  Moving to upload"));
        upload();
    }
}

//--------------------------------------HELPER METHODS-------------------------------------------//
void facePos(float target[3], float myPos[3])
{
	//Rotate to face POI
	float attTarget[3];
		
	mathVecSubtract(attTarget,target,myPos,3);
	mathVecNormalize(attTarget,3);	
	
	api.setAttitudeTarget(attTarget);
	
    if (distanceVec(myPos, target)< 0.50)        
    {
    //DEBUG(("The SPHERE is close enough to take a picture. "));    
        //if (game.alignLine(targetPOI)==true)
        //{      
            game.takePic(targetPOI);
        //}
    }
}
	
float distanceVec(float a[3], float b[3]) 
{  //finds distance between two objects
	float diff[3];
	mathVecSubtract(diff,a,b,3);
	return mathVecMagnitude(diff,3);
}

void stayInBounds() 
{ 
	float force[3] = {0.00001,0.00001,0.00001};
	//Tweak the numerical values as necessary based on the game boundaries.
	if (myState[0] >=0.62 || myState[0] <=-0.62) {
		force[0] = -myState[0];
		DEBUG(("\n  Force X"));
		
	}
	if (myState[1] >=0.76 || myState[1] <=-0.76) {
			force[1] = -myState[1];
				DEBUG(("\n  Force Y"));  
	}
	if (myState[2] >=0.62 || myState[2] <=-0.62) {
			force[2] = -myState[2];
					DEBUG(("\n  Force Z"));  
	}
	api.setForces(force);
}
	
float findMin(float a, float b)
{
    if (a < b)
    {
        return a;
    }
    else if (a == b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

int findClosestPOI(float myPos[3])
{
    float poi0[3];
    float poi1[3];
    float poi2[3];
    
	/*game.getPOILoc(poi0, 0);
	float aDistance = distanceVec(poi0, myPos);
	game.getPOILoc(poi1, 1);
	float bDistance = distanceVec(poi1, myPos);
	game.getPOILoc(poi2, 2);
	float cDistance = distanceVec(poi2, myPos);
	*/
	
	game.getPOILoc(poi0, 0);
    float aDistance = atan2(poi0[2],poi0[0]);
    game.getPOILoc(poi1, 1);
    float bDistance = atan2(poi1[2],poi1[0]);
    game.getPOILoc(poi2, 2);
    float cDistance = atan2(poi2[2],poi2[0]);

	if (findMin(aDistance, findMin(bDistance, cDistance)) == aDistance)
	{
	    return 0;
	}
	else if (findMin(aDistance, findMin(bDistance, cDistance)) == bDistance)
	{
	    return 1;
	}
	else if (findMin(aDistance, findMin(bDistance, cDistance)) == cDistance)
	{
	    return 2;
	}
	else
	{
	    DEBUG(("\n   Targeting error has occurred :("));
	}
}

void upload() 
{
    float attTarget[3];
    
	mathVecSubtract(attTarget,earth,myPos,3);
	mathVecNormalize(attTarget,3);
	
	api.setAttitudeTarget(attTarget); //Rotate to face Earth
	
    float oldScore = game.getScore();
    game.uploadPic();
    if (oldScore < game.getScore())
    {
       DEBUG(("Upload was successful!"));
    }
    if (oldScore >= game.getScore())
    {
        DEBUG(("Upload failed!"));
    }
}
