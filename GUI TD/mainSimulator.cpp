#include "Simulator.h"

int mainSimulator(int argc, char** argv) {
	if (argc < 1) {
		printf("Missing one argument: property path");
		return -1;
	}
	Simulator sim (argv[1]);
	sim.launchSim();
	return 0;
}