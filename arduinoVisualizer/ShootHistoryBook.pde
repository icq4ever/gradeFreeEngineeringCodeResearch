class ShootHistoryBook{
	ArrayList<PVector> history;
    ArrayList<String> timeStamp;
    int sizeOfRecord = 0;

	ShootHistoryBook(){
		history = new ArrayList<PVector>();
        timeStamp = new ArrayList<String>();
		//count = new ArrayList<int>();
	}

	void pushRecord(PVector shootRecord, String _timeStamp){
		// PVector t;
		// t = shhotRecord.copy();
		history.add(shootRecord);
        timeStamp.add(_timeStamp);
        sizeOfRecord++;
	}

    int getSizeOfRecord(){    return sizeOfRecord; };
}