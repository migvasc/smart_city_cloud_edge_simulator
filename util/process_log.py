import json
import sys

# Auxiliary function to create a dictionary from the json input file
# the key is the task name, and the value is the request name
def create_requests_dict(json_data,tasks_dict):
    tasks_dict[f"{json_data['name']}-ini"] = json_data['name']   
    tasks_dict[f"{json_data['name']}-end"] = json_data['name']   
    for task in json_data['tasks']:
        tasks_dict[task['name']] =  json_data['name']    
    
dict_task_finished_count = {}

def process_requests(tasks_request_dict,log_file_path,output_folder,request_submission_time_dict): 
    tasks_dict = tasks_request_dict
    task_schedule = {}
    first_task_req ={}
    req_comm_time ={}
    req_exec_time={}
    req_comm_amount={}
    task_has_com= {}
    req_start_time = {}
    hosts_free_cores = {}
    first_task_req = {}

    #The output files

    # File with data of the tasks: id, start time, completion time, duration of the task, and where it was executed
    tasks_file = open(f'{output_folder}/tasks.csv','w')    
    tasks_file.write('id;start_time;finish_time;duration;host\n')
    
    # File with data of the communications that the tasks executed: id, start time, completion time, duration of the communication 
    # and the hosts involved in the communication (the sender and the receiver)
    comms_file = open(f'{output_folder}/comms.csv','w')
    comms_file.write('id;start_time;finish_time;duration;source_host;destination_host\n')

    # File with data of the requests : id, start time, completion time, duration of the request (since it started execution until it finishes),
    # total time running computations, total time communicating data, and the amount of communications performed 
    req_file = open(f'{output_folder}/requests.csv','w')
    req_file.write('id;submission_time;start_time;finish_time;duration;total_time_execs;total_time_comms;amount_comms\n')

    # File for generating the gantt chart
    jobs_file = open(f'{output_folder}/jobs.csv','w')
    jobs_file.write('job_id,workload_name,submission_time,requested_number_of_resources,requested_time,success,starting_time,execution_time,finish_time,waiting_time,turnaround_time,stretch,consumed_energy,allocated_resources\n')


    timeslot_energy_file = open(f'{output_folder}/timeslot_energy.csv','w')
    timeslot_energy_file.write('time,host,energy_consumed,solar_energy_produced,brown_energy_consumed,battery_LOE\n') 
   
    energy_file = open(f'{output_folder}/energy.csv','w')
    energy_file.write(f'total_energy,renewable_energy,brown_energy\n')




    total_energy = 0.0
    total_renewable_power = 0.0
    total_brown_energy = 0.0
    
    with open(log_file_path, "r") as log_file:
        for (line) in enumerate(log_file):                    
            data_array = str(line[1]).split('#')
            if (len(data_array)>1):
                    data_to_process = data_array[1].split(';')
                    
                    ## Here we have information of the energy consumption and production of the hosts
                    if(data_to_process[0]=='ENERGY'):                        
                        #timeslot_energy_file.write('time,host,energy_consumed,solar_energy_produced,brown_energy_consumed,battery_LOE\n')         
                        time = data_to_process[1]
                        host = data_to_process[2]
                        energy_consumed = float(data_to_process[3])
                        solar_energy = float(data_to_process[4])
                        brown_energy = float(data_to_process[5])
                        #if brown_energy > 0:
                            #print(data_to_process)
                        battery_LOE = float(data_to_process[6].replace('\n',''))
                        timeslot_energy_file.write(f'{time},{host},{energy_consumed},{solar_energy},{brown_energy},{battery_LOE}\n')                                                

                        total_energy+= energy_consumed
                        total_renewable_power+=solar_energy
                        total_brown_energy+=brown_energy
                    
                    ## Here we have information of where the tasks were scheduled
                    if(data_to_process[0]=='SCHEDULE'):      
                        task_id = data_to_process[1]
                        task_type = task_id.split('-')
                        if ( task_type[2]!='ini' and task_type[2]!='end') :
                            core_alocated = int(data_to_process[2])
                            bus_stop_id = data_to_process[3].replace('\n','')

                            if bus_stop_id not in hosts_free_cores:
                                if bus_stop_id == 'cloud_cluster':
                                    hosts_free_cores[bus_stop_id] = [ j for j in range(1,257)]   
                                elif bus_stop_id == 'edge_cluster':
                                    hosts_free_cores[bus_stop_id] = [ j for j in range(1,129)]   
                                else:
                                    hosts_free_cores[bus_stop_id] = [1,2,3,4]  
                            #print('#ANTES NO SCHEDULE',task_id,bus_stop_id,hosts_free_cores[bus_stop_id])
                            task_schedule[task_id] = hosts_free_cores[bus_stop_id][0]                            
                            del hosts_free_cores[bus_stop_id][0]  
                           # print('#DPS NO SCHEDULE',task_id,bus_stop_id,hosts_free_cores[bus_stop_id])
                            if(bus_stop_id == 'bus_stop_15'):
                                dict_task_finished_count[task_id] = 1
                    
                    ## Here we have information of the communications performed during the simulation
                    if(data_to_process[0]=='FC'):                        

                        comm_id = data_to_process[1]                                                                                                
                        task_related_to_comm = comm_id.split('@')[1]
                        type_comm = comm_id.split('@')[0]
                        
                        if type_comm == 'comm1':
                            task_has_com[task_related_to_comm] = True

                        if (task_related_to_comm in task_has_com) or type_comm=='p':
                            comm_start_time = float(data_to_process[2])
                            comm_completion_time = float(data_to_process[3])
                            comm_duration = round(comm_completion_time - comm_start_time,6)
                            comm_src_host = data_to_process[4]
                            comm_dest_host = data_to_process[5]
                            comms_file.write(f'{comm_id};{comm_start_time};{comm_completion_time};{comm_duration};{comm_src_host};{comm_dest_host}')                                                        
                            request_id = tasks_dict[task_related_to_comm]                        

                            if request_id not in req_comm_time:                        
                                req_comm_time[request_id] = 0.0    
                                req_comm_amount[request_id] = 0.0
                            
                            req_comm_amount[request_id] = req_comm_amount[request_id] +1 
                            req_comm_time[request_id] = req_comm_time[request_id] + comm_duration
                            if request_id not in first_task_req:
                                first_task_req[request_id] = task_related_to_comm
                                req_start_time[request_id]  = comm_start_time

                    ## Here we have information of the tasks executed during the simulation
                    if(data_to_process[0]=='FE'):                    
                        #print(data_to_process)
                        task_id = data_to_process[1]
                        task_type = task_id.split('-')
                        #print(task_type)
                        if ( task_type[2]!='ini' and task_type[2]!='end') :
                            task_start_time = float(data_to_process[2])
                            task_completion_time = float(data_to_process[3])
                            if task_completion_time == -1: 
                                continue
                            duration = round(task_completion_time - task_start_time,6)

                            host = data_to_process[4].replace('\n','')
                            tasks_file.write(f'{task_id};{task_start_time};{task_completion_time};{duration};{host}\n')                                        
                            request_id = tasks_dict[task_id]
                            req_id = task_id.split('-')[0]+'-'+task_id.split('-')[1]
                            if req_id not in first_task_req:
                                first_task_req[req_id] = task_id
                                req_start_time[req_id]  = task_start_time
                                
                            if request_id not in req_exec_time:
                                req_exec_time[request_id] = 0.0

                            req_exec_time[request_id] += duration                        
                            if host == 'cloud_cluster':
                                allocated_core = 69 + task_schedule[task_id]
                            elif host == 'edge_cluster':
                                allocated_core = 500 + task_schedule[task_id]

                            else: 
                                bus_stop_number = int(host.replace('bus_stop_',''))
                                allocated_core =  (bus_stop_number * 4) + task_schedule[task_id]
                            
                            #print('#ANTES NO FE',task_id,host,hosts_free_cores[host])
                            hosts_free_cores[host].append(task_schedule[task_id])
                           # print('#DPS NO FE',task_id,host,hosts_free_cores[host])
                            if (host == 'bus_stop_15'):
                                dict_task_finished_count[task_id]= 0
                                #for key in dict_task_finished_count:
                                    #if (dict_task_finished_count[key]==1):
                                    #    print('OPAA',key,'bus_stop_15',data_to_process)
                            jobs_file.write(f',{request_id},{task_start_time},1,{task_start_time},1,{task_start_time},{duration},{task_completion_time},{0},{0},{0},-1,{allocated_core}\n')
                    
                    ## Here we have information of the requests executed during the simulation
                    if(data_to_process[0]=='FR'):                                        
                        req_id = data_to_process[1]
                        completion_time = float(data_to_process[2])
                        start_time = 0.0
                        if req_id in req_start_time:
                            start_time = req_start_time[req_id]
                        else :
                            start_time = float(request_submission_time_dict[req_id])
                        duration = round(completion_time-start_time,6)
                        total_time_execs = 0
                        if req_id in req_exec_time:
                            total_time_execs = round(req_exec_time[req_id],4)
                        total_time_comms = 0
                        amount_comms = 0

                        if req_id in req_comm_time:
                            total_time_comms = round(req_comm_time[req_id],4)
                            amount_comms = req_comm_amount[req_id]
                        req_file.write(f'{req_id};{request_submission_time_dict[req_id]};{start_time};{completion_time};{duration};{total_time_execs};{total_time_comms};{amount_comms}\n')


    energy_file.write(f'{total_energy},{total_renewable_power},{total_brown_energy}\n')

    tasks_file.close()
    comms_file.close()
    req_file.close()
    jobs_file.close()
    timeslot_energy_file.close()
    energy_file.close()


# Create a hashmap/dictionary with information of renewable energy availability for each host over time
# Key : Host, time = value
def create_hosts_green_energy_ratio_dict(time_slot_energy_file_path):
    hosts_green_dict = {}

    
    with open(time_slot_energy_file_path) as time_slot_energy_file:
        for index,line in enumerate(time_slot_energy_file):
            if index == 0:
                continue
            cols = line.split(',')
            time = round(float(cols[0]),1)
            host = cols[1]
            energy = float(cols[2])
            brown_energy = float(cols[4])
            
            if host not in hosts_green_dict:
                hosts_green_dict[host] = {}
            
            if energy > 0:
                hosts_green_dict[host][time] = 1 - (brown_energy/energy)
            else:
                hosts_green_dict[host][time] = 0
    return hosts_green_dict

# Extracts information about requests energy and brown energy consumption
def process_req_energy(output_folder): 
    
    time_slot_energy_file_path = f'{output_folder}/timeslot_energy.csv'
    tasks_input_file_path = f'{output_folder}/tasks.csv'
    req_energy = {}
    req_green_energy = {}

    hosts_green_ratio = create_hosts_green_energy_ratio_dict(time_slot_energy_file_path)
    
    with open(tasks_input_file_path) as tasks_input_file:
        for index,line in enumerate(tasks_input_file):
            if index == 0:
                continue
            cols = line.split(';')
            req_id = cols[0].split('-')[1]
            completion_timeslot =   float(int ( float(cols[2])/60)*60)
            host = cols[4].replace('\n','')
            if req_id not in req_energy:
                req_energy[req_id] = 0.0
                req_green_energy[req_id] = 0.0

            if host == 'cloud_cluster':
                energy = 0.05  * 2.21875
                req_energy[req_id] +=energy
                req_green_energy[req_id] +=energy
            else:
                energy = 0.1 * 1.2
                req_energy[req_id] +=energy
                req_green_energy[req_id] +=energy * hosts_green_ratio[host][completion_timeslot]

    result_file = open(f'{output_folder}/req_energy.csv','w')
    
    result_file.write('req_id,total_energy,green_energy\n')
    for req in req_energy:
        result_file.write(f'req_{req_id},{round(req_energy[req],4)},{round(req_green_energy[req],4)}\n')
    result_file.close()
    

# Main method for processing the data
def process_data(input_file_path,log_file,output_folder):
    tasks_request_dict = {}
    request_submission_time_dict = {}
    with open(input_file_path,"r") as input_file:
        for (index,line) in enumerate(input_file):
            cols = line.split(' ')
            if len(cols) == 3:
                break

            request_file = cols[3].replace('\n','')
            request_submission_time = cols[2]
            with open(request_file) as f:
                json_data = json.load(f)                 
                request_submission_time_dict[json_data['name'] ] = request_submission_time
                create_requests_dict(json_data,tasks_request_dict)

    process_requests(tasks_request_dict,log_file_path,output_folder,request_submission_time_dict)
   # process_req_energy(output_folder)

input_file_path = sys.argv[1]
log_file_path =  sys.argv[2]
output_folder = sys.argv[3]
print('PROCESSING LOG',input_file_path,log_file_path,output_folder)
process_data(input_file_path,log_file_path,output_folder)

