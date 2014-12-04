//Declare any variables shared between functions here
int targetPOI;
float zero[3];
float earth[3];
float darkzone[3];
int picturesTaken;
int color;
int elapsed;

void init(){
	//This function is called once when your code is first loaded.
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
	ZRState myState;
	api.getMyZRState(myState);
	float myPos[3] = {myState[0], myState[1], myState[2]};
    if (myPos[1] > 0)
    {
        color = 1;
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
	ZRState myState;
    api.getMyZRState(myState);
    float myPos[3] = {myState[0], myState[1], myState[2]};
    float poiSpyLoc[3];
    targetPOI = findClosestPOI(myPos);
    	float poiLoc[3];
    	game.getPOILoc(poiLoc, targetPOI);
	//This function is called once per second.  Use it to control the satellite.
	if (game.getNextFlare() <20 && game.getNextFlare() !=-1)
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
        if (poiSpyLoc[2]>0)
        {
            color = 1;
        }
        else
        {
            color = -1;
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
    	float target[3] = {0.0, 0.0, color*0.4};
    	//float uploadTarget[3] = {0.1, 0.0, color*0.6};

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
            if (distanceVec(myPos, zero)> 0.53) //When SPHERE is out of both orbits, upload
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
elapsed++;	
}

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
