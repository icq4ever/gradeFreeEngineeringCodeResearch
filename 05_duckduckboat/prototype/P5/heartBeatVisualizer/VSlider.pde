class VSlider{
    float w,h;
    float min, max;
    float vMin, vMax;
    float mappedValue;
    float sliderHeight;

    PFont interfaceFont;

    VSlider(float _min, float _max, float _w , float _h){
        min = _min;
        max = _max;
        w = _w;
        h = _h;

        interfaceFont = loadFont("FiraSans-Regular-80.vlw");
        textFont(interfaceFont, 60);
    }

    void setValueMinMax(float _vMin, float _vMax){
        vMin = _vMin;
        vMax = _vMax;
    }

    void update(float _value){
        mappedValue = map(_value, vMin, vMax, 0, 1);
        sliderHeight = map(mappedValue, 0, 1, 0, h);
    }

    void draw(float _x, float _y){
        pushMatrix();
        translate(_x , _y);

        pushStyle();
        fill(#007acc);
        noStroke();
        rect(0, h-sliderHeight, w, sliderHeight);
        popStyle();
        
        pushStyle();
        stroke(#FFFFFF);
        noFill();
        rect(0, 0, w, h);
        popStyle();
        popMatrix();
    }
}