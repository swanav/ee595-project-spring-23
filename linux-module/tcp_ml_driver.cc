#include <iostream>
#include <cstdlib>

using namespace std;

static const char *kModuleName = "tcp_ml";

static void unload_kernel_module() {
    if (system("sudo rmmod tcp_ml")) {
        cout << "Failed to unload the kernel module" << endl;
        exit(1);
    }
}

static void load_kernel_module() {
    int ret = 0;

    if (ret = system("kmodsign sha512 /home/swanav/MOK.priv /home/swanav/MOK.der tcp_ml.ko")) {
        cout << "Failed to sign the kernel module " << ret << endl;
        exit(1);
    }

    if (ret = system("sudo insmod tcp_ml.ko")) {
        cout << "Failed to load the kernel module " << ret << endl;
        unload_kernel_module();
        load_kernel_module();
    }
}

static void activate_tcp_ml() {
    if (system("sudo sysctl -w net.ipv4.tcp_congestion_control=tcp_ml")) {
        cout << "Failed to activate TCP ML" << endl;
        exit(1);
    }
}

static void deactivate_tcp_ml() {
    if (system("sudo sysctl -w net.ipv4.tcp_congestion_control=cubic")) {
        cout << "Failed to deactivate TCP ML" << endl;
        exit(1);
    }
}


// Make a curl request to the alice.txt file
static void make_curl_request() {
    system("curl -o /dev/null -s -w %{time_total} https://gaia.cs.umass.edu/wireshark-labs/alice.txt");
}



int main() {
    load_kernel_module();
    activate_tcp_ml();
    make_curl_request(); // Do something
    deactivate_tcp_ml();
    unload_kernel_module();
    return 0;
}