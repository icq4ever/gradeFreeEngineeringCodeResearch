MyunPulse t;

void setup(){
	size(800, 400);
	t = new MyunPulse(800, 75);
	background(0);
}

void draw(){
	// fill(0);
	// rect(0, 0, width, height);
	// background(0);
	// t.setWidth(mouseX+50);
	t.setPeriod(map(mouseX, 0, width, 0, 1));
	t.calcWave();
	t.renderWave(0, 0);
	t.setAmplitude(mouseY/2);

	
	// rect(0, 0, width, height);
	// fill(0, 4);
	// noStroke();h
	
}