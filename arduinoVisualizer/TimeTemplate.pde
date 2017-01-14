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

    int getYear() { 
        return year;
    }

    int getMonth() {
        return month;
    }

    int getDay() {
        return day;
    }

    int getHour() {
        return hour;
    }

    int getMinute() {
        return minute;
    }

    int getSecond() {
        return second;
    }
    
    long getMilliSeconds(){
        return System.currentTimeMillis() % 100;    // milliseconds
    }
    
    String getDateTime(){
        String returnTime = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second + ":" + milliSecond; 
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
    
    void draw(float _x, float _y, boolean clockOnly) {
        pushMatrix();
        pushStyle();
        textFont(clockFont, 24);
        fill(255, 255, 255);
        translate(_x, _y);
        if (!clockOnly) {
            text(year + "/" + month + "/" + day, 0, 0);
            translate(0, 24);
        }
        text(hour + ":" + minute + ":" + second + ":" + milliSecond, 0, 0);
        popStyle();
        popMatrix();
    }
}