


#include <avr/pgmspace.h>

#include "state_bandscan.h"

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "channels.h"
#include "buttons.h"

//for led peak timing
#include "timer.h"


#include "ui.h"
#include "ui_menu.h"

//LED stuff
#include "FastLED.h"
#define LED_PIN     5
#define NUM_LEDS    10
#define BRIGHTNESS  100 //seems to work and doesn't blind you
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

void StateMachine::BandScanStateHandler::onEnter() {
    orderedChanelIndex = 0;
    lastChannelIndex = Receiver::activeChannel;
    
    //LED stuff
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
}

void StateMachine::BandScanStateHandler::onExit() {
    Receiver::setChannel(lastChannelIndex);
}

//decalre variables for rssi LED output
    const int CAPTURE_THRESHOLD = 1000;
    const int RSSI_THRESHOLD = 95;
    const int CAPTURE_RATE = 10;
    const int DECAY_RATE = 5;
    
    int redCaptureLevel = 0;
    int blueCaptureLevel = 0;
    int redDroneCount = 0;
    int blueDroneCount = 0;
    bool defended = false;
    bool firstRun = true;


void StateMachine::BandScanStateHandler::onUpdate() {
    if (!Receiver::isRssiStable())
        return;

    #ifdef USE_DIVERSITY
        rssiData[orderedChanelIndex] = (Receiver::rssiA + Receiver::rssiB) / 2;
    #else
        rssiData[orderedChanelIndex] = Receiver::rssiA;
    #endif
    
    // populate redDroneCount and blueDroneCount variables based on rssi threshold values for each channel
    redDroneCount = 0;
    blueDroneCount = 0;
    for (int i = 0; i < CHANNELS_SIZE; ++i){
      if (rssiData[orderedChanelIndex]> RSSI_THRESHOLD) {
        if (orderedChanelIndex % 2 == 0) {
          redDroneCount++;
        } else {
          blueDroneCount++;
        }
      }
    }
    
    
     // start capture game logic and led output
    if (!firstRun && orderedChanelIndex == 0) {//if it's not the first run, and we are on the beginning of a band scan
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
      
      //return the LED's all to black so we can display a change! ... should work here
      for ( int i=0;i<NUM_LEDS;++i){
        leds[i] = CRGB::Black;
      }
      
      int redCapturePercent = redCaptureLevel*100/CAPTURE_THRESHOLD; //then mapped and constrained
      int blueCapturePercent = blueCaptureLevel*100/CAPTURE_THRESHOLD; //then mapped and constrained
      if (redCaptureLevel < CAPTURE_THRESHOLD && blueCaptureLevel < CAPTURE_THRESHOLD && blueCapturePercent == redCapturePercent) {
        //display purple up to %
        for(int i=0;i<((NUM_LEDS-1)*redCapturePercent);++i){
          leds[i] = CHSV(192, 255, BRIGHTNESS);//purple
        }
      } else if (redCapturePercent > blueCapturePercent) {
        if (redCaptureLevel >= CAPTURE_THRESHOLD) {
          //display blue up to blueCapturePercent
          for(int i=0;i<((NUM_LEDS-1)*blueCapturePercent);++i){
            leds[i] = CHSV(160,255, BRIGHTNESS);//blue
          }
        } else {
          //display purple up to blueCapturePercent (for when cap from neutral)
          for(int i=0;i<((NUM_LEDS-1)*blueCapturePercent);++i){
            leds[i] = CHSV(192,255, BRIGHTNESS);//purple
          }
        }
        //display red up to redCapturePercent
        for(int i=0;i<((NUM_LEDS-1)*redCapturePercent);++i){
          leds[i] = CHSV(0, 255, BRIGHTNESS);//red
        }
      } else {
        if (blueCaptureLevel >= CAPTURE_THRESHOLD) {
          //display red up to redCapturePercent
          for(int i=0;i<((NUM_LEDS-1)*redCapturePercent);++i){
            leds[i] = CHSV(0, 255, BRIGHTNESS);//red
          }
        } else {
          //display purple up to redCapturePercent (for when cap from neutral)
          for(int i=0;i<((NUM_LEDS-1)*redCapturePercent);++i){
            leds[i] = CHSV(192, 255, BRIGHTNESS);//purple
          }
        }
        //display blue up to blueCapturePercent
        for(int i=0;i<((NUM_LEDS-1)*blueCapturePercent);++i){
          leds[i] = CHSV(160,255, BRIGHTNESS);//blue
        {
      }
      if (blueCaptureLevel >= CAPTURE_THRESHOLD && defended) {
        //blink blue
        leds[(NUM_LEDS-1)] = CHSV(160,255, BRIGHTNESS);//blue
      }
      if (redCaptureLevel >= CAPTURE_THRESHOLD && defended) {
        //blink red
        leds[(NUM_LEDS-1)] = CHSV(0,255, BRIGHTNESS);//red
      }
      if (blueCaptureLevel < CAPTURE_THRESHOLD && redCaptureLevel < CAPTURE_THRESHOLD) {
        //display neutral state --- 
        for(int i=0;i<NUM_LEDS;++i){
          leds[i] = CRGB::Gray;
        }
      }
      // Show the leds
      FastLED.show();  
      
      } else {
        firstRun = false;
      }

    
    //stuff that was already here
    //orderedChanelIndex = (orderedChanelIndex + 1) % (CHANNELS_SIZE);
    //Receiver::setChannel(Channels::getOrderedIndex(orderedChanelIndex));

    //only go thru ETBest6 channels
    orderedChanelIndex = (orderedChanelIndex + 1) % (CHANNELS_SIZE);
    Receiver::setChannel(Channels::getETBest6Index(orderedChanelIndex));

    Ui::needUpdate();

    if (orderedChanelIndex == 0) {

    }
    
    
    
}
#define BORDER_LEFT_X 0
#define BORDER_LEFT_Y 0
#define BORDER_LEFT_H (SCREEN_HEIGHT - CHAR_HEIGHT - 1)

#define BORDER_RIGHT_X (SCREEN_WIDTH - 1)
#define BORDER_RIGHT_Y 0
#define BORDER_RIGHT_H BORDER_LEFT_H

#define BORDER_BOTTOM_X 0
#define BORDER_BOTTOM_Y (SCREEN_HEIGHT - CHAR_HEIGHT - 2)
#define BORDER_BOTTOM_W SCREEN_WIDTH

#define CHANNEL_TEXT_LOW_X 0
#define CHANNEL_TEXT_LOW_Y (SCREEN_HEIGHT - CHAR_HEIGHT)
#define CHANNEL_TEXT_W ((CHAR_WIDTH + 1) * 4)

#define CHANNEL_TEXT_HIGH_X (SCREEN_WIDTH - CHANNEL_TEXT_W + 1)
#define CHANNEL_TEXT_HIGH_Y CHANNEL_TEXT_LOW_Y

#define BORDER_PROGRESS_LEFT_X (CHANNEL_TEXT_LOW_X + CHANNEL_TEXT_W)
#define BORDER_PROGRESS_RIGHT_X (CHANNEL_TEXT_HIGH_X - 2)
#define BORDER_PROGRESS_Y BORDER_BOTTOM_Y
#define BORDER_PROGRESS_H SCREEN_HEIGHT - BORDER_BOTTOM_Y

#define PROGRESS_X (BORDER_PROGRESS_LEFT_X + 2)
#define PROGRESS_Y (BORDER_PROGRESS_Y + 2)
#define PROGRESS_W (BORDER_PROGRESS_RIGHT_X - PROGRESS_X - 1)
#define PROGRESS_H (SCREEN_HEIGHT - PROGRESS_Y) - 2

#define GRAPH_X (BORDER_LEFT_X + 1)
#define GRAPH_Y 0
#define GRAPH_W (BORDER_RIGHT_X - GRAPH_X)
#define GRAPH_H BORDER_BOTTOM_Y


void StateMachine::BandScanStateHandler::onInitialDraw() {
    Ui::clear();

    Ui::display.drawFastVLine(
        BORDER_LEFT_X,
        BORDER_LEFT_Y,
        BORDER_LEFT_H,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_RIGHT_X,
        BORDER_RIGHT_Y,
        BORDER_RIGHT_H,
        WHITE
    );

    Ui::display.drawFastHLine(
        BORDER_BOTTOM_X,
        BORDER_BOTTOM_Y,
        BORDER_BOTTOM_W,
        WHITE
    );

    Ui::display.drawFastHLine(
        BORDER_PROGRESS_LEFT_X,
        SCREEN_HEIGHT - 1,
        BORDER_PROGRESS_RIGHT_X - BORDER_PROGRESS_LEFT_X,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_PROGRESS_LEFT_X,
        BORDER_PROGRESS_Y,
        BORDER_PROGRESS_H,
        WHITE
    );

    Ui::display.drawFastVLine(
        BORDER_PROGRESS_RIGHT_X,
        BORDER_PROGRESS_Y,
        BORDER_PROGRESS_H,
        WHITE
    );

    Ui::display.setTextSize(1);
    Ui::display.setTextColor(WHITE);
    Ui::display.setCursor(CHANNEL_TEXT_LOW_X, CHANNEL_TEXT_LOW_Y);
    Ui::display.print(Channels::getFrequency(Channels::getOrderedIndex(0)));

    Ui::display.setCursor(CHANNEL_TEXT_HIGH_X, CHANNEL_TEXT_HIGH_Y);
    Ui::display.print(
        Channels::getFrequency(Channels::getOrderedIndex(CHANNELS_SIZE - 1)));

    Ui::needDisplay();
}

void StateMachine::BandScanStateHandler::onUpdateDraw() {
    Ui::drawGraph(
        rssiData,
        CHANNELS_SIZE,
        100,
        GRAPH_X,
        GRAPH_Y,
        GRAPH_W,
        GRAPH_H
    );

    Ui::display.drawFastHLine(
        BORDER_BOTTOM_X,
        BORDER_BOTTOM_Y,
        BORDER_BOTTOM_W,
        WHITE
    );

    Ui::clearRect(
        PROGRESS_X,
        PROGRESS_Y,
        PROGRESS_W,
        PROGRESS_H
    );

    uint8_t progressW = orderedChanelIndex * PROGRESS_W / CHANNELS_SIZE + 1;
    Ui::display.fillRect(
        PROGRESS_X,
        PROGRESS_Y,
        progressW,
        PROGRESS_H,
        WHITE
    );

    Ui::needDisplay();
}
