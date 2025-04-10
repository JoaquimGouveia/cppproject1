#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "connection.h"
#include "connectionclosedexception.h"
#include <string>
#include <memory>

class MessageHandler {
    private:
        std::shared_ptr<Connection> connection;

        void sendByte(int code);
        int recvByte();
    public:
        explicit MessageHandler(std::shared_ptr<Connection> conn);     
        void sendCode(int code);
        void sendInt(int value);
        void sendIntParameter(int param);
        void sendStringParameter(const std::string& param);
        int recvCode();
        int recvInt();
        int recvIntParameter();
        std::string recvStringParameter();
};

#endif // MESSAGEHANDLER_H