import processing.serial.*;

float accX, accY, accZ, gyroX, gyroY, gyroZ, magX, magY, magZ;
float temperature;

void setup(){
    size(400, 400, P3D);
}

void draw(){

}

void serialEvent(Serial port){
    try{
        String inString = port.readStringUntil('\n');

        if(inString != null){
            inString = trim(inString);
            String[] values = inString.split(",");

            accX = Float.parseFloat(values[0]);
            accY = Float.parseFloat(values[1]);
            accZ = Float.parseFloat(values[2]);

            gyroX = Float.parseFloat(values[3]);
            gyroY = Float.parseFloat(values[4]);
            gyroZ = Float.parseFloat(values[5]);

            magX = Float.parseFloat(values[6]);
            magY = Float.parseFloat(values[7]);
            magZ = Float.parseFloat(values[8]);

            temperature = Float.parseFloat(values[9]);
        }

    } catch (RuntimeException e){

    }
}