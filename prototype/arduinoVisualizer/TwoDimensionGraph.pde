class TwoDimensionGraph {
    int numberOfBuffer = 800;
    float[] buffer = new float[numberOfBuffer];
    
    float w, h;
    float max, min, base;
    String title;
    float data;

    TwoDimensionGraph(String _title, float _w, float _h, float _min, float _max) {
        
        w = _w;
        h = _h;
        min = _min;
        max = _max;
        title = _title;

        for (int i=0; i<numberOfBuffer; i++) {
            buffer[i] = 0;
        }
    }


    void pushData(float _data) {
        data = _data;
        for (int i=0; i<numberOfBuffer; i++) {
            if (i == numberOfBuffer - 1) {
                buffer[i] = _data;
            } else {
                buffer[i] = buffer[i+1];
            }
        }
    }


    void draw(float _x, float _centerY) {
        //title
        pushStyle();
        fill(255, 255, 0);
        noStroke();
        rect(_x, _centerY - h/2, 100, 20);
        fill(0);
        
        text(title + " : " + data, _x + 10, _centerY - h/2 + 14);
        popStyle();
        
        pushStyle();
        stroke(255, 255, 255);
        noFill();
        pushMatrix();
        translate(_x, _centerY);
        line(0, -h/2, 0, h/2);
        line(0, 0, numberOfBuffer, 0);
        
        pushStyle();
        stroke(255, 255, 0);
        //noStroke();
        strokeWeight(2);
        for(int i=0; i<numberOfBuffer-1; i++){
            line(i, constrain(map(buffer[i], min, max, h/2, -h/2), -h/2, h/2), i+1, constrain(map(buffer[i+1], min, max, h/2, -h/2), -h/2, h/2));
            //ellipse(i, buffer[i], 2, 2);
        }
        popStyle();
        
        popMatrix();
        popStyle();
    }
}