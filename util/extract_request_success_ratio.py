import sys



def process_data(input_file, output_folder):
    requests_file = open(f'{output_folder}/summary_requests.csv','w')
    requests_file.write(f'total_requests,completed,failed,completed_ratio\n')
    total_requests=0
    completed = 0
    failed =0
    completed_ratio = 0
    with open(input_file, "r") as energy_file_data:
        for (index,line) in enumerate(energy_file_data):  
            #0#req-0,0.000000,1.435000,1

            data_to_process = line.split(',')
            total_requests+=1.0
            status = data_to_process[3].replace('\n','')

            if status == '1':
                completed+=1.0
            else:
                failed+=1.0
    completed_ratio = completed/total_requests
    requests_file.write(f'{total_requests},{completed},{failed},{completed_ratio}\n')
    requests_file.close()

input_file_path = sys.argv[1]
output_folder = sys.argv[2]
process_data(input_file_path,output_folder)

