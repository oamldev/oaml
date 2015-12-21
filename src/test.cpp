#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oaml.h"

int main() {
	oamlData *oaml = new oamlData();

	oaml->Init("music/");
	oaml->Shutdown();

	return 0;
}
