#include "tbb/task_scheduler_init.h"

int main(int argc, char* argv[]) {
    tbb::task_scheduler_init init;
    return 0;
}
