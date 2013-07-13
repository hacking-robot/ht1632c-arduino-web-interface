

#define WEBDUINO_FAIL_MESSAGE "<h1>Request Failed</h1>"
#include "SPI.h" // new include
#include "avr/pgmspace.h" // new include
#include "Ethernet.h"
#include "WebServer.h"

// HT1632
#include <font_16x8.h>
#include <font_5x4.h>
#include <font_7x5.h>
#include <HT1632.h>
#include <images.h>

#define VERSION_STRING "0.1"

/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 192, 168, 1, 252 };

// ROM-based messages used by the application
// These are needed to avoid having the strings use up our limited
//    amount of RAM.

P(Page_start) = "<!DOCTYPE html><html><head><title>32x16 Led Display v" VERSION_STRING  "</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link href=\"//netdna.bootstrapcdn.com/twitter-bootstrap/2.3.2/css/bootstrap-combined.min.css\" rel=\"stylesheet\"></head><body><script src=\"//ajax.googleapis.om/ajax/libs/jquery/1.10.1/jquery.min.js\"></script><script src=\"//netdna.bootstrapcdn.com/twitter-bootstrap/2.3.2/js/bootstrap.min.js\"></script>\n";

P(Page_form) = "<div class=\"container\"><div class=\"row\">  <div class=\"span9\"><h1>32x16 Led Display</h1></div></div><br/><div class=\"row\">  <div class=\"span9\"><form method=\"POST\">  <fieldset>    <legend>Line 1</legend>    <label>Text</label>    <input type=\"text\" name=\"line1-text\" value=\"\">    <span class=\"help-block\">This will appear on the first line of the display</span><label class=\"radio\">  <input type=\"radio\" name=\"line1-color\" id=\"line1_color-1\" value=\"green\">  Green</label><label class=\"radio\">  <input type=\"radio\" name=\"line1-color\" id=\"line1_color-2\" value=\"red\">  Red</label><label class=\"radio\">  <input type=\"radio\" name=\"line1-color\" id=\"line1_color-3\" value=\"orange\">  Orange</label><label class=\"checkbox inline\">  <input type=\"checkbox\" id=\"line1-scroll\" name=\"line1-scroll\" value=\"scroll\"> Scroll line 1</label><label class=\"checkbox inline\">  <input type=\"checkbox\" id=\"line1-blink\" name=\"line1-blink\" value=\"blink\" > Blink line 1</label><br/><br/>   ";
P(Page_form_2) =    "</fieldset>  <br/><br/><br/>  <fieldset>    <legend>Line 2</legend>    <label>Text</label>    <input type=\"text\" name=\"line2-text\" value=\"\">    <span class=\"help-block\">This will appear on the second line of the display</span><label class=\"radio\">  <input type=\"radio\" name=\"line2-color\" id=\"line2_color-1\" value=\"green\" >  Green</label><label class=\"radio\">  <input type=\"radio\" name=\"line2-color\" id=\"line2_color-2\" value=\"red\">  Red</label><label class=\"radio\">  <input type=\"radio\" name=\"line2-color\" id=\"line2_color-3\" value=\"orange\">  Orange</label><label class=\"checkbox inline\">  <input type=\"checkbox\" id=\"line2-scroll\" name=\"line2-scroll\" value=\"scroll\"> Scroll line 2</label><label class=\"checkbox inline\">  <input type=\"checkbox\" id=\"line2-blink\" name=\"line2-blink\" value=\"blink\"> Blink line 2</label><br/><label>Scroll time</label><input name=\"scroll-time\"><label>Blink time</label><input name=\"blink-time\"><br/><br/>    <button type=\"submit\" class=\"btn\">Submit</button>  </fieldset></form>  </div></div></div>";
P(Page_end) = "<script>jQuery.ready(function() {jQuery('#line1-scroll').on('change', function() {jQuery('#line1-blink').attr('checked', false);});	jQuery('#line1-blink').on('change', function() {jQuery('#line1-scroll').attr('checked', false);	});	jQuery('#line2-scroll').on('change', function() {jQuery('#line2-blink').attr('checked', false);	});	jQuery('#line2-blink').on('change', function() {jQuery('#line2-scroll').attr('checked', false);	});});</script></body></html>";
P(Tail_end) = "'<br>\n";
P(Parsed_tail_begin) = "URL parameters:<br>\n";
P(Parsed_item_separator) = " = '";
P(Line_break) = "<br>\n";



/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);


#define NAMELEN 32
#define VALUELEN 128
char clear_line[OUT_SIZE+1];

int processServerTimer = 200;
int processServerTimer_count;

char Line1_Color[8]={'red'};
char Line1_Text[64];
boolean Line1_Scroll=false;
boolean Line1_Blink=false;
int Line1_Width;
int Line1_Width_i;
int Line1_Scroll_Time=20;
int Line1_Blink_Time=1000;
int Line1_Scroll_Time_Count;
int Line1_Blink_Time_Count;
boolean Line1_Blink_show=true;

char Line2_Color[8]={'red'};
char Line2_Text[64];
boolean Line2_Scroll=false;
boolean Line2_Blink=false;
int Line2_Width;
int Line2_Width_i;
int Line2_Scroll_Time=20;
int Line2_Blink_Time=1000;
int Line2_Scroll_Time_Count;
int Line2_Blink_Time_Count;
boolean Line2_Blink_show=true;

/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void indexCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{

  char name[NAMELEN];
  char value[VALUELEN];
  
  for(int i=0;i<OUT_SIZE;i++)
  {
    clear_line[i]=' ';
  }
  clear_line[OUT_SIZE+1]='\0';
  
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  server.printP(Page_form);
  server.printP(Page_form_2);
  
  if (type == WebServer::POST)
  {   
    Line1_Blink = false;
    Line1_Scroll = false;
    Line2_Blink = false;
    Line2_Scroll = false;
    Line1_Blink_show=true;
    Line1_Scroll_Time_Count=0;
    Line1_Blink_Time_Count=0;
    Line2_Scroll_Time_Count=0;
    Line2_Blink_Time_Count=0;    
    
    while (server.readPOSTparam(name, NAMELEN, value, VALUELEN))
    {
       // Line 1
        if(strcmp(name, "line1-text") == 0)
          strcpy(Line1_Text, value);
          
        if(strcmp(name, "line1-color") == 0)
          strcpy(Line1_Color, value);

        if(strcmp(name, "line1-blink") == 0)
          Line1_Blink = true;
       
        if(strcmp(name, "line1-scroll") == 0)
          Line1_Scroll = true;
          
       // Line 2
        if(strcmp(name, "line2-text") == 0)
          strcpy(Line2_Text, value);
          
        if(strcmp(name, "line2-color") == 0)
          strcpy(Line2_Color, value);

        if(strcmp(name, "line2-blink") == 0)
          Line2_Blink = true;
          
        if(strcmp(name, "line2-scroll") == 0)
          Line2_Scroll = true;     
  
        if(strcmp(name, "scroll-time") == 0)
        {
          Line1_Scroll_Time = atoi(value);
          Line2_Scroll_Time = atoi(value);
        }
  
        if(strcmp(name, "blink-time") == 0)
        {
          Line1_Blink_Time = atoi(value);
          Line2_Blink_Time = atoi(value);
        }    

    }    
Serial.println(Line1_Scroll_Time);
Serial.println(Line1_Blink_Time);
     // get lines width
     Line1_Width = HT1632.getTextWidth(Line1_Text, FONT_7X5_WIDTH, FONT_7X5_HEIGHT);
     Line2_Width = HT1632.getTextWidth(Line2_Text, FONT_7X5_WIDTH, FONT_7X5_HEIGHT);

    clearLine1();
    clearLine2();
    drawLine1();
    drawLine2();
  } 
  
  server.printP(Page_end);

}




void setup()
{
  Serial.begin(9600);
  
  /* initialize the Ethernet adapter */
  Ethernet.begin(mac, ip);
  
  /* initialize HT1632 */
  HT1632.begin(7, 5, 8, 6);
  
  HT1632.clear();
  HT1632.render();

  /* default page */
  webserver.addCommand("index.html", &indexCmd);

  /* start the webserver */
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);    
  
  if(Line1_Scroll == true && Line1_Scroll_Time_Count >= Line1_Scroll_Time)
  {
    Line1_Scroll_Time_Count=0;
    Line1_Width_i = (Line1_Width_i+1)%(Line1_Width + OUT_SIZE * 2);
    drawLine1(); 
  }

  if(Line2_Scroll == true && Line2_Scroll_Time_Count >= Line2_Scroll_Time)
  {       
    Line2_Scroll_Time_Count=0;
    Line2_Width_i = (Line2_Width_i+1)%(Line2_Width + OUT_SIZE * 2);
    drawLine2();  
  }  
  
  if(Line1_Blink == true && Line1_Blink_Time_Count >= Line1_Blink_Time)
  {
    if(Line1_Blink_show == true)
    {
      Line1_Blink_show = false;
      clearLine1();
    }      
    else
    {
      Line1_Blink_show = true;
      drawLine1(); 
    }  
    Line1_Blink_Time_Count=0;  
  }

  if(Line2_Blink == true && Line2_Blink_Time_Count >= Line2_Blink_Time)
  {
    if(Line2_Blink_show == true)
    {
      Line2_Blink_show = false;
      clearLine2(); 
    }      
    else
    {
      Line2_Blink_show = true;
      drawLine2(); 
    }  
    Line2_Blink_Time_Count=0;  
  }  

  delay(1);
  counter();
}

void counter()
{
  Line1_Scroll_Time_Count++;
  Line1_Blink_Time_Count++;
  Line2_Scroll_Time_Count++;
  Line2_Blink_Time_Count++;
  processServerTimer_count++;
}

void drawLine1()
{
  int x;
  if(Line1_Scroll == true)
    x = OUT_SIZE - Line1_Width_i;
  else
    x = 0;

  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.render();    
  drawLine(x, 0, Line1_Text, Line1_Color);
}

void clearLine1()
{
  drawLine(0, 0, clear_line, "green"); 
  drawLine(0, 0, clear_line, "red");    
}

void drawLine2()
{
  int x;
  if(Line2_Scroll == true)
    x = OUT_SIZE - Line2_Width_i;
  else
    x = 0;  
    
  HT1632.drawTarget(BUFFER_BOARD(2));
  HT1632.render();    
  drawLine(x, 9, Line2_Text, Line2_Color);
}

void clearLine2()
{
  drawLine(0, 9, clear_line, "green");
  drawLine(0, 9, clear_line, "red");  
}


void drawLine(int x,int y, char* text, char* color)
{  
  if(strcmp(color,"green") == 0 || strcmp(color,"orange") == 0)
  {
   HT1632.drawTarget(BUFFER_BOARD(1));  
   HT1632.drawText(text, x, y, FONT_7X5, FONT_7X5_WIDTH, FONT_7X5_HEIGHT, FONT_7X5_STEP_GLYPH); 
   HT1632.render();
  }
    
 
  if(strcmp(color,"red") == 0 || strcmp(color,"orange") == 0)
  {
    HT1632.drawTarget(BUFFER_BOARD(2));    
    HT1632.drawText(text, x, y, FONT_7X5, FONT_7X5_WIDTH, FONT_7X5_HEIGHT, FONT_7X5_STEP_GLYPH); 
    HT1632.render();
  }

}

