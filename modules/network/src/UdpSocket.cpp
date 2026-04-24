//
// Created by nexie on 21.02.2026.
//

#include <nx/core2/detail/logger_defs.hpp>
#include <nx/network/UdpSocket.hpp>
#include "nx/core/Thread.hpp"

using namespace nx;

UdpSocket::UdpSocket() :
    Socket(),
    udp(thread()->context())
{

}

UdpSocket::UdpSocket(std::string addr, uint16_t port) :
    UdpSocket()
{
    bind(addr, port);
}

Result UdpSocket::open() {
    boost::system::error_code ec;
    udp.open(boost::asio::ip::udp::v4(), ec);

    if (ec)
        return Result::Err(ec.what());

    return Result::Ok();
}

void UdpSocket::close() {
    udp.close();
}

Result UdpSocket::bind(const std::string & address, const uint16_t port) {
    std::string address_string = address;

    if (address == "localhost") {
        address_string = "127.0.0.1";
    }

    boost::asio::ip::address addr =
        boost::asio::ip::make_address(address);

    boost::asio::ip::udp::endpoint endpoint(addr, port);

    boost::system::error_code ec;

    ec = udp.bind(endpoint, ec);

    if (ec) {
        nxDebug("UdpSocket::bind error: %s", ec.message().c_str());
        return Result::Err(ec.what());
    }

    _startReceive();

    return Result::Ok();
}

void UdpSocket::setDestination(const std::string &address, const uint16_t port) {
    destination = {boost::asio::ip::make_address(address), port};
}

int UdpSocket::write(const char *buf) {
    return _writeData((char*)buf, std::strlen(buf));
}

int UdpSocket::read(char *buf, int len) {
    return _readData((char*)buf, len);
}

int UdpSocket::_readData(char *buf, int len) {
    auto read_bytes = std::min((std::size_t)len, ready_read_bytes);

    memcpy(buf, _buffer, read_bytes);
    return read_bytes;
}

int UdpSocket::_writeData(char *buf, int len) {

    udp.async_send_to(boost::asio::buffer(buf, len),
        destination,
        [this](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                nxDebug("Sent {} bytes", bytes_transferred);
                NX_EMIT(bytesWritten, bytes_transferred);
            } else {
                nxDebug("Error: {}", ec.message().c_str());
            }
        });

    return 0;
}

void UdpSocket::_startReceive() {
    udp.async_receive(boost::asio::buffer(_buffer, 4096),
    [this](boost::system::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            nxTrace("Received {} bytes", bytes_transferred);

            NX_EMIT(this->readyRead)
            NX_EMIT(this->readyReadBytes, bytes_transferred)
            ready_read_bytes = bytes_transferred;

            _startReceive();
        }
        else {
            nxDebug("Receive error: {}", ec.message().c_str());
            NX_EMIT(this->socketError, Result::Err(ec.message()));
        }
    });
}

// bool UdpSocket::bind(std::string_view addr, uint16_t port) {
//     boost::asio::ip::address address =
//         boost::asio::ip::make_address(addr);
//
//     boost::asio::ip::udp::endpoint endpoint(address, port);
//
//     boost::system::error_code ec;
//
//     ec = udp.bind(endpoint, ec);
//
//     if (ec) {
//         nxDebug("UdpSocket::bind() failed: %s", ec.message().c_str());
//         udp.close();
//         return false;
//     }
//
//     return true;
// }

