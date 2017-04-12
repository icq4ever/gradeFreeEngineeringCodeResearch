import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress remote;

String demo1="";
String demo2="";

PFont font;

void setup() {
  size(800, 300);
  frameRate(30);
  
  font = loadFont("ACaslonPro-Bold-72.vlw");
  
  oscP5 = new OscP5(this, 8000);

  //remote = new NetAddress("127.0.0.1", 8000);
  textFont(font, 72);
}

void draw() {
  background(0);
  textAlign(LEFT);
  fill(255);
  text("DEMO1 : " + demo1, 100, height/2-40);
  text("DEMO2 : " + demo2, 100, height/2+40);
  
  println(demo1 + "\t" + demo2);
}

void oscEvent(OscMessage theOscMessage) {
  if (theOscMessage.checkAddrPattern("/GFE_1")==true) {
    demo1 = theOscMessage.get(0).stringValue();
  }

  if (theOscMessage.checkAddrPattern("/GFE_2")==true) {
    demo2 = theOscMessage.get(0).stringValue();
  }
}