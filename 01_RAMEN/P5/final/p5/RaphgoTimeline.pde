class RaphagoActionTime{
	String message;
	float millisTime;
	RaphagoActionTime(String _message, int _minute, int _second){
		message = _message;
		millisTime = ((_minute*60) + _second)*1000;
	}

	float getMillisTime(){
		return millisTime;
	}

	String getMessage(){
		return message;
	}
	// void setStartTime(float _startTime){
	// 	startTime = _startTime;
	// }

	// void setMessage()
	// draw(){
	// 	if()
	// }
}

class RaphagoActionList{
	boolean bStarted;
	float startTime;
	RaphagoActionTime []actionList = new RaphagoActionTime[9];
	PFont hangulFont;

	RaphagoActionList(){
		hangulFont = createFont("AppleSDGothicNeo-Bold", 80);
		bStarted = false;
		for(int i=0; i<9; i++){
			actionList[0] = new RaphagoActionTime("쇠구슬 가열\n라면체 걸기", 0, 0);
			actionList[1] = new RaphagoActionTime("쇠구슬 장착", 0, 10);
			actionList[2] = new RaphagoActionTime("체 내리기", 0, 59);
			actionList[3] = new RaphagoActionTime("쇠구슬 투하", 1, 10);
			actionList[4] = new RaphagoActionTime("체 올리기", 3, 10);
			actionList[5] = new RaphagoActionTime("찬바람", 3, 21);
			actionList[6] = new RaphagoActionTime("체 내리기", 3, 40);
			actionList[7] = new RaphagoActionTime("계란투하", 3, 52);
			actionList[8] = new RaphagoActionTime("체 올리기", 4, 52);
		}
	}

	void update(float _currentMillis){
		if(bStarted){
			for(int i=0; i<9; i++){
				if(i!=8 && _currentMillis >= actionList[i].getMillisTime()  && _currentMillis < actionList[i+1].getMillisTime()){
					pushStyle();
					textFont(hangulFont);
                    textAlign(RIGHT, TOP);
                    textLeading(80);
					fill(0, 255, 255);
					text(actionList[i].getMessage(), 1400, 120);
					popStyle();
				}
			}
		}
	}

	void activate(){
		bStarted = true;
	}

	void deactivate(){
		bStarted = false;
	}
}