GForceVis gVis;
TwoDimensionGraph XVis;
TwoDimensionGraph YVis;
float GFMin, GFMax;

int numberOfBuffer = 500;


void setup(){
	size(1440, 800);
    frameRate(60);

    GFMin = 256;
    GFMax = 640;
    gVis = new GForceVis(250, 200, 300, 300, 384, 640, 4);
    XVis = new TwoDimensionGraph("X-AXIS", 550, 200, 600, 300, 384, 640);
    YVis = new TwoDimensionGraph("Y-AXIS", 550, 600, 600, 300, 384, 640);
}

void draw(){
    
    // demonstration of arduino sensor value
    float tempX = randomGaussian() * 10 + 512;
    float tempY = randomGaussian() * 10 + 512;
    
    pushData(tempX, tempY);
    
	background(10, 10, 10);
    gVis.draw();
    XVis.draw();
    YVis.draw();
    
}

void pushData(float _dataX, float _dataY){
    PVector data = new PVector(_dataX, _dataY);
    gVis.pushData(data);
    XVis.pushData(data.x);
    YVis.pushData(data.y);
}