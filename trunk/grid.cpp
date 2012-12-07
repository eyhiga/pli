#include "grid.hpp"

int hostMaisLento(Grid *grid) {
	int retorno = 0, i;
	if (grid->m < 0)
		return -1;
	for (i=1;i<grid->m;i++) {
		if (grid->TI[i] > grid->TI[retorno])
			retorno = i;
	}
	return retorno;
}
