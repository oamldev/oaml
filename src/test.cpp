#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oaml.h"

int main() {
	oamlInit("music/");
	oamlShutdown();

	return 0;
}
