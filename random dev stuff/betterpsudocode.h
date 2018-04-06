then a wild Gerald appeared.


     case 1:
    starts neutral, red vtx seen, each consecutive read counts up until tower is capped, red vtx is no longer seen, but tower remains red.
    case 2:
    starts blue, red vtx seen and no blue vtx is seen, each consecutive read counts up until tower is capped, red vtx is no longer seen, tower remains red.
    case 3:
    starts blue, red vtx seen but also a blue vtx is seen, tower blinks blue.
    case 4:
    starts blue, x red vtx seen but also x-y blue vtx is seen, each consecutive read counts up until tower is capped by red
    
    case 1.5:
    starts neutral, blue vtx seen, each consecutive read counts up until tower is capped, blue vtx is no longer seen, tower remains blue
    case 2.5:
    starts red, blue vtx seen and no red vtx seen, each consecutive read counts up until tower is capped, blue vtx is no longer seen, tower remains blue
    case 3.5:
    starts red, blue vtx and red vtx seen, tower blinks red
    case 4.5:
    starts red, x blue vtx and x-y red vtx is seen, each consecutive read counts up until tower is capped by blue
    
    
    case 5:
    starts neutral, N red vtx seen, each consecutive read counts up until tower is capped (faster than case 1? (captureRate * N)), red vtx are no longer seen, but tower remains red.
    
    
    
    constants:
    captureThreshold
    captureRate
    decayRate
    
    state:
    redCaptureLevel
    blueCaptureLevel
    redDroneCount
    blueDroneCount
    
    
    
    
    
    
    case 1: 
    starts neutral (redCaptureLevel < captureThreshold && blueCaptureLevel < captureThreshold)
    red vtx seen (redDroneCount > 0)
    each consecutive read counts up until tower is capped (redCaptureLevel += captureRate)
    red vtx is no longer seen (redDroneCount == 0)
    but tower remains red (redCaptureLevel == captureThreshold)
    
    if (redCaptureLevel < captureThreshold && blueCaptureLevel < captureThreshold) {
      if (redDroneCount > 0) { // drone is near
        redCaptureLevel += captureRate * redDroneCount; // start capturing
      }
    }
    if (redDroneCount == 0) { // drone left
      if (redCaptureLevel < captureThreshold) { // not captured yet
        redCaptureLevel -= decayRate; // capture level decay
      } else { // captured
        redCaptureLevel = captureThreshold; // normalize capture level in case redCaptureLevel > captureThreshold
      }
    }
    
    
    
    
    
    
    case 2: starts blue (redCaptureLevel < captureThreshold && blueCaptureLevel == captureThreshold)
    red vtx seen (redDroneCount > 0)
    each consecutive read counts up until tower is capped (redCaptureLevel += captureRate)
    red vtx is no longer seen (redDroneCount == 0)
    but tower remains red (redCaptureLevel == captureThreshold)
    
    if (redCaptureLevel < captureThreshold && blueCaptureLevel == captureThreshold) { // not captured
      if (redDroneCount > 0) { // drone is near
        redCaptureLevel += captureRate; // start capturing
      }
    }
    if (redDroneCount == 0) { // drone left
      if (redCaptureLevel < captureThreshold) { // not captured yet
        redCaptureLevel -= decayRate; // capture level decay
      } else { // captured
        redCaptureLevel = captureThreshold; // normalize capture level in case redCaptureLevel > captureThreshold
        blueCaptureLevel = 0; // reset blue capture level
      }
    }
    
    
    
    
    
    case 3: starts blue (redCaptureLevel < captureThreshold && blueCaptureLevel == captureThreshold)
    red vtx seen (redDroneCount > 0)
    blue vtx seen (blueDroneCount > 0)
    equal vtx seen from each team (redDroneCount == blueDroneCount)
    
    //TEAM INDEPENDENT
    if (redCaptureLevel == captureThreshold || blueCaptureLevel == captureThreshold) { // captured by a team
      if (redDroneCount > 0 && blueDroneCount > 0 && redDroneCount == blueDroneCount) { // there are an equal, nonzero, number of drones nearby from each team
        if (redCaptureLevel == captureThreshold) {
          // blink red
        } else {
          // blink blue
        }
      }
    }
    
    
    
    
    
    case 4.5: (redCaptureLevel == captureThreshold && blueCaptureLevel < captureThreshold)
    X red vtx seen (redDroneCount > 0)
    Y blue vtx seen (blueDroneCount > 0)
    more blue than red (blueDroneCount > redDroneCount)
    
    if (redCaptureLevel == captureThreshold && blueCaptureLevel < captureThreshold) { // red controls
      if (blueDroneCount > 0 && blueDroneCount > redDroneCount) { // more blue drones are near
        blueCaptureLevel += captureRate * (blueDroneCount - redDroneCount); // start capturing
      }
    }
    if (blueCaptureLevel > captureThreshold) { // blue captured
      blueCaptureLevel = captureThreshold; // normalize capture level if there is excess
    }
    
    
    
    
    
Case 99?:
    neutral tower
    red enters
    red captures 50%
    blue enters
    
    ... red & blue raise capture levels until one captures? (red will win unless derp)
    
    
    red captures to 75%, blue captures to 25%
    red leaves
    
    ... then: red capture level decays, blue continues gaining cap
    
    red decays to 70%, blue captures to 30%
    red enters
    red captures to 100%, blue captures to 60%
    red controls
    
    
    case A: red leaves, blue stays
      blue captures from 60% to 100%
      blue controls
      
    case B: red stays, blue stays
      red controls, blueCaptureLevel = 60%, tower in contention(case 3) and flashes red
      option A - blue decays from 60%, even though in contention and blue nearby (BENEFIT FOR OWNING, other than winning)
      option B - blueCaptureLevel = 0
      
    case C: red leaves, blue leaves
      red controls, blue decays from 60%
      
    case D: red stays, blue leaves
      red controls, blue decays from 60% at decate rate * N
      
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
led output states: 
    % blue to display = blueCaptureLevel*100/CAPTURE_THRESHOLD
    blue blink = blueCaptureLevel >= CAPTURE_THRESHOLD && defended
    % red to display = redCaptureLevel*100/CAPTURE_THRESHOLD
    red blink = redCaptureLevel >= CAPTURE_THRESHOLD && defended
    neutral = blueCaptureLevel < CAPTURE_THRESHOLD && redCaptureLevel < CAPTURE_THRESHOLD
    // for overlap between % blue to display and % red to display, show purple
    -- equal opposition - (neutral code only: blue && red at X%, only shows one color ... PURPLE!)
    

    const int CAPTURE_THRESHOLD = 1000;
    const int CAPTURE_RATE = 10;
    const int DECAY_RATE = 5;
    
    int redCaptureLevel = 0;
    int blueCaptureLevel = 0;
    int redDroneCount = 0;
    int blueDroneCount = 0;
    bool defended = false;
    bool firstRun = true;


  // game loop starts here

    // populate redDroneCount and blueDroneCount variables based on rssi values for each channel
    //redDroneCount = 0;
    //blueDroneCount = 0;
    //for each channel {
      //if (channelRSSI > SIGNAL_STRENGTH_THRESHOLD) {
        //if (channel % 2 == 0) {
          //redDroneCount++;
        //} else {
          //blueDroneCount++;
        //}
      //}
    //}

    // start capture
    if (!firstRun) {
      defended = false; // reset defended flag each loop
      if (redCaptureLevel >= CAPTURE_THRESHOLD && blueCaptureLevel < CAPTURE_THRESHOLD) { // red controls
        if (blueDroneCount > 0) { // blue is attacking
          if (blueDroneCount > redDroneCount) { // more blue drones are near
            blueCaptureLevel += CAPTURE_RATE * (blueDroneCount - redDroneCount); // blue start capturing
            if (blueCaptureLevel >= CAPTURE_THRESHOLD) { // blue gains control
              redCaptureLevel = 0; // red loses control
            }
          } else { // more red drones are near (or an equal number of red and blue drones)
            blueCaptureLevel -= DECAY_RATE; // blue control decays
            defended = true; // set defended flag to start flashing
          }
        }
      } else if (blueCaptureLevel >= CAPTURE_THRESHOLD && redCaptureLevel < CAPTURE_THRESHOLD) { // blue controls
        if (redDroneCount > 0) { // red is attacking
          if (redDroneCount > blueDroneCount) { // more red drones are near
            redCaptureLevel += CAPTURE_RATE * (redDroneCount - blueDroneCount); // red start capturing
            if (redCaptureLevel >= CAPTURE_THRESHOLD) { // red gains control
              blueCaptureLevel = 0; // blue loses control
            }
          } else { // more blue drones are near (or an equal number of red and blue drones)
            redCaptureLevel -= DECAY_RATE; // red control decays
          }
        }
      } else { // tower is uncontrolled
        redCaptureLevel += CAPTURE_RATE * redDroneCount; // start capturing
        if (redCaptureLevel >= CAPTURE_THRESHOLD) { // red gains control
          blueCaptureLevel = 0; // blue loses control
        }
        blueCaptureLevel += CAPTURE_RATE * blueDroneCount; // start capturing
        if (blueCaptureLevel >= CAPTURE_THRESHOLD) { // blue gains control
          redCaptureLevel = 0; // red loses control
        }
      }
      // end capture

      // decay team control when unmaintained
      if (redCaptureLevel > 0 && redCaptureLevel < CAPTURE_THRESHOLD && redDroneCount == 0) { // red started capturing, does not have control, and all red drones have left
          redCaptureLevel -= DECAY_RATE; // red control decays
      }
      if (blueCaptureLevel > 0 && blueCaptureLevel < CAPTURE_THRESHOLD && blueDroneCount == 0) { // blue started capturing, does not have control, and all blue drones have left
          blueCaptureLevel -= DECAY_RATE; // blue control decays
      }
      // end decay

      // start housekeeping
      if (redCaptureLevel >= CAPTURE_THRESHOLD) {
        redCaptureLevel = CAPTURE_THRESHOLD;
      }
      if (blueCaptureLevel >= CAPTURE_THRESHOLD) {
        blueCaptureLevel = CAPTURE_THRESHOLD;
      }
      if (redCaptureLevel < 0) {
        redCaptureLevel = 0;
      }
      if (blueCaptureLevel < 0) {
        blueCaptureLevel = 0;
      }
      // end housekeeping

      // update LED variables
        //TODO
      // end update LED variables
    } else {
      firstRun = false;
    }

  // end game loop


 //led output states: 
    //% blue to display = blueCaptureLevel*100/CAPTURE_THRESHOLD
    //blue blink = blueCaptureLevel >= CAPTURE_THRESHOLD && defended
    //% red to display = redCaptureLevel*100/CAPTURE_THRESHOLD
    //red blink = redCaptureLevel >= CAPTURE_THRESHOLD && defended
    //neutral = blueCaptureLevel < CAPTURE_THRESHOLD && redCaptureLevel < CAPTURE_THRESHOLD
    // for overlap between % blue to display and % red to display, show purple
    //-- equal opposition - (neutral code only: blue && red at X%, only shows one color ... PURPLE!)
    int redCapturePercent = redCaptureLevel*100/CAPTURE_THRESHOLD; //then mapped and constrained
    int blueCapturePercent = blueCaptureLevel*100/CAPTURE_THRESHOLD; //then mapped and constrained
    if (redCaptureLevel < CAPTURE_THRESHOLD && blueCaptureLevel < CAPTURE_THRESHOLD && blueCapturePercent == redCapturePercent) {
      //display purple up to %
    } else if (redCapturePercent > blueCapturePercent) {
      if (redCaptureLevel >= CAPTURE_THRESHOLD) {
        //display blue up to blueCapturePercent
      } else {
        //display purple up to blueCapturePercent
      }
      //display red up to redCapturePercent
    } else {
      if (blueCaptureLevel >= CAPTURE_THRESHOLD) {
        //display red up to redCapturePercent
      } else {
        //display purple up to redCapturePercent
      }
      //display blue up to blueCapturePercent
    }
    if (blueCaptureLevel >= CAPTURE_THRESHOLD && defended) {
      //blink blue
    }
    if (redCaptureLevel >= CAPTURE_THRESHOLD && defended) {
      //blink red
    }
    if (blueCaptureLevel < CAPTURE_THRESHOLD && redCaptureLevel < CAPTURE_THRESHOLD) {
      //display neutral state --- but it could be being captured still lol
    }


chan = constrain(
            map(
                blueCapLvl,
                0,
                100,
                0,
                NUM_LEDS-1
            ),
            0,
            NUM_LEDS-1
        );