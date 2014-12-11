//Declare any variables shared between functions here
int targetPOI;
float zero[3];
float earth[3];
float darkzone[3];
int picturesTaken;
int color;
int elapsed;
int strategy;
int flareCounter;

void init(){
	//This function is called once when your code is first loaded.
	flareCounter = 0;
	elapsed = 0;
    zero[0]=0.0;
	zero[1]=0.0;
	zero[2]=0.0;
	earth[0]=0.64;
	earth[1]=0.0;
	earth[2]=0.0;
	darkzone[0]=0.55;
	darkzone[1]=0.0;
	darkzone[2]=0.0;
	picturesTaken = 0;
	strategy =0;
	ZRState myState;
	api.getMyZRState(myState);
	float myPos[3] = {myState[0], myState[1], myState[2]};
    if (myPos[1] > 0)
    {
        color = -1;
        DEBUG(("We are blue!")); //Determine if SPHERE is red or blue
    }
    else
    {
        color = -1;
        DEBUG(("We are red!"));
    }
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
	
}

void loop(){
if (strategy==0)
{    
	ZRState myState;
    api.getMyZRState(myState);
    float myPos[3] = {myState[0], myState[1], myState[2]};
    float poiSpyLoc[3];
    targetPOI = findClosestPOI(myPos);
    	float poiLoc[3];
    	game.getPOILoc(poiLoc, targetPOI);
	//This function is called once per second.  Use it to control the satellite.
	if (game.getNextFlare() < 25 && game.getNextFlare() !=-1)
	{
	    api.setPositionTarget(darkzone);
        float attTarget[3];
    		mathVecSubtract(attTarget,earth,myPos,3);
    		mathVecNormalize(attTarget,3);
    		api.setAttitudeTarget(attTarget);
                        float oldScore = game.getScore();
                        
                        game.uploadPic();
                        if (oldScore < game.getScore())
                        {
                            //DEBUG(("Upload was successful!"));
                            picturesTaken = 0;
                        }
                        if (oldScore >= game.getScore())
                        {
                            //DEBUG(("Upload failed!"));
                        }
                    

        game.getPOILoc(poiSpyLoc, targetPOI);
        if (poiSpyLoc[2]>0.15)
        {
            color = -1;
        }
        else
        {
            color = 1;
        }
	}
	else if (elapsed>180 && game.getMemoryFilled()>0)
        {
            
    	//float uploadTarget[3] = {0.1, 0.0, color*0.6};
            //api.setPositionTarget(uploadTarget);
            float attTarget[3];
    		mathVecSubtract(attTarget,earth,myPos,3);
    		mathVecNormalize(attTarget,3);
    		api.setAttitudeTarget(attTarget);
            if (distanceVec(myPos, zero)> 0.0) //When SPHERE is out of both orbits, upload
            {
                float oldScore = game.getScore();
                
                game.uploadPic();
                if (oldScore < game.getScore())
                {
                    DEBUG(("Upload was successful!"));
                    picturesTaken = 0;
                }
                if (oldScore >= game.getScore())
                {
                    DEBUG(("Upload failed!"));
                }
            
            }
        }
	else
	{
    	float target[3] = {0.0, 0.0, color*0.44};

    	api.setPositionTarget(target);
    	
    	facePos(zero, myPos);
        if (game.getMemoryFilled()==game.getMemorySize()) 
        { //If SPHERE has a valid picture
            //DEBUG(("\n  Moving to upload"));
            //api.setPositionTarget(uploadTarget);
            float attTarget[3];
    		mathVecSubtract(attTarget,earth,myPos,3);
    		mathVecNormalize(attTarget,3);
    		api.setAttitudeTarget(attTarget);//Move to upload position
            if (distanceVec(myPos, zero)> 0/*.53*/) //When SPHERE is out of both orbits, upload
            {
                float oldScore = game.getScore();
                
                game.uploadPic();
                if (oldScore < game.getScore())
                {
                    //DEBUG(("Upload was successful!"));
                    picturesTaken = 0;
                }
                if (oldScore >= game.getScore())
                {
                    //DEBUG(("Upload failed!"));
                }
            
            }
       
        }

	}
	if (game.getFuelRemaining() <= 0)
    {
    game.turnOff();
    //DEBUG(("\nGame over man, game over!"));
    }
    if (game.getNextFlare() == 29)
    {
        flareCounter++;
    }
    if (game.getScore() < game.getOtherScore() && (flareCounter/3)==2)
    {
        strategy = 1; 
    }
}
else if (strategy==1)
{
    stayInBounds();
    ZRState myState;
    ZRState otherState;
    float myPos[3];
    float otherPos[3];
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

        api.getOtherZRState(otherState);
        
        for (int i = 0; i<3; i++) {
            otherPos[i] = 1.2*otherState[i];
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
elapsed++;	
}

void facePos(float target[3], float myPos[3]){
		//Rotate to target
		float attTarget[3];
		
		mathVecSubtract(attTarget,target,myPos,3);
		mathVecNormalize(attTarget,3);
		api.setAttitudeTarget(attTarget);
		
        if (distanceVec(myPos, target)< 1.50)
        {
            //DEBUG(("The SPHERE is close enough to take a picture. ")); //    
            if (game.alignLine(targetPOI)==true)
            {

                    //DEBUG(("\n  Pictures: %d", picturesTaken+1)); //      
                    game.takePic(targetPOI);
                    picturesTaken++;
            }
        }
		//return distance(attTarget, myPos);
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
    
	game.getPOILoc(poi0, 0);
	float aDistance = distanceVec(poi0, myPos);
	game.getPOILoc(poi1, 1);
	float bDistance = distanceVec(poi1, myPos);
	game.getPOILoc(poi2, 2);
	float cDistance = distanceVec(poi2, myPos);
	
	
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
	    //DEBUG(("\n   Targeting error has occurred :("));
	}
}
float distanceVec(float a[3], float b[3]) {  //finds distance between two objects
	float diff[3];
	mathVecSubtract(diff,a,b,3);
	return mathVecMagnitude(diff,3);
}

void stayInBounds() { 
	float force[3] = {0.00001,0.00001,0.00001};
	ZRState myState;
	api.getMyZRState(myState);
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
    ZRState myState;
    api.getMyZRState(myState);
    float myPos[3] = {myState[0], myState[1], myState[2]};
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
