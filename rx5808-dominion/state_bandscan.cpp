


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
int rssi = 0;
int disp = 0;
int chan =0;
int peakrssi = 0;
int peakindex = 0;
int peakchan = 0;
static Timer rssipeak = Timer(8 * 1000); //timer in seconds
int RSSI_THRESH = 95;

void StateMachine::BandScanStateHandler::onUpdate() {
    if (!Receiver::isRssiStable())
        return;

    #ifdef USE_DIVERSITY
        rssiData[orderedChanelIndex] = (Receiver::rssiA + Receiver::rssiB) / 2;
    #else
        rssiData[orderedChanelIndex] = Receiver::rssiA;
    #endif

    //variable used to change fill color based on what frequency is detected
    chan = constrain(
            map(
                orderedChanelIndex,
                0,
                CHANNELS_SIZE,
                0,
                255
            ),
            0,
            255
        );

    //variable used to display rssi value
    int disp = 0;
    rssi = rssiData[orderedChanelIndex];
    disp = rssi * NUM_LEDS; //scale it a bit to cover our #of led's

    //well dump disp points into each LED until we run out and display the last led as variable brightness
    int j = 0;
    if (disp > BRIGHTNESS){
      while( disp > BRIGHTNESS) {
        disp = disp-BRIGHTNESS;
        //led[#]=CHSV(hue, staturation, brightness);
        leds[j] = CHSV(chan, 255, 55);//faint glow based of scanned frequency
        j = j + 1;//somehow i didn't trust ++j lol
        if(j> NUM_LEDS){//because we can totally walk off this array if brightness is too high or something, idk
          disp = 0;
          break;
        }
      }
    }
    if( (disp <= BRIGHTNESS) && (j<=NUM_LEDS)) {
      leds[j] = CHSV(0, 255, disp); //brightness of red based on remaining rssi for last pixel
    }

    
    if(rssipeak.hasTicked()){
      peakrssi=0;
    }
    if( (rssi >= RSSI_THRESH) && (rssi >= peakrssi) ){
      peakrssi = rssi;
      peakindex = orderedChanelIndex;
      peakchan = chan;
      leds[NUM_LEDS-1] = CHSV(peakchan, 255, BRIGHTNESS);
      rssipeak.reset();
    }
    
    // Show the leds
    FastLED.show();
    //return them all to black for next frequency.
    for ( int i=0;i<NUM_LEDS;++i){
      leds[i] = CRGB::Black;
    }
    
    //make it persist to the next channel
    if((!rssipeak.hasTicked()) && (rssi < peakrssi)){ 
      leds[NUM_LEDS-1] = CHSV(peakchan, 255, BRIGHTNESS-(BRIGHTNESS/4));
      leds[NUM_LEDS-2] = CHSV(peakchan, 255, BRIGHTNESS/2);
      leds[NUM_LEDS-3] = CHSV(peakchan, 255, BRIGHTNESS/3);
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
