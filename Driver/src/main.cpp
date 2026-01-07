#include "main.h"

Main Main::getInstance() {
	static Main singleton;
	return singleton;
}

void main() {
	while (true) {
		// run all update checks from client etc.
	}
}