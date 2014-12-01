//NOTE TO SELF:
//  Maybe add something that resets picture counter when hit by flare.



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
int countPOI;

//Counters
int time;
int picturesTaken;
int timeSinceArrival;
float targetScore;
int strategyType;

void init()
{
    //----Coordinates & Positioning----//
    zero[0]=0.0;
	zero[1]=0.0;
	zero[2]=0.0;
	
	safetyZone[0]=0.6;
	safetyZone[1]=0.0;
	safetyZone[2]=0.0;
	
	api.getMyZRState(myState);
	if (myState[1] > 0)
	{
	    teamColor = 1;
	    strategyType = 0;
	    DEBUG(("We are Blue"));
	}
	else
	{
	    teamColor = 0;
	    strategyType = 1;
	    DEBUG(("We are Red"));
	}
	
	for (int i = 0; i<3; i++) {
        myPos[i] = myState[i];
    }
	
	//----Targeting----//
	pastPOIs[0] = -1;
    pastPOIs[1] = -1;
    pastPOIs[2] = -1;
    countPOI = 0;
    
	targetPOI = findClosestPOI(myPos, pastPOIs);
    DEBUG(("\n  targetPOI: %d", targetPOI));
    game.getPOILoc(poiPos, targetPOI);
    pastPOIs[countPOI] = targetPOI;
    countPOI++;
	//cornerTurned = false;
	
	//----Counters----//
	picturesTaken = 0;
	timeSinceArrival = 0;
	time = 0;
	targetScore = 30;
}

void loop()
{
    DEBUG(("\n# of Captured POIs: %d", countPOI));
    //DEBUG(("\nPOIs Visited 1: %d", pastPOIs[0]));
    //DEBUG(("\nPOIs Visited 2: %d", pastPOIs[1]));
    //DEBUG(("\nPOIs Visited 3: %d", pastPOIs[2]));
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
        //DEBUG(("\nGame over man, game over!"));
    }
    /*else if (game.getNextFlare() != -1 && game.getNextFlare() < 5)
    {
        game.turnOff();
        DEBUG(("Flare incoming"));
    }
    else
    {
        game.turnOn();
    }*/
    
    /*while (game.getScore() > 14)
    {
        api.setPositionTarget(safetyZone);
    }*/
    
    
    if (strategyType == 0) {
        //----TargetPOI----//
        if (time == 60 || time % 60 == 0)
        {
            DEBUG(("\n  POIs Updated..."));
            pastPOIs[0] = -1;
            pastPOIs[1] = -1;
            pastPOIs[2] = -1;
            countPOI = 0;
            
            targetPOI = findClosestPOI(myPos, pastPOIs);
            DEBUG(("\ntargetPOI: %d", targetPOI));
            pastPOIs[countPOI] = targetPOI;
            countPOI++;
            
            picturesTaken = 0;
            
        }
        /*if (targetPOI > 2 || countPOI >= 3)
        {
            pastPOIs[0] = -1;
            pastPOIs[1] = -1;
            pastPOIs[2] = -1;
            countPOI = 0;
            
            targetPOI = findClosestPOI(myPos, pastPOIs);
            pastPOIs[countPOI] = targetPOI;
            countPOI++;
        }*/
        
        game.getPOILoc(poiPos, targetPOI);
        
        for (int i = 0; i<3; i++) 
        { //Reset POI vector to half its original magnitude
            poiPos[i] *= 0.5;
        }
        
        float posTarget[3] = {poiPos[0]*3.9, poiPos[1]*3.9, poiPos[2]*3.9}; 
        //Find coordinates for SPHERE to be at while taking pictures
        float uploadTarget[3] = {poiPos[0]*7.25, poiPos[1]*7.25, poiPos[2]*7.25};
        //Find coordinates for SPHERE to be at while uploading
        
        if (picturesTaken > 3)// && time > 60) 
        { 
        //If SPHERE has attempted more than 3 pictures (valid or not), move to next POI
            if (countPOI == 3)
            {
                DEBUG(("\n  All POIs captured"));
                pastPOIs[0] = -1;
                pastPOIs[1] = -1;
                pastPOIs[2] = -1;
                countPOI = 0;
            
                targetPOI = findClosestPOI(myPos, pastPOIs);
                pastPOIs[countPOI] = targetPOI;
                countPOI++;
                picturesTaken = 0;
                DEBUG(("\n  Moving on~~"));
                DEBUG(("\n  New targetPOI: %d", targetPOI));
            }
            else
            {
                targetPOI = findClosestPOI(myPos, pastPOIs);
                pastPOIs[countPOI] = targetPOI;
                countPOI++;
            
                picturesTaken = 0;
                DEBUG(("\n Moving on~~"));
                DEBUG(("\n targetPOI: %d", targetPOI));
            }
        }
        else { 
        //Otherwise, continue taking pictures
            facePos(poiPos, myPos);
        }
        
        //----Collision Avoidance & Positioning----//
        //float midpoint[3] = {(myPos[0]+posTarget[0])/2, (myPos[1]+posTarget[1])/2, (myPos[2]+posTarget[2])/2};
        //float qtl1[3] = {(myPos[0]+midpoint[0])/2, (myPos[1]+midpoint[1])/2, (myPos[2]+midpoint[2])/2};
        //float qtl3[3] = {(posTarget[0]+midpoint[0])/2, (posTarget[1]+midpoint[1])/2, (posTarget[2]+midpoint[2])/2};
        
        arcMove(posTarget);
        /* if (mathVecMagnitude(midpoint,3) < 0.35) { 
        // this part is really rough - with some tweaking of magnitudes and such, it might work better.
            arcMove(midpoint, posTarget);
        }
        else { //Proceed to target coordinates as usual
            /*if(mathVecMagnitude(qtl3, 3)>0.35)
            {
        		 if (mathVecMagnitude(qtl1,3)>0.35)
        		 {
        		 	api.setPositionTarget(posTarget);
        		 }
        		 else
        		 {
        		     arcMove(midpoint, posTarget);
        		 }	
            }
            else
            {
            	arcMove(midpoint, posTarget);
            }
            */
//        }
        
        if (game.getMemoryFilled()>0 && game.getScore() < targetScore) { //If SPHERE has a valid picture
            DEBUG(("\n  Moving to upload"));
            
            float attTarget[3];
            attTarget[0] = 0.64 -myPos[0]; // [0.64,0,0] is the position of Earth
            attTarget[1] = 0 -myPos[1];
            attTarget[2] = 0 -myPos[2];
            mathVecNormalize(attTarget,3);
            api.setAttitudeTarget(attTarget);
            
            api.setPositionTarget(uploadTarget); //Move to upload position
            if (distanceVec(myPos, zero)> 0.53 && distanceVec(myPos,attTarget)> 0.05) //When SPHERE is out of both orbits, upload
            {
                float oldScore = game.getScore();
                game.uploadPic();
                if (oldScore < game.getScore())
                {
                    //DEBUG(("Upload was successful!"));
                }
                if (oldScore >= game.getScore())
                {
                    //DEBUG(("Upload failed!"));
                }
                
            }
        }
    }
    else if (strategyType == 1) {
        api.getOtherZRState(otherState);
        
        for (int i = 0; i<3; i++) {
            otherPos[i] = 0.5*otherState[i];
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

//--------------------------------------HELPER METHODS-------------------------------------------//
void facePos(float target[3], float myPos[3]){
		//Rotate to target
		float attTarget[3];
		
		mathVecSubtract(attTarget,target,myPos,3);
		mathVecNormalize(attTarget,3);
		api.setAttitudeTarget(attTarget);
		
        if (distanceVec(myPos, target)< 0.50)
        {
            //DEBUG(("The SPHERE is close enough to take a picture. ")); //    
            if (game.alignLine(targetPOI)==true)
            {
                if (timeSinceArrival > 4) //5 seconds between pictures, 5 seconds after arriving at POI
                {
                    DEBUG(("\n  Pictures: %d", picturesTaken+1)); //      
                    game.takePic(targetPOI);
                    picturesTaken++;
                    timeSinceArrival = 0;
                }
                timeSinceArrival++;
            }
        }
		//return distance(attTarget, myPos);
	}
	
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

int findClosestPOI(float myPos[3],int pastPOI[3])
{
    float poi0[3];
    float poi1[3];
    float poi2[3];
    
	game.getPOILoc(poi0, 0);
	float aDistance = distanceVec(poi0, myPos);
	game.getPOILoc(poi1, 1);
	float bDistance = distanceVec(poi1, myPos);
	game.getPOILoc(poi2, 2);
	float cDistance = distanceVec(poi2, myPos);
	
	switch(pastPOI[0])
	{
	    case 0: aDistance = 1000;
	    break;
	    case 1: bDistance = 1000;
	    break;
	    case 2: cDistance = 1000;
	    break;
	    default:
	    break;
	}
	switch(pastPOI[1])
	{
	    case 0: aDistance = 1000;
	    break;
	    case 1: bDistance = 1000;
	    break;
	    case 2: cDistance = 1000;
	    break;
	    default:
	    break;
	}
	switch(pastPOI[2])
	{
	    case 0: aDistance = 1000;
	    break;
	    case 1: bDistance = 1000;
	    break;
	    case 2: cDistance = 1000;
	    break;
	    default:
	    break;
	}
	
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

void arcMove(float posTarget2[3])
{
    float midpoint[3] = {(myPos[0]+posTarget2[0])/2, (myPos[1]+posTarget2[1])/2, (myPos[2]+posTarget2[2])/2};
    if (mathVecMagnitude(midpoint,3) < 0.35) {
        mathVecNormalize(midpoint,3);
     	for (int i = 0; i<3; i++) {
    	 	midpoint[i] *= 0.45;
     	}
     	api.setPositionTarget(midpoint);
     	DEBUG((" | Heading to waypoint | "));
    }
    else {
        api.setPositionTarget(posTarget2);
    }
}
