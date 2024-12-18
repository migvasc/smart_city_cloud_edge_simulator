
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

enum e_message_type {
  MESSAGE_DAG_SUBMISSION,
  MESSAGE_TASK_SUBMISSION,  
  MESSAGE_TASK_COMPLETED,
  MESSAGE_UPDATE_ENERGY,
  MESSAGE_END_SIMULATION
};

class Message{

public: 
    e_message_type type;
    void* data;
    
    Message(e_message_type type_, void* someData){
        type = type_;
        data = someData;
    }

    Message(e_message_type type_){
        type = type_;
        data = nullptr;
    }

};

#endif