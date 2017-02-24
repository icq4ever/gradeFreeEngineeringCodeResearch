class MyunPulse{

	int xspacing = 4;   		// How far apart should each horizontal location be spaced
	int w;              		// Width of entire wave
	int h;

	float theta = 0.0;  		// Start angle at 0
	float amplitude = 75.0;  	// Height of wave
	float period = 100.0;  		// How many pixels before the wave repeats
	float dx;  					// Value for incrementing X, a function of period and xspacing
	float[] yvalues;  			// Using an array to store height values for the wave
	int x;
	float strokeWidth = 3;

	MyunPulse(int _w, int _h, float _amp) {
		// size(640, 360);
		amplitude = _amp;
		w = _w+16;
		h = _h+40;
		dx = (TWO_PI / period) * xspacing;
		yvalues = new float[20000/xspacing];
	}

	void calcWave() {
		// Increment theta (try different values for 'angular velocity' here
		// theta += 2;

		float x = theta;
		dx = (TWO_PI / period) * xspacing;
		for (int i = 0; i < yvalues.length; i++) {
			yvalues[i] = sin(x)*amplitude;
			x+=dx;
		}
	}


	void renderWave(float _x, float _y, float _scale) {
		pushMatrix();
		translate(_x, _y);
		scale(_scale, _scale);
		pushStyle();

		noFill();
		stroke(255);
		strokeWeight(strokeWidth);
		// A simple way to draw the wave with an ellipse at each location
		// for (int x = 0; x < yvalues.length-1; x++) {
		  // ellipse(x*xspacing, height/2+yvalues[x], 8, 8);
		  // line(x*xspacing, height/2+yvalues[x], (x+1)*xspacing, height/2+yvalues[x+1]);
		// }

		// point(x*xspacing, h/2+yvalues[x]);
		line(x*xspacing, h/2+yvalues[x], (x+1)*xspacing, h/2+yvalues[x+1]);
		x++;
		
		if(x > w/xspacing)  {
			x = 0;
			pushStyle();
			fill(20);
			rectMode(CORNER);
			noStroke();
			rect(0, 0, w, h);
			popStyle();
		}

		
		pushStyle();
		fill(0, 10);
		noStroke();
		rectMode(CORNER);
		rect(0, 0, w, h);
		popStyle();

		noFill();
		stroke(255);
		strokeWeight(1);
		
		popStyle();


		pushStyle();
		fill(10);
		noStroke();
		rectMode(CORNER);
		rect(-10, -10, 10, h+20);
		rect(w, -10, 10, h+20);
		popStyle();
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