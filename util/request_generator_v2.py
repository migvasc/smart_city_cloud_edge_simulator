import osmnx as ox
import pandas as pd
import networkx as nx
import math
import json
import sys
from random import randint


### Compute the distance between two nodes
def distance(p1, p2):
  return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)



def create_clusters_df(street_graph, computing_infra_csv_path):
    col_names = names=["id", "long", "lat"]
    df_computing_infra = pd.read_csv(computing_infra_csv_path,names = col_names)
    max = int(len(street_graph.nodes)/len(df_computing_infra))+1
    
    streets_nearest_busstop_map = {}
    clusters = {}

    for i in range(len(df_computing_infra)):
        clusters[i] = []
        
    for node in street_graph:
        min_dist = 9999999.0
        current_best_cluster = 0
        coord_street = [ street_graph.nodes[node]['x'],street_graph.nodes[node]['y']]
        for i in df_computing_infra.index:
            coord_computing_node = [df_computing_infra['long'][i],df_computing_infra['lat'][i]]
            dist = distance(coord_street, coord_computing_node) 
            if dist < min_dist and len(clusters[i]) < max:
                min_dist = dist
                current_best_cluster = i
        clusters[current_best_cluster].append(node)
        streets_nearest_busstop_map[node] = f'bus_stop_{current_best_cluster}'
    return (streets_nearest_busstop_map)

# Build the graph of streets
def build_street_graph(place): 
    area = ox.geocode_to_gdf(place)
    graph_streets = ox.graph_from_place(place, network_type='drive')
    return(graph_streets)

# Generate a path of size n_hops streets. It will be the longest path among the possibilities
def generate_path_dist(street_graph,offset,n_hops):
    path = []
    cont = 0
    best_dest = []
    for node in street_graph.nodes:
        if cont < offset:
            cont= cont+1
            continue
        candidatos_dist =  nx.descendants_at_distance(street_graph, node, n_hops)
        if (len(candidatos_dist))>1:
            print(street_graph.nodes[node]['x'])
            cords_source = [ street_graph.nodes[node]['x'],street_graph.nodes[node]['y']]
            dest_offset = randint(1, len(candidatos_dist))
            count_dest = 1
            max_dist = -1
            print(f'qtd de possiveis candidatos: {len(candidatos_dist)}')
            for dest in candidatos_dist:  
                dist = nx.shortest_path_length(street_graph, node, dest, weight='length')
                if dist > max_dist:
                  #  print(dist,max_dist)
                    max_dist = dist
                    best_dest = dest
                    path = nx.shortest_path(street_graph, source=node, target=best_dest)
            break
    return(path)
    
def generate_path(street_graph,offset,n_hops):
    path = []
    cont = 0
    for node in street_graph.nodes:
        if cont < offset:
            cont= cont+1
            continue
        candidatos_dist =  nx.descendants_at_distance(street_graph, node, n_hops)
        if (len(candidatos_dist))>1:
            dest_offset = randint(1, len(candidatos_dist))
            count_dest = 1
            for dest in candidatos_dist:  
                if count_dest < dest_offset:
                    count_dest+=1
                    continue
                path = nx.shortest_path(street_graph, source=node, target=dest)
                break
            break
    return(path)


def get_min_max_eta(src,dest,street_graph):    
    dist =  nx.shortest_path_length(street_graph, src, dest, weight='length')
    time_it_takes = dist / 10 # Considering that the speed in the streets is approximately 36 km/h or 10 m/s
    
    min_eta = int(time_it_takes)
    if min_eta == 0 :
        min_eta = 1
    max_eta = min_eta * 2
    return min_eta,max_eta


    
def create_dependency_graph(street_graph,node,dependent_node,current_min_eta, current_max_eta,dependency_graph):
    if current_min_eta <0: 
        return
        
    if node not in dependency_graph:
        dependency_graph[f'{node}'] = []
        
    if current_min_eta == 0 and current_max_eta == 0: 
        return

    key = int(node.split('_')[0])
    edges =  street_graph.in_edges(key)
    # print(f'vai analisar as edges q entram no node {key}')
    # print(f'tem tudo isso de edges {len(edges)} pro no {node}')
    for edge in edges:
        min_eta,max_eta = get_min_max_eta(edge[0],edge[1],street_graph)
        previous_min_eta = current_min_eta
        previous_max_eta = current_max_eta
        current_min_eta-= min_eta
        current_max_eta-= max_eta
        if current_max_eta < current_min_eta:
           current_max_eta = current_min_eta
        adj_node = f'{edge[0]}_{current_min_eta}_{current_max_eta}'
        # print(adj_node,dependent_node)
        if adj_node != dependent_node and adj_node not in dependency_graph[node] and current_min_eta>=0:
            dependency_graph[node].append(adj_node)
            create_dependency_graph(street_graph,adj_node,dependent_node,current_min_eta, current_max_eta,dependency_graph)
        current_min_eta = previous_min_eta
        current_max_eta = previous_max_eta
        

### Builds the dependency graph of the tasks of the streets
def build_dependency_graph(street_graph,path) :
    dependency_graph = {}
    ## The first node doesnt have dependencies
    dependency_graph[f'{path[0]}_0_0']=[]
    current_min_eta = 0
    current_max_eta = 0

    ## Now we compute for each segment of the path
    for i in range(1,len(path)):
        min_eta,max_eta = get_min_max_eta(path[i-1],path[i],street_graph)
        dependent_node = f'{path[i-1]}_{current_min_eta}_{current_max_eta}'
        
        ## We update the ETA with the time it takes to get to the destination
        current_min_eta+= min_eta
        current_max_eta+= max_eta
        node = f'{path[i]}_{current_min_eta}_{current_max_eta}'
        dependency_graph[node] = []
        
        ## The current node in the path has as dependency the previous node
        dependency_graph[node].append(dependent_node)
        create_dependency_graph(street_graph,node,dependent_node,current_min_eta, current_max_eta,dependency_graph)
        
    return(dependency_graph)

def print_parent_graph(graph):
    for node in graph:
        print(node)
    for son in graph:
        for parent in graph[parent]:
            print(parent,son)

def print_path(path,graph):
    for node in path:
        print(graph.in_edges(node))
        print(graph.out_edges(node))

def build_networkx_from_dag(dag):
    digraph = nx.DiGraph()

    for son in dag:
        for parent in dag[node]:
            digraph.add_edge(parent,son)
    return(digraph)

def generate_workload(place,index,amount_requests,n_hops) :    
    total_tasks = 0.0
    total_requests = index + amount_requests
    while (index < total_requests):
        
        street_graph = build_street_graph(place)
        offset = randint(0, len(street_graph.nodes))
        path_ex =  generate_path_dist(street_graph,offset,n_hops)
        if len(path_ex) == 0:
            continue
        dag = build_dependency_graph(street_graph,path_ex)
        
        print('path',path_ex)

        for i in range(1,len(path_ex)):
            src = path_ex[i-1]
            dest = path_ex[i]
            min,max = get_min_max_eta(src,dest,street_graph)
            print(src,dest,min,max)
        
        if len(dag) == 0: continue
        path_computing_infra = '/home/msilvava/postdoc/SmartCityFogComputing-main/projet/vieille-toulouse-computing-nodes.csv'
        dict_busstops = create_clusters_df(street_graph,path_computing_infra)
        digraph = nx.DiGraph()
        all_nodes = []

        ### Printing dag 
        # print('#-#-#-#-#-#-#-#-#_#')
        #
        for son in dag:
          #  all_nodes.append(son)
            for parent in dag[son]:           
            #    all_nodes.append(parent)
                digraph.add_edge(parent,son)
            #    print(parent,son)


       # for node in all_nodes:
       #     key = int(node.split('_')[0]        )
       #     edges = street_graph.in_edges(key)
        #    for edge in edges:
         #       src = edge[0]
          #      dest = edge[1]
           #     min,max = get_min_max_eta(src,dest,street_graph)
            #    print(f'{edge[0]} {edge[1]} {min},{max}')

     #   print('###############################################')
        topological_sorted_dag = list(nx.topological_sort(digraph))
    
        tasks_list = []
        print('qtd de tarefasss',len(topological_sorted_dag))
        total_tasks +=len(topological_sorted_dag)
        req_name = f"req-{index}"
        for node in topological_sorted_dag:
            key = int(node.split('_')[0])
            all_nodes.append(key)
           # if node == topological_sorted_dag[len(topological_sorted_dag)-1]:
            parents_formated = []
            for parent in dag[node]:
                parents_formated.append(f'{req_name}-{parent}')
            item = {
                "name" : f"{req_name}-{node}",
                "flopsAmount": 13.5e8,
                "machine" : f'{dict_busstops[key]}',
                "parents" : parents_formated
            }
            tasks_list.append(item)
            
        # Data to be written
        dictionary = {
            "name": req_name,
            "schemaVersion": "1.0",
            "tasks": tasks_list,
            "path": path_ex
        }     
        # Serializing json
        json_object = json.dumps(dictionary, indent=4)
         
        # Writing to sample.json
        with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/requests_v2_toulouse/req_{index}.json", "w") as outfile:
            outfile.write(json_object)
        index+=1
        print('request',index)
        #if len(topological_sorted_dag) > 20: 
          #  print(topological_sorted_dag, 'len', len(topological_sorted_dag))
     #   break
    print(' media de tarefas para as reqs: ', (total_tasks/amount_requests))
#place = 'Toulouse, France'
place = 'Toulouse, France'

index = int(sys.argv[1])

generate_workload(place,index,3750,5)