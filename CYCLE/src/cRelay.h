
#ifndef cRelay_h
#define cRelay_h

#include <outputExtend.h>


class CRelay{
		public:
				CRelay();
				void update(uint8_t);
				void test(uint8_t, uint8_t);
				void loop();
				int8_t currDevice;
		private:
				outputExtend* oe;
				bool action;
				uint32_t resetTimer = 0;
				uint32_t static const resetTime = 5000;
				uint8_t audio;
				uint8_t optoCopter;

};
#endif
