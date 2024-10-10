import xml.etree.ElementTree as ET
import math 
from pulp import *
import time
import argparse
import os
import json


def get_network_latency(x1,y1,z1,x2,y2,z2):
    return math.sqrt( pow(x1-x2,2) + pow( y1-y2,2))  + z1 + z2

def create_hosts_with_cores(hosts):
    all_hosts = []
    core_host = {}
    for i in range(len(hosts)):
        cores = int( hosts[i].attrib['core'])
        for core in range(cores):
            core_host_id = f"{hosts[i].attrib['id']}_{core+1}"
            all_hosts.append(core_host_id)
            core_host[core_host_id] = i
    return (all_hosts,core_host)


def create_latency_matrix(hosts_inside_zone):
    lat_matrix = []
    amount_of_hosts = len(hosts_inside_zone)
    for i in range(amount_of_hosts):
        column = []
        for j in range(amount_of_hosts):
                coords_i = hosts_inside_zone[i].attrib['coordinates'].split(' ')
                coords_j = hosts_inside_zone[j].attrib['coordinates'].split(' ')
                lat = get_network_latency(float(coords_i[0]),float(coords_i[1]),float(coords_i[2]),float(coords_j[0]),float(coords_j[1]),float(coords_j[2]))
                column.append(lat)                
        lat_matrix.append(column)
    return (lat_matrix)
    
tree = ET.parse('platform_latency_vieille-toulouse_vivaldi_cloud.xml')
root = tree.getroot()
print(root[0][0].attrib)
hosts_inside_zone = root[0]

latency_matrix = create_latency_matrix(hosts_inside_zone)
all_hosts,map_core_host = create_hosts_with_cores(hosts_inside_zone)

def get_latency(h1,h2,latency_matrix,map_core_host):
    return (latency_matrix[map_core_host[h1]][map_core_host[h2]])
    
value =get_latency(all_hosts[0],all_hosts[0],latency_matrix,map_core_host)


path = 'req_0.json'

def create_empty_matrix(size):
    matrix = []
    for i in range(size):
        columns = []
        for j in range(size):
            columns.append(0)
        matrix.append(columns)
    return (matrix)

def add_tasks(tasks):
    total_tasks = len(tasks)
    for i in range(total_tasks):
        tasks.append(f'{tasks[i]}_s')
        tasks.append(f'{tasks[i]}_d')        
        tasks.append(f'{tasks[i]}_r')        

def generate_dependency_matrix(tasks,matrix, parent_map, map_task_index):

    for task in tasks:
        task_index                           = map_task_index[task]          
        task_d_s_index                       = 3 * task_index + 3
        task_d_index                         = 3 * task_index + 4
        task_d_r_index                       = 3 * task_index + 5
        matrix[task_d_s_index][task_d_index] = 1
        matrix[task_d_index][task_d_r_index] = 1
        matrix[task_d_r_index][task_index]   = 1

        for parent in parent_map[task]:      
            parent_index = map_task_index[parent]
            matrix[parent_index][task_d_s_index] = 1

json_file = open(path, "r") 
data = json.loads(json_file.read())
map_task_index = {}
i = 0
tasks = []
tasks_parents = {}


tasks = []

task_pref_host = {}
for task in data['tasks']:
    map_task_index[task['name']] = i
    i = i+1
    tasks.append(task['name'])
    tasks_parents[task['name']] = task['parents']
    task_pref_host[task['name']] = task['machine']

total_tasks = len(tasks) * 4

dep_matrix = create_empty_matrix(total_tasks)
generate_dependency_matrix(tasks,dep_matrix,tasks_parents,map_task_index)
add_tasks(tasks)


print(tasks)



# Tasks 
t = tasks

# a -> b 
# b -> c
total_tasks = int(len(tasks)/4 )
dag =   dep_matrix

# t_0 = a, t_1 = b, t_2 = c
t_index  = map_task_index

p = 1

s = 1

hosts = all_hosts

hosts_capacity = 1

lat = latency_matrix


local_data = task_pref_host
print('Vai comecar o milp ! ')
# Variables
x = LpVariable.dicts("x",(hosts,t), 0,    cat='Integer') # Task allocation
print('criou o X')
z = LpVariable.dicts("z",(hosts,hosts,t,t), 0,    cat='Continuous') # Task allocation between two hosts 
print('criou o z')
C = LpVariable.dicts("C",t, 0,    cat='Continuous') # Job completion time
Z  = LpVariable("Z", 0, cat='Continuous')

prob = LpProblem("Teste", LpMinimize)

# Constraints

for j in t:
    prob += lpSum( [ x[i][j]  for i in hosts ] )  ==1 
    prob +=  p  * lpSum( [x[i][j]    for i in hosts ]  ) <= C[j] 

print('add as constraints iniciais de escalonamento')

for i in hosts:
    prob +=  lpSum( [ x[i][j]*p/s for j in t ]  ) <= Z

print('add as constraints de velocidade e tempo ')
for j in range(0,total_tasks):    
    task_d_s_index   = 3*j +3
    task_d_s         = t[task_d_s_index]
    task_d_index     = 3*j +4
    task_d = t[task_d_index]
    task_d_r_index   = 3*j +5
    task_d_r = t[task_d_r_index]
    task = t[j]
    host_with_data = local_data[task_d]

    prob += x[host_with_data][task_d] ==1
    
    for i in hosts:
         prob += x[i][task_d_s] == x[i][task]
         prob += x[i][task_d_r] == x[i][task]


print('add as constraints de tarefa de comm')

for i in range(len(dag)):
    for j in range(len(dag)):
        if(dag[i][j] == 1):
            for h in hosts:
                for k in hosts:
                    if h != k:
                        prob +=  x[h][t[i]] >= z[h][k][t[i]][t[j]]
                        prob +=  x[k][t[j]] >= z[h][k][t[i]][t[j]]
                        prob +=  z[h][k][t[i]][t[j]] >= x[h][t[i]] + x[k][t[j]] - 1

print('add as constraints procedencia')
# Procedencia, tempo de completude
for i in range(len(dag)):
    for j in range(len(dag)):
        if(dag[i][j] == 1):
            for h in hosts:
                for k in hosts:
                        prob +=  C[t[i]] + p  *  x[k][t[j]] + get_latency(h,k,latency_matrix,map_core_host) * z[h][k][t[i]][t[j]]   <= C[t[j]]


print('add as constraints de latencia')


for j in t:
    prob.addConstraint( Z >= C[j]) # Para minimizar o tempo de execução

prob += Z

print('VAI RESOLVER')
prob.solve()

# The status of the solution is printed to the screen
print("Status:", LpStatus[prob.status])

# Each of the variables is printed with it's resolved optimum value
for v in prob.variables():
    print(v.name, "=", v.varValue)

