#include "AutoDoSth.h"

AutoDoSth::AutoDoSth(std::function<void(void)> fun){
	this->fun = fun;
}

AutoDoSth::~AutoDoSth() {
	try {
		fun();
	}
	catch (...) {

	}

}
