#include "../include/windowsmod.hpp"
#include <cstdlib>

using _vvf_t = void(*)(void);

extern void _windowsmod_reg_init(void);

_vvf_t init_functions[] = {
    _windowsmod_reg_init,
    NULL
};

void windowsmod_init(void) {
    for (_vvf_t *p = init_functions; true; p++) {
        if (*p == NULL)
            break;

        (*p)();
    }
}
