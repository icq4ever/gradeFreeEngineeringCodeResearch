class ButtonIndicator{
	// boolean btnOnOff[] = new boolean[10];
	float marginRight;
	float boxWidth;
	float boxHeight;
	color fillColor;
	color strokeColor;
	String btnLabel[] = new String[10];

	PFont smallFont;

	ButtonIndicator(){
		boxWidth = boxHeight = 30;
		marginRight = 40;
		fillColor 	= #FFFF00;
		strokeColor = #FFFFFF;

		smallFont = loadFont("RobotoMonoForPowerline-Medium-12.vlw");

		btnLabel[0] = "SOLENOID\n1";
    	btnLabel[1] = "SOLENOID\n2";
    	btnLabel[2] = "SOLENOID\n3";
	    btnLabel[3] = "DROP\nBALL";
	    btnLabel[4] = "HEATING\nBALL";;
	    btnLabel[6] = "SERVO\nUP";
	    btnLabel[5] = "SERVO\nDOWN";
	    btnLabel[7] = "START";
	    btnLabel[8] = "EGG\nBREAKER";
	    btnLabel[9] = "HOT\nWATER";
	}

	void draw(boolean[] _b, float _marginX, float _marginY){
		pushStyle();
		pushMatrix();
		translate(_marginX, _marginY);

		fill(#0A0A0A);
		stroke(#FFFFFF);
		rect(0, 0, 390, 250);

		// start Btn
		ellipseMode(CORNER);
		ellipse(40, 40, 30, 30);
		if(_b[7])	fill(#FF0000);
		else 		noFill();
		ellipse(40, 40, 30, 30);

		// servo UP
		noFill();	
		rect(40, 140, 30, 30);
		if(_b[6])	fill(#FFFF00);
		else 		noFill();
		rect(40, 140, 30, 30);

		// servo DOWN
		noFill();
		rect(40, 180, 30, 30);
		if(_b[5])	fill(#FFFF00);
		else 		noFill();
		rect(40, 180, 30, 30);


		// soleonid 1
		noFill();
		rect(140, 180, 30, 30);
		if(_b[0])	fill(#FFFF00);
		else 		noFill();
		rect(140, 180, 30, 30);

		// soleonid 2
		noFill();
		rect(180, 180, 30, 30);
		if(_b[1])	fill(#FFFF00);
		else 		noFill();
		rect(180, 180, 30, 30);

		// soleonid 3
		noFill();
		rect(220, 180, 30, 30);
		if(_b[2])	fill(#FFFF00);
		else 		noFill();
		rect(220, 180, 30, 30);


		// egg breaker
		noFill();
		rect(320, 100, 30, 30);
		if(_b[8])	fill(#FFFF00);
		else 		noFill();
		rect(320, 100, 30, 30);


		// ball heat
		noFill();
		rect(280, 140, 30, 30);
		if(_b[4])	fill(#FFFF00);
		else 		noFill();
		rect(280, 140, 30, 30);

		// ball drop
		noFill();
		rect(320, 140, 30, 30);
		if(_b[3])	fill(#FFFF00);
		else 		noFill();
		rect(320, 140, 30, 30);

		// hot water
		noFill();
		rect(320, 180, 30, 30);
		if(_b[9])	fill(#FFFF00);
		else 		noFill();
		rect(320, 180, 30, 30);

		popMatrix();
		popStyle();
	}
}