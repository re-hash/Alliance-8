//--------------------------------------VARIABLES--------------------------------------//
//Positioning
ZRState myState;
ZRState otherState;
int teamColor;
float myPos[3];
float otherPos[3];
float zero[3];
int targetPOI;
float poiPos[3];
float safetyZone[3];
int pastPOIs[3];
float darkzone[3];
int countPOI;

//Counters
int time;
int picturesTaken;
int timeSinceArrival;
float targetScore;
int strategyType;

void init()
{
    strategyType=0;
    //----Coordinates & Positioning----//
    zero[0]=0.0;
	zero[1]=0.0;
	zero[2]=0.0;
	darkzone[0]=0.55;
	darkzone[1]=0.0;
	darkzone[2]=0.0;
	safetyZone[0]=0.6;
	safetyZone[1]=0.0;
	safetyZone[2]=0.0;
	
	api.getMyZRState(myState);
	if (myState[1] > 0)
	{
	    teamColor = 1;

	    DEBUG(("We are blue"));
	}
	else
	{
	    teamColor = 0;
	    DEBUG(("We are red"));
	}
	
	for (int i = 0; i<3; i++) {
        myPos[i] = myState[i];
    }
	
	//----Targeting----//
	pastPOIs[0] = -1;
    pastPOIs[1] = -1;
    pastPOIs[2] = -1;
    countPOI = 0;
    

	//cornerTurned = false;
	
	//----Counters----//
	picturesTaken = 0;
	timeSinceArrival = 0;
	time = 0;
	targetScore = 30;
}

void loop()
{
    
    //----Base Functions----//
    time = api.getTime();
    stayInBounds();
    
    api.getMyZRState(myState);
    for (int i = 0; i<3; i++) {
        myPos[i] = myState[i];
    }
    
    if (game.getFuelRemaining() <= 30)
    {
        arcMove(darkzone);
        //DEBUG(("\nGame over man, game over!"));
    }
    if (game.getFuelRemaining() <= 0)
    {
        game.turnOff();
        //DEBUG(("\nGame over man, game over!"));
    }
    
    else if (game.getNextFlare() <25 && game.getNextFlare() !=-1)
	{
	    arcMove(darkzone);
	}
    else {
        if (time > 4)
        {
        api.getOtherZRState(otherState);
        
        for (int i = 0; i<3; i++) {
            otherPos[i] = 1.08*otherState[i];
        }
        
        if (distanceVec(myPos,otherPos) < 0.38) {
            arcMove(otherPos);
            DEBUG((" | Heading to otherPos | "));
        }
        else {
            arcMove(otherState);
            DEBUG((" | Heading to otherState | "));
        }
        }
    }

}

//--------------------------------------HELPER METHODS-------------------------------------------//

	
float distanceVec(float a[3], float b[3]) {  //finds distance between two objects
	float diff[3];
	mathVecSubtract(diff,a,b,3);
	return mathVecMagnitude(diff,3);
}

void stayInBounds() { 
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

void speedMove(float loc[3],float tarSpeed, ZRState myZRState) {
		float velTar[3];
		mathVecSubtract(velTar, loc, myZRState, 3);
		float ratio = tarSpeed / mathVecMagnitude(velTar, 3);
		
		velTar[0] *= ratio;
		velTar[1] *= ratio;
		velTar[2] *= ratio;
		api.setVelocityTarget(velTar);
	}
	


void arcMove(float posTarget2[3])
{
    float midpoint[3] = {(myPos[0]+posTarget2[0])/2, (myPos[1]+posTarget2[1])/2, (myPos[2]+posTarget2[2])/2};
    if (mathVecMagnitude(midpoint,3) < 0.29) {
        mathVecNormalize(midpoint,3);
     	for (int i = 0; i<3; i++) {
    	 	midpoint[i] *= 0.48;
     	}
     	api.setPositionTarget(midpoint);
     	DEBUG((" | Heading to waypoint | "));
    }
    else {
        api.setPositionTarget(posTarget2);
    }
}
