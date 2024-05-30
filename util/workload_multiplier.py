import json
import os

index = 0
def multiply(files):
    new_index = 0
    for day in range(0,10):
        print(day)
        
        for file in files:
            json_data = {}

            with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests_v2_toulouse/{file}") as f:
                json_data = json.load(f)                 

            json_data['name'] = f'req-{new_index}'
            for task in json_data['tasks']:
                id = task['name'].split('-')
                task['name'] = f'req-{new_index}-{id[2]}'
                new_parents = []
                #print(task['parents'])
                for parent in task['parents']:
                    id = parent.split('-')[2]
                    new_parents.append(f'req-{new_index}-{id}')
                task['parents'] = new_parents
                #print(task['parents'])

            json_object = json.dumps(json_data, indent=4)

            with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests_v2_toulouse_1day/req_{new_index}.json", "w") as outfile:
                outfile.write(json_object)
            new_index+=1
            print(new_index)


files = os.listdir('/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests_v2_toulouse/')
print(files)
multiply(files)