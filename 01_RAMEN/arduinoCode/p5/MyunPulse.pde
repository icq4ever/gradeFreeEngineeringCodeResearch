class MyunPulse{

	int xspacing = 4;   		// How far apart should each horizontal location be spaced
	int w;              		// Width of entire wave

	float theta = 0.0;  		// Start angle at 0
	float amplitude = 75.0;  	// Height of wave
	float period = 200.0;  		// How many pixels before the wave repeats
	float dx;  					// Value for incrementing X, a function of period and xspacing
	float[] yvalues;  			// Using an array to store height values for the wave
	int x;
	float strokeWidth = 5;

	MyunPulse(int _w, float _amp) {
		// size(640, 360);
		amplitude = _amp;
		w = _w+16;
		dx = (TWO_PI / period) * xspacing;
		yvalues = new float[20000/xspacing];
	}

	void calcWave() {
		// Increment theta (try different values for 'angular velocity' here
		// theta += 2;

		float x = theta;
		for (int i = 0; i < yvalues.length; i++) {
			yvalues[i] = sin(x)*amplitude;
			x+=dx;
		}
	}

	void renderWave(float _x, float _y) {
		pushMatrix();
		translate(_x, _y);

		noFill();
		stroke(255);
		strokeWeight(strokeWidth);
		// A simple way to draw the wave with an ellipse at each location
		// for (int x = 0; x < yvalues.length-1; x++) {
		  // ellipse(x*xspacing, height/2+yvalues[x], 8, 8);
		  // line(x*xspacing, height/2+yvalues[x], (x+1)*xspacing, height/2+yvalues[x+1]);
		// }

		// point(x*xspacing, height/2+yvalues[x]);
		line(x*xspacing, height/2+yvalues[x], (x+1)*xspacing, height/2+yvalues[x+1]);
		x++;
		
		if(x > w/xspacing-1)  {
			x = 0;
			fill(0);
			rectMode(CENTER);
			noStroke();
			rect(w/2, 200, w+50, 400);
		}

		
		pushStyle();
		fill(0, 10);
		noStroke();
		rectMode(CENTER);
		rect(w/2, 200, w+50, 400);
		popStyle();

		noFill();
		stroke(255);
		strokeWeight(1);
		
		popMatrix();
	}

	void setPeriod(float _period){
		period = _period;
	}

	void setAmplitude(float _amp){
		amplitude = _amp;
	}

	void setWidth(int _w){
		w = _w+16;

		// update period, and dx
		period = _w/4;
		dx = (TWO_PI / period) * xspacing;
	}
}