class TwoDimensionUI{ 
    float w, h;
    PVector min, max;
    PVector rawValue;
    PVector normalizedValue;
    float size;
    
    int graphMargin = 20;
    
    boolean bOn;
    String title;

    PFont interfaceFont;
    PFont debugFont;
    
    float CROSS_WIDTH = 80;
    float CROSS_LENGTH = 240;
    
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
    
    void setGraphMargin(int _margin){
        graphMargin = _margin;    
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
            point(    
                      //map(normalizedValue.x, -1, 1, -size/2+10, size/2-10),    // disable left right point drawing
                      0,
                      map(normalizedValue.y, -1, 1, -size/2+graphMargin, size/2-graphMargin)
                      );

            popStyle();

            pushStyle();
            rectMode(CORNER);
            
            // background
            fill(#000000);
            noStroke();
            rect(-CROSS_WIDTH/2, -CROSS_WIDTH/2-5, CROSS_WIDTH, -size/2+graphMargin);
            rect(-CROSS_WIDTH/2,  CROSS_WIDTH/2+5, CROSS_WIDTH,  size/2-graphMargin);
            
            rect( CROSS_WIDTH/2+5, -CROSS_WIDTH/2,  size/2-graphMargin, CROSS_WIDTH);
            rect(-CROSS_WIDTH/2-5, -CROSS_WIDTH/2, -size/2+graphMargin, CROSS_WIDTH);
            
            
            fill(#00FF00);
            noStroke();
            // handling
            if(normalizedValue.x > 0)    rect( CROSS_WIDTH/2+5, -CROSS_WIDTH/2, map(normalizedValue.x, -1, 1, -size/2+graphMargin, size/2-graphMargin), CROSS_WIDTH);
            else                         rect(-CROSS_WIDTH/2-5, -CROSS_WIDTH/2, map(normalizedValue.x, -1, 1, -size/2+graphMargin, size/2-graphMargin), CROSS_WIDTH);

            fill(#FFFF00);
            // throttling
            if(normalizedValue.y > 0)    rect(-CROSS_WIDTH/2,  CROSS_WIDTH/2+5, CROSS_WIDTH, map(normalizedValue.y, -1, 1, -size/2+graphMargin, size/2-graphMargin));
            else                         rect(-CROSS_WIDTH/2, -CROSS_WIDTH/2-5, CROSS_WIDTH, map(normalizedValue.y, -1, 1, -size/2+graphMargin, size/2-graphMargin));
            popStyle();
            
            // draw rects
            pushStyle();
            noFill();
            stroke(#FFFFFF);
            
            rect(-CROSS_WIDTH/2, -CROSS_WIDTH/2-5, CROSS_WIDTH, -size/2+graphMargin);
            rect(-CROSS_WIDTH/2,  CROSS_WIDTH/2+5, CROSS_WIDTH,  size/2-graphMargin);
            
            rect( CROSS_WIDTH/2+5, -CROSS_WIDTH/2,  size/2-graphMargin, CROSS_WIDTH);
            rect(-CROSS_WIDTH/2-5, -CROSS_WIDTH/2, -size/2+graphMargin, CROSS_WIDTH);
            popStyle();
        }
        popMatrix();
        
    }
}