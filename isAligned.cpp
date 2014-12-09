bool isAligned(float myPos[3], float myAtt[3], float poiLoc[3])
{
    if (mathVecInner(poiLoc, myAtt) < -0.99) //dot product should be -1 if the two vectors are indeed pointing in opposite directions
    {
        return true; //untested, can someone see if this works?
    }
    else
    {
        return false;
    }
    
}
