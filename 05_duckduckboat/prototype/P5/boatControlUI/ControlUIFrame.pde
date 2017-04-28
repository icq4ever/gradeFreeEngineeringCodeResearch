/*
    duck boat control UI
*/

class ControlUIFrame extends PApplet {
    int w, h;
    PApplet parent;
    ControlP5 cp5;
    Textarea description;
    Toggle toggleOffline, toggleControlOverride;

    public ControlUIFrame(PApplet _parent, int _w, int _h, String _name) {
        super();   
        parent = _parent;
        w=_w;
        h=_h;
        PApplet.runSketch(new String[]{this.getClass().getName()}, this);
        
    }

    public void settings() {
        size(w, h);
    }

    public void setup() {
        surface.setLocation(0, 20);
        cp5 = new ControlP5(this);

        description = cp5.addTextarea("desc")
            .setPosition(20, 200)
            .setLineHeight(14)
            .setColor(color(255));
        //description.setText("LDUCKBOAT CONTROLLER\n\nALT+L \t: load settings\nALT+S\t: save settings\nSPACE\t: manual control on/off \nZ\t: sudo offline on/off");
        description.setText("LOVEDUCKBOAT CONTROLLER\n\nSPACE\t: manual control on/off \nZ\t: sudo offline on/off\n<, > : throttle");

        toggleOffline = cp5.addToggle("offline")
            .plugTo(parent, "bKillEnabled")
            .setPosition(20, 20)
            .setSize(20, 20)
            .setValue(true);        // for safety
        toggleOffline.getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.TOP).setPaddingX(5);

        toggleControlOverride = cp5.addToggle("manualcontrol\noverride")
            .plugTo(parent, "bManualControlEnabled")
            .setPosition(150, 20)
            .setSize(20, 20)
            .setValue(false);
        toggleControlOverride.getCaptionLabel().align(ControlP5.RIGHT_OUTSIDE, ControlP5.TOP).setPaddingX(5);

        cp5.addSlider("accel-sensitivity")
            .plugTo(parent, "throttleSensitivity")
            .setRange(0, 1)
            .setNumberOfTickMarks(21)
            .setValue(0.5)
            .setPosition(20, 70)
            .setSize(260, 20)
            .getCaptionLabel().align(ControlP5.LEFT, ControlP5.TOP_OUTSIDE).setPaddingX(5);


        cp5.addSlider("handling-sensitivity")
            .plugTo(parent, "handlingSensitivity")
            .setRange(0, 1)
            .setNumberOfTickMarks(21)
            .setValue(0.5)
            .setPosition(20, 120)
            .setSize(260, 20)
            .getCaptionLabel().align(ControlP5.LEFT, ControlP5.TOP_OUTSIDE).setPaddingX(5);

        //cp5.getController("accel-sensitivity").getCaptionLabel().align(ControlP5.LEFT, ControlP5.TOP_OUTSIDE).setPaddingX(0);
        //cp5.getController("handling-sensitivity")


        //// save/load key mappding setting
        //cp5.mapKeyFor(new ControlKey() { public void keyEvent() { cp5.saveProperties("settings.json");}}, ALT, 's');
        //cp5.mapKeyFor(new ControlKey() { public void keyEvent() { cp5.loadProperties("settings.json");}}, ALT, 'l');

        // offline mode keymapping
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                bKillEnabled = !bKillEnabled;
                if (!bKillEnabled) {
                    toggleOffline.setValue(false);
                    keyThrottleValue = 0;
                    handlingValue = 0;
                } else {
                    toggleOffline.setValue(true);
                }
            }}, 'z');

        // manual control override keyMapping setting
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                bManualControlEnabled = !bManualControlEnabled;
                if (!bManualControlEnabled) {
                    toggleControlOverride.setValue(false);
                    keyThrottleValue = 0;
                    handlingValue = 0;
                } else {
                    toggleControlOverride.setValue(true);
                }
            }}, ' ');
            
        // manual control key
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                if (bManualControlEnabled) {
                    keyThrottleValue++;
                    if(keyThrottleValue > 100)    keyThrottleValue = 100;
                }
            }}, '.');
            
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                if (bManualControlEnabled) {
                    keyThrottleValue++;
                    if(keyThrottleValue > 100)    keyThrottleValue = 100;
                }
            }}, '>');
            
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                if (bManualControlEnabled) {
                    keyThrottleValue--;
                    if(keyThrottleValue < -100)    keyThrottleValue = -100;
                }
            }}, ',');
        cp5.mapKeyFor(new ControlKey() { public void keyEvent() { 
                if (bManualControlEnabled) {
                    keyThrottleValue--;
                    if(keyThrottleValue < -100)    keyThrottleValue = -100;
                }
            }}, '<');
    }

    void draw() {
        background(#5F5F5F);
        pushStyle();
        imageMode(CENTER);
        
        image(duckImage, 235, 240 + sin(radians(millis())/10)*4, 52, 52);
        image(waterImage, 230-waterImage.width/2, 250, 52, 52);
        image(waterImage, 230+waterImage.width/2, 250, 52, 52);
        popStyle();
        pushStyle();
        noFill();
        stroke(#EFEFEF);
        line(20, 180, this.width-20, 180);
        popStyle();
    }
}