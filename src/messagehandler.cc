#include "messagehandler.h"
#include "connection.h"
#include "protocol.h"

#include "connectionclosedexception.h"
#include <string>
#include <iostream>

using namespace std;

MessageHandler::MessageHandler(std::shared_ptr<Connection> conn) 
        : connection(std::move(conn)){}

void MessageHandler::sendByte(int code) {
    connection->write(static_cast<unsigned char>(code));
}

int MessageHandler::recvByte() {
    int code = static_cast<int>(connection->read());
    std::cout << "Received byte: " << code << std::endl;
    return code;
}

void MessageHandler::sendCode(int code) {
    sendByte(code);
}

void MessageHandler::sendInt(int value) {
    sendByte((value >> 24) & 0xFF);
    sendByte((value >> 16) & 0xFF);
    sendByte((value >> 8) & 0xFF);
    sendByte(value & 0xFF);
}

void MessageHandler::sendIntParameter(int param) {
    sendCode(static_cast<int>(Protocol::PAR_NUM));
    sendInt(param);
}

void MessageHandler::sendStringParameter(const std::string& param) {
    sendCode(static_cast<int>(Protocol::PAR_STRING));
    sendInt(param.length());
    for (char ch : param ) {
        sendByte(static_cast<int>(ch));
    }
}

int MessageHandler::recvCode() {
    return recvByte();
}

int MessageHandler::recvInt() {
    int byte_1 = recvByte();
    int byte_2 = recvByte();
    int byte_3 = recvByte();
    int byte_4 = recvByte();

    return (byte_1 << 24) | (byte_2 << 16) | (byte_3 << 8) | byte_4;
}

int MessageHandler::recvIntParameter() {
    int code = recvCode();
    if (code != static_cast<int>(Protocol::PAR_NUM)) {
        // Look into creating new exceptions
        throw ConnectionClosedException();
    }
    return recvInt();
}

string MessageHandler:: recvStringParameter() {
    int code = recvCode();
    if (code != static_cast<int>(Protocol::PAR_STRING)) {
        // Look into creating new exceptions
        std::cout << "Code: " << code << std::endl;
        throw ConnectionClosedException();
    }
    int length = recvInt();
    string str;
    for (int i = 0; i < length; ++i) {
        str += static_cast<char>(recvByte());
    }
    return str;
}

