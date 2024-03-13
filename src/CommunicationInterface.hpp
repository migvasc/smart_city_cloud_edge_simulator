#ifndef COMMUNICATIONINTERFACE_HPP
#define  COMMUNICATIONINTERFACE_HPP
#include <string>
#include "MailBoxSingleton.hpp"
#include "Message.hpp"
class CommunicationInterface
{
    public:
        virtual void handle_message(Message *message) = 0;
        virtual void register_object(std::string name,CommunicationInterface *object ){
            MailBoxSingleton::get_instance()->register_object(name,object);
        }
};
#endif