#ifndef MESSAGE_HPP
#define MESSAGE_HPP


/** Types of messages exchanged between two peers. */
enum e_message_type {
  MESSAGE_DAG_SUBMISSION,
  MESSAGE_VM_SUBMISSION,
  MESSAGE_VM_FINISHED,
  MESSAGE_UPDATE_ENERGY,
  MESSAGE_GET_WORKERS,
  MESSAGE_GET_ENERGY,
  MESSAGE_GET_GREEN_ENERGY,
  MESSAGE_GREEN_ENERGY_VALUE,
  MESSAGE_HOST_RESPONSE,
  MESSAGE_HOST_INFORMATION,
  MESSAGE_TURN_HOST_ON,
  MESSAGE_SHUTDOWN_HOST,
  MESSAGE_START_VM_EXECUTION,
  MESSAGE_START_VM_MIGRATION,
  MESSAGE_VM_MIGRATION_STARTED,
  MESSAGE_VM_MIGRATION_FINISHED,
  MESSAGE_VM_MIGRATION_ERROR,
  MESSAGE_GRID_GET_ENERGY_INFO
};

class Message{

public: 

    e_message_type type;
    void* data;
    
    Message(e_message_type type_, void* some_data){
        type = type_;
        data = some_data;
    }

    Message(e_message_type type_){
        type = type_;
        data = nullptr;
    }

};

#endif