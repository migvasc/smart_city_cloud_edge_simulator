import json
import sys


def create_requests_dict(json_data,tasks_dict):

    for task in json_data['tasks']:
        tasks_dict[task['name']] =  json_data['name']    


def process_requests(tasks_request_dict,log_file_path,output_folder):        
    
    tasks_file = open(f'{output_folder}/tasks.csv','w')

    tasks_file.write('id;start_time;finish_time;duration;host\n')
    first_task_req = {}

    comms_file = open(f'{output_folder}/comms.csv','w')
    comms_file.write('id;start_time;finish_time;duration;source_host;destination_host\n')

    req_file = open(f'{output_folder}/requests.csv','w')
    req_file.write('id;start_time;finish_time;duration;total_time_execs;total_time_comms;amount_comms\n')

    tasks_dict = tasks_request_dict

    task_schedule = {}

    jobs_file = open(f'{output_folder}/jobs.csv','w')

    jobs_file.write('job_id,workload_name,submission_time,requested_number_of_resources,requested_time,success,starting_time,execution_time,finish_time,waiting_time,turnaround_time,stretch,consumed_energy,allocated_resources\n')

    first_task_req ={}
    req_comm_time ={}
    req_exec_time={}
    req_comm_amount={}
    task_has_com= {}
    req_start_time = {}
    hosts_free_cores = {}
    with open(log_file_path, "r") as log_file:
        for (line) in enumerate(log_file):                    
            data_array = str(line[1]).split('#')
            if (len(data_array)>1):
                    data_to_process = data_array[1].split(';')
                    if(data_to_process[0]=='SCHEDULE'):
                        task_id = data_to_process[1]
                        core_alocated = int(data_to_process[2])
                        bus_stop_id = data_to_process[3].replace('\n','')

                        if bus_stop_id not in hosts_free_cores:
                            hosts_free_cores[bus_stop_id] = [1,2,3,4]                                                                                
                        task_schedule[task_id] = hosts_free_cores[bus_stop_id][0]
                        del hosts_free_cores[bus_stop_id][0]  
                        
                    if(data_to_process[0]=='FC'):                        
                        comm_id = data_to_process[1]                                                                                                
                        task_related_to_comm = comm_id.split('@')[1]
                        type_comm = comm_id.split('@')[0]
                        if type_comm == 'comm1':
                            task_has_com[task_related_to_comm] =True
                        if (task_related_to_comm in task_has_com) or type_comm=='p':
                            comm_start_time = float(data_to_process[2])
                            comm_completion_time = float(data_to_process[3])
                            comm_duration = round(comm_completion_time - comm_start_time,6)
                            comm_src_host = data_to_process[4]
                            comm_dest_host = data_to_process[5]
                            comms_file.write(f'{comm_id};{comm_start_time};{comm_completion_time};{comm_duration};{comm_src_host};{comm_dest_host}')                                                        
                            request_id = tasks_dict[task_related_to_comm]                        
                            if request_id not in req_comm_time:
                                print(request_id)
                                req_comm_time[request_id] = 0.0    
                                req_comm_amount[request_id] = 0.0

                            
                            req_comm_amount[request_id] = req_comm_amount[request_id] +1 
                            req_comm_time[request_id] = req_comm_time[request_id] + comm_duration

                    if(data_to_process[0]=='FE'):                    
                        task_id = data_to_process[1]
                        task_start_time = float(data_to_process[2])
                        task_completion_time = float(data_to_process[3])
                        duration = round(task_completion_time - task_start_time,6)
                        host = data_to_process[4].replace('\n','')
                        tasks_file.write(f'{task_id};{task_start_time};{task_completion_time};{duration};{host}\n')                                        
                        request_id = tasks_dict[task_id]
                        print(task_id)
                        req_id = task_id.split('-')[0]+'-'+task_id.split('-')[1]
                        if req_id not in first_task_req:
                            first_task_req[req_id] = task_id
                            req_start_time[req_id]  = task_start_time
                            
                        if request_id not in req_exec_time:
                            req_exec_time[request_id] = 0.0

                        req_exec_time[request_id] += duration                        
                        
                        bus_stop_number = int(host.replace('bus_stop_',''))
                        allocated_core =  (bus_stop_number * 4) + task_schedule[task_id]
                    # com id
                      #  jobs_file.write(f'{task_id},{request_id},{task_start_time},1,{task_start_time},1,{task_start_time},{duration},{task_completion_time},{0},{0},{0},-1,{allocated_core}\n')
                        hosts_free_cores[host].append(task_schedule[task_id])
                        jobs_file.write(f',{request_id},{task_start_time},1,{task_start_time},1,{task_start_time},{duration},{task_completion_time},{0},{0},{0},-1,{allocated_core}\n')


                    if(data_to_process[0]=='FR'):                                        
                        req_id = data_to_process[1]
                        completion_time = float(data_to_process[2])
                        start_time = req_start_time[req_id]
                        duration = round(completion_time-start_time,6)
                        print(duration)
                        total_time_execs = round(req_exec_time[req_id],4)
                        total_time_comms = 0
                        amount_comms = 0

                        if req_id in req_comm_time:
                            total_time_comms = round(req_comm_time[req_id],4)
                            amount_comms = req_comm_amount[req_id]
                        req_file.write(f'{req_id};{start_time};{completion_time};{duration};{total_time_execs};{total_time_comms};{amount_comms}\n')

    tasks_file.close()
    comms_file.close()
    req_file.close()
    jobs_file.close()

def process_data(input_file_path,log_file,output_folder):
    tasks_request_dict = {}

    with open(input_file_path,"r") as input_file:
        for (index,line) in enumerate(input_file):
            print(line)
            cols = line.split(' ')
            if len(cols) == 3:
                break

            request_file = cols[3].replace('\n','')

            with open(request_file) as f:
                json_data = json.load(f)
                create_requests_dict(json_data,tasks_request_dict)

    process_requests(tasks_request_dict,log_file_path,output_folder)



input_file_path = sys.argv[1]
log_file_path =  sys.argv[2]
output_folder = sys.argv[3]
process_data(input_file_path,log_file_path,output_folder)
