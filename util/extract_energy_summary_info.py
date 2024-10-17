import sys



def process_data(input_file, output_folder):
    energy_file = open(f'{output_folder}/summary_energy.csv','w')
    energy_file.write(f'total_energy,renewable_energy,brown_energy\n')
    total_energy=0
    total_renewable_power=0
    total_brown_energy=0
    with open(input_file, "r") as energy_file_data:
        for (index,line) in enumerate(energy_file_data):  
            print(line)                  
            data_to_process = line.split(',')
            time = data_to_process[0]
            host = data_to_process[1]
            energy_consumed = float(data_to_process[2])
            solar_energy = float(data_to_process[3])
            brown_energy = float(data_to_process[4])
            battery_LOE = float(data_to_process[5].replace('\n',''))
            total_energy+= energy_consumed
            total_renewable_power+=solar_energy
            total_brown_energy+=brown_energy

    energy_file.write(f'{total_energy},{total_renewable_power},{total_brown_energy}\n')
    energy_file.close()


input_file_path = sys.argv[1]
output_folder = sys.argv[2]
process_data(input_file_path,output_folder)

