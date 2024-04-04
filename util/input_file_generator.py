path = '/home/msilvava/postdoc/data/sp_estimation_trips.csv'
from random import randint

data_dict = {}
total_trips = 0.0
with open(path, 'r') as csv_file:
    for index,line in enumerate (csv_file):
        if index > 0:            
            row = line.split(',')
            hour = float(row[0])
            value = float(row[1].replace('\n',''))
            total_trips += value
            data_dict[hour]=value

ratio_of_travels = {}
for hour in data_dict:
    ratio_of_travels[hour] = round(data_dict[hour]/total_trips,4)
print(ratio_of_travels)


workload_file =  open('/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/vieille-toulouse_week.txt','w')
cont = 0

for day in range(7):
    total_trips_to_generate = 30000
    instants_of_time = []
    for hour in ratio_of_travels:
        initial_time = hour * 3600  + (day * 24*60*60)
        tasks_to_generate = int(ratio_of_travels[hour]*total_trips_to_generate)+1

        for i in range(int(tasks_to_generate*.75)):
            instant_of_time = randint(0, 60) + initial_time
            instants_of_time.append(instant_of_time)
        for i in range(int(tasks_to_generate*.25)):
            instant_of_time = randint(0, 3600) + initial_time
            instants_of_time.append(instant_of_time)

        
    instants_of_time.sort()

    for instant_of_time in instants_of_time:
        workload_file.write(f"dagdispatcher send_DAG {instant_of_time} input/workload/requests/req_{cont}.json\n")
        cont+=1

workload_file.write("dagdispatcher stop 605100\n\n")
workload_file.close()
