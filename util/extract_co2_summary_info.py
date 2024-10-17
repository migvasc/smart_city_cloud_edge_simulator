import sys



def process_data(input_file, output_folder):
    co2_file = open(f'{output_folder}/summary_co2.csv','w')
    co2_file.write(f'total_co2,total_grid_co2,total_bat_co2,total_pv_co2\n')
    total_co2=0
    total_grid_co2=0
    total_bat_co2=0
    total_pv_co2=0
    with open(input_file, "r") as co2_file_data:
        for (index,line) in enumerate(co2_file_data):  
          #  print(line)                  
            data_to_process = line.split(';')
            time = data_to_process[0]
            host = data_to_process[1]
            total_co2_timeslot = float(data_to_process[2])
            grid_co2 = float(data_to_process[3])
            bat_co2 = float(data_to_process[4])
            pv_co2 = float(data_to_process[5])
            total_co2+=total_co2_timeslot
            total_grid_co2+=grid_co2
            total_bat_co2+=bat_co2
            total_pv_co2+=pv_co2

    co2_file.write(f'{total_co2},{total_grid_co2},{total_bat_co2},{total_pv_co2}\n')
    co2_file.close()


input_file_path = sys.argv[1]
output_folder = sys.argv[2]
process_data(input_file_path,output_folder)

