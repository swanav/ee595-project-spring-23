#include <iostream>

using namespace std;

static void load_kernel_module() {
    system("sudo insmod ./build/module/tcp_ml.ko");
}

static void unload_kernel_module() {
    system("sudo rmmod tcp_ml");
}

int main(int argc, const char** argv) {
    cout << "Hello, world!" << endl;
    cout << "argc: " << argc << endl;
    for (int i = 0; i < argc; i++) {
        cout << "argv[" << i << "]: " << argv[i] << endl;
    }
    return 0;
}
