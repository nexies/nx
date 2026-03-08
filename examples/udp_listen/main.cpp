//
// Created by nexie on 22.02.2026.
//

#include <nx/network/UdpSocket.hpp>
#include <nx/core.hpp>

class SocketReader : public nx::Object {

    nx::UdpSocket * sock;

public:
    SocketReader(nx::UdpSocket * sock) : sock(sock) {
        nx::connect(sock, &nx::UdpSocket::readyReadBytes, this, &SocketReader::onReadyRead);
    }

    void onReadyRead(uint64_t bytes) {
        char buf [1000];
        int read = sock->read(buf, sizeof(buf));

        nxDebug("read {} bytes", read);
        nxDebug("{}", buf);
    }
};

int main (int argc, char * argv[]) {
    nx::App::Init(argc, argv);

    nx::Result res = nx::Result::Ok();

    nx::UdpSocket socket;
    res = socket.open();
    if (!res) {
        nxError("Error: {}", res.get_err().str());
        return -1;
    }
    nxDebug("Socket opened for listen");

    res = socket.bind("127.0.0.1", 45678);
    if (!res) {
        nxError("Error: {}", res.get_err().str());
        return -1;
    }
    nxDebug("Socket bind on localhost port 45678");

    socket.setDestination("127.0.0.1", 45678);
    socket.write("Hello world!");

    return nx::App::Exec();
}