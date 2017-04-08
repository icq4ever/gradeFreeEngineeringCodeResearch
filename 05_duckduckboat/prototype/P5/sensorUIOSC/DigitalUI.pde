/*
    digital UI (output only) 
*/

class DigitalUI{
    boolean bOn;
    PImage icon;
    PFont debugFont;
    String title;
    
    DigitalUI(String _title, String _iconFileName){
        title = _title;
        bOn = false;
        icon = loadImage(_iconFileName);
        debugFont = loadFont("Monospaced-12.vlw");
    }
    
    void update(boolean _isOn){
        bOn = _isOn;
    }
    
    void draw(float _x, float _y){
        pushMatrix();
        {
            translate(_x, _y);
            imageMode(CENTER);
            if(bOn)    image(icon, 150, 150);
            
            // display theme
            pushStyle();
            fill(#FFFFFF);
            noStroke();
            textFont(debugFont, 12);
            textAlign(RIGHT, TOP);
            text(title, 260, 40);
            popStyle();
            
            // draw frame
            pushStyle();
            stroke(#FFFFFF);
            noFill();
            rect(20, 20, 260, 260);
            popStyle();
            
            
        }
        popMatrix();
    }
}