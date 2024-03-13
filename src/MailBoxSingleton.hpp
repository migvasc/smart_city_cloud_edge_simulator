#include "Message.hpp"
#include "robin_hood.h"
#include <mutex>
#include <queue> 
#include<iostream>  
#include <stdlib.h>

using namespace std;
#ifndef MAILBOXSINGLETON_HPP
#define MAILBOXSINGLETON_HPP
/**
 * The Singleton class defines the `GetInstance` method that serves as an
 * alternative to constructor and lets clients access the same instance of this
 * class over and over.
 */
class CommunicationInterface;
class MailBoxSingleton
{

    /**
     * The Singleton's constructor/destructor should always be private to
     * prevent direct construction/desctruction calls with the `new`/`delete`
     * operator.
     */
private:
    static MailBoxSingleton * pinstance_;
    static std::mutex mutex_;
    robin_hood::unordered_map<std::string,std::queue<Message*> * > message_hash;
    robin_hood::unordered_map<std::string,CommunicationInterface * > message_obj;
    
protected:
    MailBoxSingleton()
    {
    }
    ~MailBoxSingleton() {}
    

public:
    /**
     * Singletons should not be cloneable.
     */
    MailBoxSingleton(MailBoxSingleton &other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const MailBoxSingleton &) = delete;
    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static MailBoxSingleton *get_instance();
    /**
     * Finally, any singleton should define some business logic, which can be
     * executed on its instance.
     */
    void send_message(std::string mailboxID,Message *message);
    Message *get_message(std::string id);
    void register_object(std::string name,CommunicationInterface *object );

    
};



#endif