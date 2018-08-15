#include <dataStruct.h>

DataStruct::DataStruct(uint8_t address, uint8_t len){
	//create new Struct Here
	for (int i = address; i < len + address; i++){
		Segment;
		Segment.address = i;
		Segment.digit = 0;
		Segment.color = 0;
		if (head == NULL){
			head = Segment; 
		}
	}
}


DataStruct::getSize(){
	uint32_t size = 0;
	while(head.next != NULL){
		size += sizeof(head);
		head = head.next;
	}
}

//deconstructor;
//serial write;
//serial read;
