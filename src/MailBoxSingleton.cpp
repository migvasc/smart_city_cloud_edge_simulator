#include "MailBoxSingleton.hpp"
#include "CommunicationInterface.hpp"
/**
 * Static methods should be defined outside the class.
 */

MailBoxSingleton* MailBoxSingleton::pinstance_{nullptr};
std::mutex MailBoxSingleton::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */
MailBoxSingleton *MailBoxSingleton::get_instance()
{
    
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        
        pinstance_ = new MailBoxSingleton();        
    }
    

    return pinstance_;

}

void MailBoxSingleton::send_message(std::string mailboxID,Message *message)
{
    try {           
        if(message_hash.find(mailboxID) == message_hash.end()){
            message_hash[mailboxID] = new std::queue<Message*>;
        }    
        message_hash[mailboxID]->push(message);
        
        if(message_obj.find(mailboxID)!= message_obj.end()){
            
            message_obj[mailboxID]->handle_message(message); 
            message_hash[mailboxID]->pop();

        }
    } catch (const std::exception& e) 
    {            
        cout << "Error : at mailbox singleton send message   "   << e.what() << endl;

    }	
}

Message *MailBoxSingleton::get_message(std::string id)
{
    Message * result = nullptr;
    
    if(!message_hash.empty() && message_hash.find(id) != message_hash.end() && !message_hash[id]->empty()){
        result = message_hash[id]->front();
        message_hash[id]->pop();
    }
    return result;
}

void MailBoxSingleton::register_object(std::string name,CommunicationInterface *object )
{
    message_obj[name] = object;
}