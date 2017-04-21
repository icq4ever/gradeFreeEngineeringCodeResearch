class TwoDimensionUI{
    float w, h;
    PVector min, max;
    PVector rawValue;
    PVector normalizedValue;
    float size;
    
    boolean bOn;
    String title;

    PFont interfaceFont;
    PFont debugFont;
    
    
    TwoDimensionUI(String _title, float _size){
        title = _title;
        rawValue = new PVector(0, 0);
        min = new PVector(-100, -100);
        max = new PVector(100, 100);
        size = _size;
        normalizedValue = new PVector(0, 0);
    }
    
    void update(PVector _inputValue){
        rawValue = _inputValue;
        normalizedValue.x = map(rawValue.x, -100, 100, -1, 1);
        normalizedValue.y = map(rawValue.y, -100, 100, 1, -1); 
    }
    
    void draw(float _x, float _y){
        pushMatrix();
        {
            translate(_x, _y);
            pushStyle();
            rectMode(CENTER);
            noFill();
            rect(0, 0, w, h);
            popStyle();

            pushStyle();
            noFill();
            stroke(#FFFF00);
            strokeWeight(5);
            point(    map(normalizedValue.x, -1, 1, -size/2+10, size/2-10), 
                      map(normalizedValue.y, -1, 1, -size/2+10, size/2-10)
                      );

            popStyle();

            pushStyle();
            rectMode(CORNER);
            fill(#00FF00);
            noStroke();
            // if(normalizedValue.x > 0)           
            rect(0, -10, map(normalizedValue.x, -1, 1, -size/2+10, size/2-10), 20);

            fill(#FFFF00);
            rect(-10, 0, 20, map(normalizedValue.y, -1, 1, -size/2+10, size/2-10));
            popStyle();
        }
        popMatrix();
        
    }
}