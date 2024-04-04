import json
import sys


index = 0


for day in range(1,7):
    print(day)
    
    new_index = 30000*day
    for index in range(0,30000):
        json_data = {}

        with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests/req_{index}.json") as f:
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

        with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests/req_{new_index}.json", "w") as outfile:
            outfile.write(json_object)
        new_index+=1
