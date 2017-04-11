/*
    vertical slider UI (output only) 
 */

class VSlider {
    float w, h;
    float min, max;
    float normalizedValue;        // normalized value (0 ~ 1);
    float sliderHeight;
    int   rawValue;
    boolean bOn;
    String title;

    float lowThreshold;
    float highThreshold;

    PFont interfaceFont;
    PFont debugFont;

    int controlType;

    // _min  : min value of input
    // _max  : max value of output
    // _w    : width of slider
    // _h    : height of slider
    VSlider(String _title, float _min, float _max, float _highThreshold, int _controlType) {
        title = _title;
        min = _min;
        max = _max;
        w = 30;
        h = 200;
        rawValue = 0;
        highThreshold = _highThreshold;

        controlType = _controlType;

        // UI font setting
        interfaceFont = loadFont("FiraSans-Regular-80.vlw");    
        debugFont = loadFont("Monospaced-14.vlw");
    }

    void setLowThreshold(float _lowThreshold) {
        lowThreshold = _lowThreshold;
    }

    // update slider value
    boolean update(float _inputValue) {
        rawValue = (int)_inputValue;
        if (rawValue <= min)    rawValue = 0;
        normalizedValue = constrain(map(_inputValue, min, max, 0, 1), 0, 1);
        sliderHeight = map(normalizedValue, 0, 1, 0, h);

        if (controlType == 0) {
            if (rawValue > highThreshold)    bOn = true;
            else                            bOn = false;
        } else if (controlType == 1) {    // middle
            if (lowThreshold < rawValue && rawValue < highThreshold)    bOn = true;
            else                                                       bOn = false;
        }
        return bOn;
    }

    // draw slider. 
    // _x, _y is left/bottom of slider   
    void draw(float _x, float _y) {
        // draw
        pushMatrix();
        {
            translate(_x, _y);

            // slider 
            pushStyle();

            if (bOn)    fill(#FF0000);
            else           fill(#00e227);

            noStroke();
            rect(55, 50+h-sliderHeight, w, sliderHeight);
            popStyle();

            // slider stroke
            pushStyle();
            stroke(#FFFFFF);
            noFill();
            rect(55, 50, w, h);
            popStyle();

            pushStyle();
            textFont(interfaceFont, 80);
            textAlign(RIGHT, BOTTOM);
            fill(#FFFF00);
            if (rawValue <= 0) {
                text("--", 260, 265);
            } else {
                text(rawValue, 260, 265);
            }
            popStyle();

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
            rectMode(CENTER);
            stroke(#FFFFFF);
            noFill();
            rect(150, 150, 260, 260);
            popStyle();
        }
        popMatrix();
    }
}