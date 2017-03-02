class TimeTemplate {
	int year, month, day;
	int hour, minute, second;
	long milliSecond;
	
	PFont clockFont;

	TimeTemplate() {
		
		// init 2000-01-01 0:0:0:00
		year = 2000;
		month = 1;
		day = 1;
		hour = 0;
		minute = 0;
		second = 0;
		milliSecond = 0;
		
		clockFont = loadFont("SFUIDisplay-Black-24.vlw");
	}

	int getYear() 			{   return year;    }
	int getMonth()  		{   return month;	}
	int getDay() 			{	return day;		}
	int getHour() 			{	return hour;	}
	int getMinute() 		{	return minute;	}
	int getSecond() 		{	return second;	}
	long getMilliSeconds()	{	return System.currentTimeMillis() % 100;	} // milliseconds 
	
	String getDateTime(){
		String returnTime = getDateString() + " " + getClockString();
		return returnTime;
	}

	void getLocalTime() {
		year = year();
		month = month();
		day = day();
		hour = hour();
		minute = minute();
		second = second();
		milliSecond = getMilliSeconds();
	}

	// todo : synchronize
	void synchronize() {
	}

	void update(){
		getLocalTime();
	}

	String getDateString(){
		String dateText;
		dateText = year + "/";

		if(month < 10)  dateText += "0" + month + "/";
		else            dateText += month + "/";

		if(day < 10)    dateText += "0" + day;
		else            dateText += day;
		
		return dateText;
	}

	String getClockString(){
		String clockText = "";

		if(hour < 0)    clockText += "0" + hour + ":";
		else            clockText += hour + ":";

		if(minute < 0)  clockText += "0" + minute + ":";
		else            clockText += minute + ":";

		if(second < 0)  clockText += "0" + second + ":";
		else            clockText += second + ":";

		if(milliSecond < 10)            clockText += "00" + milliSecond;
		else if (milliSecond < 100)     clockText += "0" + milliSecond;
		else                            clockText += milliSecond;

		return clockText;
	}
	
	void draw(float _x, float _y, boolean clockOnly) {
		pushMatrix();
		pushStyle();
		textFont(clockFont, 24);
		fill(255, 255, 255);
		translate(_x, _y);
		if (!clockOnly) { 
			text(getDateString(), 0, 0);            
			translate(0, 24);
		}

		text(getClockString(), 0, 0);
		popStyle();
		popMatrix();
	}
}