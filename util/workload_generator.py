import osmnx as ox
import pandas as pd
import networkx as nx
import math
import json
from random import randint

### Compute the distance between two nodes
def distance(p1, p2):
  return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

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

### Create dictionary of streets and its responsible bus stop
def create_clusters(computing_infra_graph, street_graph):
    dict_street_cluster = {}
    max = int(len(street_graph.nodes)/len(computing_infra_graph.nodes))+1
    print(f'MAX {max}')
    clusters = {}

    for i in range(len(computing_infra_graph.nodes)):
        clusters[i] = []

    for i in range(len(street_graph)):
        x = street_graph.nodes[f'street_{i}']['longitude']
        y = street_graph.nodes[f'street_{i}']['latitude']
        min_dist = 99999999.0
        current_best_cluster = 0
        coord_street = [x,y]
        for j in range(len(computing_infra_graph.nodes)):
            x = computing_infra_graph.nodes[j]['x']
            y = computing_infra_graph.nodes[j]['y']
            coord_infra = [x,y] 
            dist = distance(coord_street, coord_infra) 
            if dist < min_dist and len(clusters[j]) < max:
                min_dist = dist
                current_best_cluster = j
        dict_street_cluster['street_{i}'] = f'bus_stop_{current_best_cluster}'
    return(dict_street_cluster)

# Build the graph of streets
def build_street_graph(place): 
    area = ox.geocode_to_gdf(place)
    graph_streets = ox.graph_from_place(place, network_type='drive')
    return(graph_streets)


def build_computing_infra_df(place):
    graph_bus_stops = ox.geometries_from_place(place, tags={'highway':'bus_stop'})
    return 0


def generate_path(street_graph,offset):
    path = []
    cont = 0
    for node in street_graph.nodes:
        if cont < offset:
            cont= cont+1
            continue
        candidatos_dist =  nx.descendants_at_distance(street_graph, node, 3)
        if (len(candidatos_dist))>1:
            edges_in = street_graph.in_edges(node)
            for dest in candidatos_dist:  
                path = nx.shortest_path(street_graph, source=node, target=dest)
            break
    return(path)
    
def create_parent_graph(graph,node, time, previous,parent_graph):
    if node == previous:
        return
    if node not in parent_graph:
        parent_graph[f'{node}'] = []
    if time == 0 : 
        return
    key = int(node.split('_')[0])
    edges =  graph.in_edges(key)
    for edge in edges:
        adj_node = f'{edge[0]}_{time-1}'
        if adj_node != previous and adj_node not in parent_graph[node]:
            parent_graph[node].append(adj_node)
            create_parent_graph(graph,adj_node, time-1,previous,parent_graph)

def build_parent_graph(graph,path):
    parent_graph = {}
    visited = {}
    for i in range(len(path)):
        parent_graph[f'{path[i]}_{i}']=[]
    for i in range(1,len(path)):
        node = f'{path[i]}_{i}'
        previous = f'{path[i-1]}_{i-1}'
        parent_graph[node].append(previous)
        create_parent_graph(graph,node, i,previous,parent_graph)
    return(parent_graph)


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

def generate_workload(place,amount_requests):
    index =0

    while (index < amount_requests):
    
        street_graph = build_street_graph(place)
        
        offset = randint(0, len(street_graph.nodes))
       # print('vai ter offset de',offset)
        path_ex = generate_path(street_graph,offset)
      #  print('no inicio', path_ex[0],'no fim',path_ex[len(path_ex)-1])
      #  print('caminho',path_ex)
        dag = build_parent_graph(street_graph,path_ex)

        if len(dag) == 0: continue
        #print_parent_graph(dag)
       # print_path(path_ex,street_graph)
        path_computing_infra = '/home/msilvava/postdoc/SmartCityFogComputing-main/projet/vieille-toulouse-computing-nodes.csv'
        dict_busstops = create_clusters_df(street_graph,path_computing_infra)
        digraph = nx.DiGraph()
        for son in dag:
            for parent in dag[son]:
                digraph.add_edge(parent,son)
    
        topological_sorted_dag = list(nx.topological_sort(digraph))
        
      #  for node in topological_sorted_dag:
       #     key = int(node.split('_')[0])
         #   print(node, dict_busstops[key])
         #   print('parents',dag[node])
    
        tasks_list = []
        req_name = f"req-{index}"
        for node in topological_sorted_dag:
            key = int(node.split('_')[0])

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
            "tasks": tasks_list   
        }     
        # Serializing json
        json_object = json.dumps(dictionary, indent=4)
         
        # Writing to sample.json
        with open(f"/home/msilvava/postdoc/smartcity_edge_simulator/input/workload/req_{index}.json", "w") as outfile:
            outfile.write(json_object)
        index+=1
        print('request',index)
        
place = 'Vieille-Toulouse, France'
generate_workload(place,30000)