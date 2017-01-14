
GForceVis gVis;
TwoDimensionGraph XVis;
TwoDimensionGraph YVis;
float GFMin, GFMax;
float sensorX, sensorY;

boolean cont;

TimeTemplate clock;
ShootHistoryBook shootLog;

// todo : export to json for check lately

int numberOfBuffer = 500;


void setup() {
    size(1440, 800);
    frameRate(60);
    clock = new TimeTemplate();
    shootLog = new ShootHistoryBook();

    sensorX = 0;
    sensorY = 0;
    GFMin = 256;
    GFMax = 640;
    gVis = new GForceVis(300, 300, 384, 640, 4);
    XVis = new TwoDimensionGraph("X-AXIS", 550, 300, 384, 640);
    YVis = new TwoDimensionGraph("Y-AXIS", 550, 300, 384, 640);

    cont = true;
}

void draw() {
    // demonstration of arduino sensor value
    sensorX = randomGaussian() * 10 + 512;
    sensorY = randomGaussian() * 10 + 512;

    if (cont)    pushData(sensorX, sensorY);

    background(10, 10, 10);
    gVis.draw(250, 200);
    XVis.draw(600, 200);
    YVis.draw(600, 600);

    clock.update();
    clock.draw(40, height-60, false);

    if (shootLog.getSizeOfRecord() !=0) {
        for (int i=0; i<shootLog.getSizeOfRecord(); i++) {
            pushStyle();
            fill(255, 255, 0);
            text(shootLog.timeStamp.get(i), 40, 400+i*18);
            fill(0, 255, 0);
            text(shootLog.history.get(i).x, 200, 400+i*18);
            popStyle();
        }
    }
}

void pushData(float _dataX, float _dataY) {
    PVector data = new PVector(_dataX, _dataY);
    gVis.pushData(data);
    XVis.pushData(data.x);
    YVis.pushData(data.y);
}

void mousePressed() {
    if (cont) {
        PVector _t = new PVector(sensorX, sensorY);
        String _now = clock.getDateTime();

        shootLog.pushRecord(_t, _now);
        gVis.record(_t);

        cont = false;
    }
}

void keyPressed() {
    if (key == 'c' || key == 'C') {
        cont = true;
    }
}