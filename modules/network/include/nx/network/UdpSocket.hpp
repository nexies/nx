//
// Created by nexie on 21.02.2026.
//

#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP

#include <nx/network/Socket.hpp>
#include <boost/asio/ip/udp.hpp>

namespace nx {
    class UdpSocket : public Socket {
    public:
        UdpSocket();
        UdpSocket(std::string addr, uint16_t port);


        Result open () override;
        void close () override;

        Result bind (const std::string & address,  const uint16_t port);

        void setDestination (const std::string & address, const uint16_t port);

        int write (const char * buf);
        int read (char * buf, int len);

    protected:
        int _readData(char *buf, int len) override;
        int _writeData(char *buf, int len) override;
    private:

        void _startReceive();

        boost::asio::ip::udp::socket udp;
        boost::asio::ip::udp::endpoint destination;

        char _buffer[4096];
        size_t ready_read_bytes { 0 };
    };
}

#endif //UDPSOCKET_HPP
