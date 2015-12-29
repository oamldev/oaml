#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>

#include "oaml.h"

int main() {
	oamlApi *oaml = new oamlApi();

	oaml->Init("music/");
	oaml->Shutdown();

	return 0;
}
