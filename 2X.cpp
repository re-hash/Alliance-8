//Globals
float zero[3];
float earth[3];
float darkzone[3];
int picturesTaken;
int color;
int elapsed;
int targetPOI;
float locPOI[3];
float shootPos[3];

bool zero_taken;
bool one_taken;
bool two_taken;


ZRState myState;
float myPos[3];
ZRState otherState;
float otherPos[3];

void init(){
    zero_taken=false;
    one_taken=false;
    two_taken = false;
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
	shootPos[0]=0.0;
	shootPos[1]=0.0;
	picturesTaken = 0;
	updateState();
    if (myPos[1] > 0)
    {
        color = 1;
        targetPOI = 1;
        DEBUG(("We are blue!"));
    }
    else
    {
        color = -1;
        targetPOI = 1;
        DEBUG(("We are red!"));
    }
    //shootPos[2]=color*0.44;
}

void loop(){
	updateState();
    if (1==2)
    {
        
    }
	else
	{
	    targetPOI = 1;//findClosestPOI(myPos);
	    game.getPOILoc(locPOI, targetPOI);
    	mathVecNormalize(locPOI, 3);
    	shootPos[1] = fabsf(locPOI[1])*0.44;
        shootPos[2] = color*sqrtf(1-powf(locPOI[1], 2))*0.44;
    	api.setPositionTarget(shootPos);
    	facePos(zero, targetPOI);
    }
	if (elapsed % 60 == 0)
	{
	    zero_taken=false;
        one_taken=false;
        two_taken = false;
	}
    elapsed++;
}

void updateState(){
    api.getMyZRState(myState);
    api.getOtherZRState(otherState);
	for (int i = 0; i < 3; i++)
	{
	    myPos[i] = myState[i];
	    otherPos[i] = otherState[i];
	}
}

int incrementPOI(int poi_number){
    //if (poi_number < 2) return poi_number+1;
    //else if (poi_number==2) return 0;
    return 1;
}
void facePos(float target[3], int targetPOI){
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
                DEBUG(("ALIGNED"));
                    //DEBUG(("\n  Pictures: %d", picturesTaken+1)); //
                    float oldScore = game.getScore();
                    game.takePic(targetPOI);
                    float newScore = game.getScore();
                    if (oldScore+0.06 < newScore)
                    {
                        //targetPOI = incrementPOI(targetPOI);
                        hit(targetPOI);
                        DEBUG(("Got a picture!"));
                    }
                    picturesTaken++;
            }
        }
	}
void hit(int poi)
{
    if (poi == 0)
    {
        zero_taken = true;
        DEBUG(("ZERO TAKEN!!!"));
    }
    else if (poi == 1)
    {
        one_taken = true;
        DEBUG(("ONE TAKEN!!!"));
    }
    else if (poi == 2)
    {
        two_taken = true;
        DEBUG(("TWO TAKEN!!!"));
    }
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
	if (zero_taken) aDistance = 99;
	if (one_taken) bDistance = 99;
	if (two_taken) cDistance = 99;
	
	
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
