



if rssi is above threshold, increase capture number..... by a % of the capture time set.
if it's not, decrease capture number for that channel ... could be decrease to 0 but maintain full captures somehow

render to led strip based on what channel/team has most captures ( for each channel, check stored values of capture or special full capture number, render cap or highest cap)


int teamcap = -2
int red[chann]; //set to all zeros
int blue[chann]; //set to all zeros
int red = 0;
int blue =0;
int highred = -1;
int highblue = -1;
int threshold = 95 //or whatever value ends up calibrated. this is rssi a nearby drone must exceed to be counted
int capture_delay = 3; //may have to be mathed a bit. want to represent rssi gain that will occur over 1 second
int BluecapC = 0;
int RedcapC = 0;
int capdata[chann]; //array length chann = number of channels scanned

//loop

//rssi = newvalue for this channel
//thisChannelIndex // the channel we scanned and got the new rssi value for on this loop


// now we have how much rssi accumulated on each channel, and how many red or blue players are detected
if (rssi > threshold){
  increase capdata[thisChannelIndex]
  if (thisChannelIndex %2 == 0)
    red[thisChannelIndex] = 1;
  else
    blue[thisChannelIndex] = 1;
}  
if (rssi < threshold){
   decrease  capdata[thisChannelIndex]
   if (thisChannelIndex %2 == 0)
     red[thisChannelIndex] = 0;
   else
     blue[thisChannelIndex] = 0;
}
//add up red[] and blue[] totals.... yes some channels will be useless data lol i guess we could do this all in one array and %2 the index when we need data out of it.. oh well.
red = 0;
blue = 0;
//find highest rssi on each team
redhigh = capdata[thisChannelIndex];
bluehigh = capdata[thisChannelIndex];
for(inti =0; i<chann;++i){
  if(i%2==0){
    red = red + red[i]; //count red[] total
    if(redhigh < capdata[i])
      redhigh = capdata[i]; //find highest red
  }else{
    blue = blue + blue[i]; //count blue[] total
    if(bluehigh < capdata[i])
      bluehigh = capdata[i]; //find highest blue
  }
}

//what to do depending on how many pilots are detected on each team
if ( red > blue) { //red can cap
  //red rssi caps depending on how many blue there are
  //red capture%  = capdata[highred] / blue
  RedcapC = capdata[redhigh] - (blue * capture_delay);
  if(RedcapC == capped_value){
        teamcap = 0; //red
  }
}
if (blue > red){ //blue can cap
  //blue rssi caps depending on how many red there are
  //blue capture%  = capdata[highblue] / red
  BluecapC = capdata[bluehigh] / red;
  if(BluecapC == capped_value){
        teamcap = 1; //blue
  }
}
if (blue == red && blue != 0){ // if capped, in defence mode
  if (teamcap == 0 || teamcap == 1) //capped by red or blue
    //defence mode
    //blink whatever color the tower already is
  else {
    //whoever gets to cap rssi first caps it.
    //display highest of capdata[highblue] and cpadata[highred]
    if( RedcapC = capdata[redhigh] > BluecapC = capdata[bluehigh]){
      RedcapC = capdata[redhigh];
      if(RedcapC == capped_value){
        teamcap = 0; //red
      }
    }
    if(BluecapC = capdata[bluehigh] > RedcapC = capdata[redhigh]){
      BluecapC = capdata[bluehigh];
      if(BluecapC == capped_value){
        teamcap = 1; //blue
      }
    }
    else{
      BluecapC = equal_value;
      RedcapC = equal_value;
    } 
  }
}


if(BluecapC == equal_value && RedcapC == equal_value){
  //display equal opposing forces animation
}
if (teamcap ==0){
  //display red tower
}
else if(teamcap==1){
  //display blue tower
}
else{
  //neutral flash thing
  
}


//some other stuff then end loop


    
    
    
    case 1:
    starts neutral, red vtx seen, each consecutive read counts up until tower is capped, red vtx is no longer seen, but tower remains red.
    
    case 2:
    starts blue, red vtx seen and no blue vtx is seen, each consecutive read counts up until tower is capped, red vtx is no longer seen, tower remains red.
    
    case 3:
    starts blue, red vtx seen but also a blue vtx is seen, tower blinks blue.
    
    case 4:
    starts blue, x red vtx seen but also x-y blue vtx is seen, tower is cis capped slower by red
    
    case 5:
    idk is thera case five?