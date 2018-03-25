then a wild Gerald appeared.

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
      	
      // end update LED variables
    } else {
      firstRun = false;
    }

  // end game loop
    
    