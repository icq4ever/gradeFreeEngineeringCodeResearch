/*
    vertical slider (output only) 
*/

class VSlider{
    float w,h;
    float min, max;
    float normalizedValue;        // normalized value (0 ~ 1);
    float sliderHeight;

    PFont interfaceFont;

    // _min  : min value of input
    // _max  : max value of output
    // _w    : width of slider
    // _h    : height of slider
    VSlider(float _min, float _max, float _w , float _h){
        min = _min;
        max = _max;
        w = _w;
        h = _h;

        // UI font setting
        interfaceFont = loadFont("FiraSans-Regular-80.vlw");    
        textFont(interfaceFont, 60);
    }

    // update slider value
    void update(float _heartRate){
        normalizedValue = constrain(map(_heartRate, min, max, 0, 1), 0, 1);
        sliderHeight = map(normalizedValue, 0, 1, 0, h);
    }

    // draw slider. 
    // _x, _y is left/bottom of slider   
    void draw(float _x, float _y){
        pushMatrix();
        {
            translate(_x , _y);
    
            // slider 
            pushStyle();
            fill(#007acc);
            noStroke();
            rect(0, h-sliderHeight, w, sliderHeight);
            popStyle();
            
            // slider stroke
            pushStyle();
            stroke(#FFFFFF);
            noFill();
            rect(0, 0, w, h);
            popStyle();
        }
        popMatrix();
    }
}