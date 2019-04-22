
/*
 *
 *	Author :: Charles Stevenson
 *		CPP struct memory test
 *
 */


#ifndef dataStruct.h
#define dataStruct.h


struct Segment{
	uint8_t address;
	uint8_t digit;
	uint8_t color;
	Segment* next;
}


class DataStruct{
	public:
			DataStruct();

	private:
		Segment* head;
}

#endif
