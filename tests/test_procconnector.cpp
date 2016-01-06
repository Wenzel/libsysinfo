#include "procconnector.h"

int main(int argc, char* argv[])
{
    ProcConnector connector = ProcConnector();
    connector.listen();
    // wait for messages
    std::this_thread::sleep_for(std::chrono::milliseconds(9999999));
}
