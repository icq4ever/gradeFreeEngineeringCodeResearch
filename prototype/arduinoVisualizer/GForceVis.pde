class GForceVis {
    // float x, y;
    float w, h;
    float min, max;
    int step;
    ArrayList<PVector>history;
    PVector data;

    GForceVis(float _w, float _h, float _min, float _max, int _step) {
        // x = _x;
        // y = _y;
        w = _w;
        h = _h;
        min = _min;
        max = _max;
        step = _step;

        history = new ArrayList<PVector>();
    }
    
    void record(PVector loc){
        history.add(loc);
    }

    void pushData(PVector _data) {
        data = _data;
    }

    PVector normalizeData(PVector _data) {
        PVector _t = new PVector(map(_data.x, min, max, -w/2, w/2), map(_data.y, min, max, -h/2, h/2));
        return _t;
    }

    void draw(float _x, float _y) {
        stroke(255, 255, 255);
        noFill();


        pushMatrix();
        translate(_x, _y);
        
                
        // line(-w/2-10, 0, w/2+10, 0);
        // line(0, -w/2-10, 0, w/2+10);
        strokeWeight(10);
        point(0, 0);

        strokeWeight(1);
        for (int i=0; i<step+1; i++) {
            ellipse(0, 0, i*w/step, i*h/step);
        }
        
        
        // draw history
        if(history.size()!=0){
            pushStyle();
            fill(255, 0, 255);
            for(int i=0; i<history.size(); i++){
                ellipse(normalizeData(history.get(i)).x, normalizeData(history.get(i)).y, 10, 10);
            }
            popStyle();
        }

        pushStyle();
        fill(255, 255, 0);
        noStroke();
        ellipse(normalizeData(data).x, normalizeData(data).y, 10, 10);
        popStyle();

        popMatrix();

        // title
        //pushMatrix();
        //pushStyle();
        //translate(x - w/2, y - h/2);
        //fill(255, 255, 0);
        //noStroke();
        //rect(-20, 0, 200, 20);

        //fill(0);
        ////text("FRONT :: " + "X : " + int(min) + "/ Y: " + int(max), -10, 14);

        //popStyle();
        //popMatrix();
    }
}