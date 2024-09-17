#include "bar.h"
#include "foo.h"
#include <stdio.h>

void bar() {
    printf("bar enter\n");
    foo();
    printf("bar exit\n");
}
