


def build_request_hashmap(input_file):
    result = {}
    data_to_process = []

    with open(input_file, "r") as log_file:
        for (line) in enumerate(log_file):                    
            data_array = str(line[1]).split('#')
            if (len(data_array)>1):
                data_to_process = data_array[1].split(';')
                if(data_to_process[0]=='FR'):                                        
                    req_id = data_to_process[1]
                    completion_time = float(data_to_process[2])
                    result[req_id] = completion_time

    return result


input_file_cache = '/home/msilvava/postdoc/smartcity_edge_simulator/output/output_baseline_vieille-toulouse_vivaldi_cache_map/logfile.log'
input_file_no_cache = '/home/msilvava/postdoc/smartcity_edge_simulator/output/output_baseline_vieille-toulouse_vivaldi_no_cache_map/logfile.log'


requests_cache = build_request_hashmap(input_file_cache)

requests_no_cache = build_request_hashmap(input_file_no_cache)


for req in requests_no_cache:
    if req not in requests_cache:
        print(req)

