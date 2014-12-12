//--------------------------------------VARIABLES--------------------------------------//

//Positioning
ZRState myState;
ZRState otherState;

int teamColor;
int side;
int spyLoc;
float myPos[3];
float otherPos[3];
float otherXPos;
float zero[3];
float earth[3];
float darkZone[3];
int targetPOI;
float poiPos[3];
float posTarget[3];
float poiSpyLoc[3];
float predictedPOIPos[3];

//Counters
int time;
int picturesTaken;
int flareCounter;

void init()
{
    //----Coordinates & Positioning----//
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
	
	spyLoc = -1;
	
    zero[0]=0.0;
	zero[1]=0.0;
	zero[2]=0.0;
	
	earth[0]=0.64;
	earth[1]=0.0;
	earth[2]=0.0;

	darkZone[0]= 0.4;
	darkZone[1]= side * 0.05;
	darkZone[2]= spyLoc * 0.05;
	
	for (int i = 0; i<3; i++) 
	{
        myPos[i] = myState[i];
    }
	
	//----Targeting----//
	targetPOI = teamColor;
	
	//----Counters----//
	time = 0;
	picturesTaken = 0;
	flareCounter = 0;
}

void loop()
{
    //DEBUG(("\n  targetPOI: %d", targetPOI));
    
    //----Base Functions----//
    time = api.getTime();
    
    api.getMyZRState(myState);
    for (int i = 0; i<3; i++) 
    {
        myPos[i] = myState[i];
    }
    
    //----Targetting----//
    game.getPOILoc(poiPos, targetPOI);
    
    predictedPOIPos[0] = -0.01;
    predictedPOIPos[1] = poiPos[1];
    predictedPOIPos[2] = spyLoc * sqrtf(0.04 - powf(poiPos[1], 2.0)); 
    
    if (time % 60 == 0)
    {
        game.getPOILoc(poiPos, targetPOI);
        
        for (int i = 0; i < 3; i++)
        {
            posTarget[i] = predictedPOIPos[i] * 2.25;
        }
        
    }
    
    darkZone[2]= spyLoc * 0.05;
    
    if (game.getMemoryFilled() == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            posTarget[i] = predictedPOIPos[i] * 2.25;
        }
    }
    else if (game.getMemoryFilled() == 1)
    {
       for (int i = 0; i < 3; i++)
        {
            posTarget[i] = predictedPOIPos[i] * 1.95;
        } 
    }
    else
    {
        upload();
    }
    
    //----Decision Making----//
    if (game.getNextFlare() == 29)
    {
        flareCounter++;
        DEBUG(("#Flares: %d", flareCounter));
    }
    
    if (game.getFuelRemaining() <= 0)
    {
        game.turnOff();
        DEBUG(("\nGame over man, game over!"));
    }
    else if (time >= 235)
    {
        upload();
    }
    else if (game.getNextFlare() <= 22 && game.getNextFlare() != -1)
	{
	    game.getPOILoc(poiSpyLoc, targetPOI);
	    
	    if (poiSpyLoc[2] > 0.15)
        {
            spyLoc = 1;
        }
        else
        {
            spyLoc = -1;
        }
        
        darkZone[2]= spyLoc * 0.05;
        
        arcMove(darkZone);

	    if (flareCounter == 1) //Get other DZ pos
        {
            
            if (game.getNextFlare() == 1)
            {
                //darkZone[0] = checkDZ();
                checkDZ();
            }
        }

        if (game.getMemoryFilled() > 0)
        {
            upload();
        }
        else
        {
            facePos(predictedPOIPos, myPos);
        }
        
	}
    else
    {
        if (game.getMemoryFilled() == game.getMemorySize())
        {
            upload();
        }
        else
        {
            facePos(predictedPOIPos, myPos);
        }
        arcMove(posTarget);
    }
}

//--------------------------------------HELPER METHODS-------------------------------------------//
void checkDZ()
{
    api.getOtherZRState(otherState);
    
    for (int i = 0; i < 3; i++)
    {
        otherPos[i] = otherState[i];
    }
    
    
    mathVecNormalize(otherPos, 3);
    
    for (int i = 0; i < 3; i++)
    {
        otherPos[i] *= 0.20;
    }
    
    darkZone[0] = 0.4;
    darkZone[1] = otherPos[1];
    darkZone[2] = otherPos[2];
}


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
        if (game.alignLine(targetPOI)==true)
        {      
            game.takePic(targetPOI);
        }
    }
}
	
float distanceVec(float a[3], float b[3]) 
{  //finds distance between two objects
	float diff[3];
	mathVecSubtract(diff,a,b,3);
	return mathVecMagnitude(diff,3);
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

void arcMove(float posTarget2[3])
{
    float midpoint[3] = {(myPos[0]+posTarget2[0])/2, (myPos[1]+posTarget2[1])/2, (myPos[2]+posTarget2[2])/2};
    if (mathVecMagnitude(midpoint,3) < 0.35) {
        mathVecNormalize(midpoint,3);
     	for (int i = 0; i<3; i++) {
    	 	midpoint[i] *= 0.49;
     	}
     	api.setPositionTarget(midpoint);
     	//DEBUG((" | Heading to waypoint | "));
    }
    else {
        api.setPositionTarget(posTarget2);
    }
}

