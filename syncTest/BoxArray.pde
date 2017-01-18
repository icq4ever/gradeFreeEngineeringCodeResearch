class BoxArray{
    ArrayList<Boolean> pass;
    int numberOfBox = 0;
    int currentStage = 0;
    boolean nowCheck = false;
    
    BoxArray(int _numberOfBox){
        pass = new ArrayList<Boolean>();
        numberOfBox = _numberOfBox;
        for(int i=0; i<numberOfBox; i++){
            pass.add(false);
        }
    }
    
    draw(){
        
    }
}