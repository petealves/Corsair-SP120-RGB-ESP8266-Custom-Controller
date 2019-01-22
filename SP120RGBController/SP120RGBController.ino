#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

#define LEDS_PER_FAN 1
#define NR_FANS 3
#define NUM_LEDS (NR_FANS * LEDS_PER_FAN)
#define DATA_PIN 2
#define LED_TYPE UCS1903
#define COLOR_ORDER BRG
int FRAMES_PER_SECOND = 60;

CRGB leds[NUM_LEDS];

#define ssid      "YOUR_SSID"       // WiFi SSID
#define password  "YOUR_PASSWORD"  // WiFi password


ESP8266WebServer server ( 80 );
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int rainbowCycle = 0;
int juggleEffect = 0;
int bpmEffect = 0;

int status_on = 1;
int brightness = 255;
int brightnessPercentage = (brightness*100)/255;
String color = "#FFFFFF";
int r, g, b = 0;
String effect = "";

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

String getPage(){
  String page = "<html lang='en-US'><head><meta name='viewport' content='width=device-width, initial-scale=1'/>";
  page += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";
  page += "<link href='https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css' rel='stylesheet'> <script src='https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js'></script>";
  page += "<link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.5.0/css/all.css' integrity='sha384-B4dIYHKNBt8Bc12p+WXckhzcICo0wtJAoU8YZTY5qE0Id1GSseTk6S+L3BlXeVIU' crossorigin='anonymous'>";
  page += "<title>ESP8266 Corsair SP120 RGB Controller</title></head><body style='background-color:#212121'>";
  page += "<div class='container-fluid' style='width:70%'>";
  page +=       "<br>";
  page +=   "<div class='row'>";
  page +=     "<div class='col-md-12' style='text-align:center'>";
  page +=       "<h1 style='color:white;'>ESP8266 Corsair SP120 RGB Controller</h1>";
  page +=       "<div class='row'>";
  page +=         "<div class='col-md-2'><h4 class ='text-left' style='color:white;'>Status: ";
  page +=           "<input id='toggle-status' type='checkbox' checked data-toggle='toggle' data-onstyle='success' data-offstyle='danger' data-size='small' onchange='handleStatus(this.checked)'>";
  page +=         "</div>";
  page +=       "</div>";
  page +=       "<br>";
  page +=       "<div class = 'row'>";
  page +=         "<div class='col-md-6'><h4 class ='text-left' style='color:white;'> Brightness: ";
  page +=           "<span id='slider-value' class='badge'>";
  page +=             brightnessPercentage;
  page +=           "</span></h4>";
  page +=          "<input type='range' style='width:40%' min='0' max='100' value='"+(String)brightnessPercentage+"' class='slider' id='slider-brightness' onchange='sendBrightness(this.value)'/>";
  page +=         "</div>";
  page +=       "</div>";
  page +=       "<br>";
  page +=       "<div class = 'row'>";
  page +=         "<div class='col-md-6'><h4 class ='text-left' style='color:white;'> Color: ";
  page +=           "<span id='color-picker-value' class='badge'>";
  page +=             "<input type='color' name='color' id='color-picker' value='"+color+"' onchange='changeColor(this.value)' />";
  page +=           "</span></h4>";  
  page +=         "</div>";
  page +=       "</div>";
  page +=       "<br>";
  page +=       "<div class = 'row'>";
  page +=         "<div class='col-md-6'><h4 class ='text-left' style='color:white;'> Effects: ";
  page +=           "<div class='dropdown'>";
  page +=               "<button class='btn btn-info dropdown-toggle' type='button' id='dropdownEffectsButton' data-toggle='dropdown' aria-haspopup='true' aria-expanded='false'>";
  page +=                   "Effects";
  page +=               "</button>";
  page +=               "<div class='dropdown-menu' aria-labelledby='dropdownEffectsButton'>";
  page +=                 "<a class='dropdown-item' onclick=\"handleEffect('rainbowcycle')\"> Rainbow Cycle </a>";
  page +=                 "<a class='dropdown-item' onclick=\"handleEffect('fixedcolor')\"> Fixed Color</a>";  
  page +=                 "<a class='dropdown-item' onclick=\"handleEffect('bpm')\"> Bpm </a>";
  page +=                 "<a class='dropdown-item' onclick=\"handleEffect('juggle')\"> Juggle</a>";
  page +=               "</div>";
  page +=            "</div>";
                      if (effect == "rainbowcycle"){
                        "<div class='col-md-6'><h4 class ='text-left' style='color:white;'> Rainbow Speed (Frames Per Second): ";
  page +=               "<span id='slider-rainbow-speed-value' class='badge'>";
  page +=                 FRAMES_PER_SECOND;
  page +=               "</span></h4>";
  page +=               "<input type='range' style='width:40%' min='5' max='1000' value='"+(String)FRAMES_PER_SECOND+"' class='slider' id='slider-rainbow-speed' onchange='handleBrightnessSpeed(this.value)'/>";                      
                      }
  page +=         "</div>";
  page +=         "</div>";
  page +=       "</div>";
  page +=     "<br><br><p><a href='https://github.com/petealves' target='_blank'><i style='font-size:35px' class='fab'>&#xf09b;</i></p>";
  page += "</div></div></div>";
  page += "<script>";

  page +=       "function handleStatus(value){";
  page +=       "var request = new XMLHttpRequest();";
  page +=       "request.open('POST', '/');";
  page +=       "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page +=       "request.send(encodeURI('status')+'='+value);}"; 
  
  page +=       "function sendBrightness(value){";
  page +=       "document.getElementById('slider-value').innerHTML = value; ";
  page +=       "var request = new XMLHttpRequest();";
  page +=       "request.open('POST', '/');";
  page +=       "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page +=       "request.send(encodeURI('brightness')+'='+value);}"; 
   
  page +=       "function changeColor(value){";
  page +=       "var request = new XMLHttpRequest();";
  page +=       "request.open('POST', '/');";
  page +=       "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page +=       "request.send(encodeURI('color')+'='+value);}";  
  
  page +=       "function handleEffect(effect){";
  page +=       "var request = new XMLHttpRequest();";
  page +=       "request.open('POST', '/');";
  page +=       "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page +=       "request.send(encodeURI('effect')+'='+effect);";
  page +=       "}";  

  page +=       "function handleBrightnessSpeed(value){";
  page +=       "document.getElementById('slider-rainbow-speed-value').innerHTML = value; ";
  page +=       "var request = new XMLHttpRequest();";
  page +=       "request.open('POST', '/');";
  page +=       "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');";
  page +=       "request.send(encodeURI('rainbow_speed')+'='+value);}"; 
  
  page += "</script>";
  page += "</body></html>";
  return page;
}

void handleRoot() {
  //digitalWrite(led, 1);
  if ( server.hasArg("status") ) {
    handleStatus();
  }

  if (server.hasArg("brightness")){
    handleBrightness();
  }

  if (server.hasArg("color")){
    color = server.arg("color");
    handleColor();
  }

  if (server.hasArg("effect")){
    
    effect = server.arg("effect");
    if (effect == "rainbowcycle") { 
          juggleEffect = 0; 
          bpmEffect = 0;
          rainbowCycle = 1;
    }else if(effect == "fixedcolor") {
          handleColor();
    }else if(effect == "juggle"){
          rainbowCycle = 0;
          bpmEffect = 0;
          juggleEffect = 1;
    }
    else if(effect == "bpm"){
          rainbowCycle = 0;
          juggleEffect = 0;
          bpmEffect = 1;
    }
    server.send ( 200, "text/html", getPage() );
  }

  if (server.hasArg("rainbow_speed")){
    FRAMES_PER_SECOND = server.arg("rainbow_speed").toInt();
    Serial.println(FRAMES_PER_SECOND);
  }
  
  server.send ( 200, "text/html", getPage() );
  
}

void handleStatus(){
  if(server.arg("status")== "false"){
    for(int i=0; i<=NUM_LEDS-1; i++){
      leds[i] = CRGB::Black;
    }
    status_on = 0;
    rainbowCycle = 0;
    juggleEffect = 0;
    bpmEffect = 0;
    FastLED.show();
    
  }else{
    handleColor();
    status_on = 1;
  }
}

void handleBrightness(){
  brightnessPercentage = server.arg("brightness").toInt();
  float brightnessCalculated = ((float)brightnessPercentage/(float)100)*255;
  brightness = (int)brightnessCalculated;
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Corsair-SP120-RGB Controller");
  WiFi.begin(ssid, password);
  
  Serial.println("");
  
  //-----------------------------------
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);         
  FastLED.setBrightness(255);
  FastLED.show();
  //-----------------------------------
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  for(int i=0; i<=NUM_LEDS-1; i++){
    leds[i] = CRGB::White;
  }
  FastLED.show();
}



void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  if (rainbowCycle == 1) {
    fill_rainbow( leds, NUM_LEDS, gHue, 7);
    FastLED.show(); 
    FastLED.delay(2000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }else if (juggleEffect == 1){
    FRAMES_PER_SECOND = 60;
    handleJuggle();
    FastLED.show(); 
    FastLED.delay(2000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }else if (bpmEffect == 1){
    FRAMES_PER_SECOND = 60;
    handleBpm();
    FastLED.show(); 
    FastLED.delay(2000/FRAMES_PER_SECOND); 
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }
  delay(10);
}

void handleColor() {

  rainbowCycle = 0;
  juggleEffect = 0;
  bpmEffect = 0;
  
  
  String colorReplaced = color;
  colorReplaced.remove(0, 1); // Remove first char (#)
  long number = (long) strtol( &colorReplaced[0], NULL, 16);
  int l_r = number >> 16;
  int l_g = number >> 8 & 0xFF;
  int l_b = number & 0xFF;

  Serial.println(l_r);
  Serial.println(l_g);
  Serial.println(l_b);

  updateColor(l_r, l_g, l_b);
}

void updateColor(int l_r, int l_g, int l_b) {
  r = l_r;
  g = l_g;
  b = l_b;
  for(int i=0; i<=NUM_LEDS-1; i++){
      leds[i] = CRGB(r, g, b);
  }
    
  FastLED.show();

}

void handleJuggle(){
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void handleBpm(){
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
