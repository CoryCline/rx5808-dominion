then a wild Gerald appeared.

  led output states: 
    % blue to display
    blue blink
    % red to display
    red blink
    neutral
    equal opposition
    
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
    
    
    
    led output states: 
    % blue to display
    blue blink
    % red to display
    red blink
    neutral
    equal opposition

constants:
    const int captureThreshold = 1000;
    const int captureRate = 10;
    const int decayRate = 5;
    
state:
    int redCaptureLevel = 0;
    int blueCaptureLevel = 0;
    int redDroneCount = 0;
    int blueDroneCount = 0;

    // start capture
    if (redCaptureLevel >= captureThreshold && blueCaptureLevel < captureThreshold) { // red controls
    	if (blueDroneCount > 0) { // blue is attacking
      	if (blueDroneCount > redDroneCount) { // more blue drones are near
          blueCaptureLevel += captureRate * (blueDroneCount - redDroneCount); // blue start capturing
          if (blueCaptureLevel >= captureThreshold) { // blue gains control
            redCaptureLevel = 0; // red loses control
          }
        } else { // more red drones are near (or an equal number of red and blue drones)
      		blueCaptureLevel -= decayRate; // blue control decays
        }
      }
    } else if (blueCaptureLevel >= captureThreshold && redCaptureLevel < captureThreshold) { // blue controls
    	if (redDroneCount > 0) { // red is attacking
      	if (redDroneCount > blueDroneCount) { // more red drones are near
          redCaptureLevel += captureRate * (redDroneCount - blueDroneCount); // red start capturing
          if (redCaptureLevel >= captureThreshold) { // red gains control
            blueCaptureLevel = 0; // blue loses control
          }
        } else { // more blue drones are near (or an equal number of red and blue drones)
      		redCaptureLevel -= decayRate; // red control decays
        }
      }
    } else { // uncontrolled
      redCaptureLevel += captureRate * redDroneCount; // start capturing
      if (redCaptureLevel >= captureThreshold) { // red gains control
        blueCaptureLevel = 0; // blue loses control
      }
      blueCaptureLevel += captureRate * blueDroneCount; // start capturing
      if (blueCaptureLevel >= captureThreshold) { // blue gains control
        redCaptureLevel = 0; // red loses control
      }
    }
    // end capture
    
    // decay team control when unmaintained
    if (redCaptureLevel > 0 && redCaptureLevel < captureThreshold && redDroneCount == 0) { // red started capturing, does not have control, and all red drones have left
      	redCaptureLevel -= decayRate; // red control decays
    }
    if (blueCaptureLevel > 0 && blueCaptureLevel < captureThreshold && blueDroneCount == 0) { // blue started capturing, does not have control, and all blue drones have left
      	blueCaptureLevel -= decayRate; // blue control decays
    }
    // end decay
    
    // start housekeeping
    if (redCaptureLevel >= captureThreshold) {
      redCaptureLevel = captureThreshold;
    }
    if (blueCaptureLevel >= captureThreshold) {
      blueCaptureLevel = captureThreshold;
    }
    if (redCaptureLevel < 0) {
    	redCaptureLevel = 0;
    }
    if (blueCaptureLevel < 0) {
    	blueCaptureLevel = 0;
    }
    // end housekeeping
    
    
    
    
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
      
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    Code:
    
    
    if (redCaptureLevel == captureThreshold && blueCaptureLevel < captureThreshold) { // red controls
  		if (blueDroneCount > 0 && blueDroneCount > redDroneCount) { // more blue drones are near
      	blueCaptureLevel += captureRate * (blueDroneCount - redDroneCount); // start capturing
      }
    } else if (blueCaptureLevel == captureThreshold && redCaptureLevel < captureThreshold) { // blue controls
  		if (redDroneCount > 0 && redDroneCount > blueDroneCount) { // more red drones are near
      	redCaptureLevel += captureRate * (redDroneCount - blueDroneCount); // start capturing
      }
    }
    
    if (redCaptureLevel >= captureThreshold) { // red captured
      redCaptureLevel = captureThreshold;
    } else if (blueCaptureLevel >= captureThreshold) { // blue captured
      blueCaptureLevel = captureThreshold;
    } else 
    
    
    if (redCaptureLevel < 0) {
    	redCaptureLevel = 0;
    }
    if (blueCaptureLevel < 0) {
    	blueCaptureLevel = 0;
    }
    
    
//    if (redCaptureLevel >= captureThreshold && blueCaptureLevel >= captureThreshold) { // too close to tell
//    	if (redCaptureLevel > blueCaptureLevel) {
//      	redCaptureLevel = captureThreshold;
//      } else if (blueCaptureLevel > redCaptureLevel) {
//      	blueCaptureLevel = captureThreshold;
//      } else {
//      	redCaptureLevel = captureThreshold - 1;
//        blueCaptureLevel = captureThreshold - 1;
//      }
//    }
    
    