def create_dict_requests(path):
    dict_requests = {}
    with open(path,'r') as req_file:
        for index,line in enumerate(req_file):            
            if index == 0: 
                continue
            row = line.split(';')
       #     print(row)
            dict_requests[row[0]] = float(row[4])
    
    return dict_requests

dict_baseline_min_requests = create_dict_requests('output_baseline_min/requests.csv')
dict_alg_to_compare_requests = create_dict_requests('output_baseline_30k/requests.csv')

baseline_makespan = 0.0
alg_comp_makespan = 0.0
cont = 0.0
mean_diff = 0.0
best_cloud = 0
cloud_reqs = []
for key in dict_alg_to_compare_requests:
    baseline_makespan+=dict_baseline_min_requests[key]
    alg_comp_makespan+=dict_alg_to_compare_requests[key]
    mean_diff += abs ( (dict_alg_to_compare_requests[key]-dict_baseline_min_requests[key])/dict_baseline_min_requests[key])
    cont+=1
    if dict_alg_to_compare_requests[key] < dict_baseline_min_requests[key]:
        print('opa')
        print(key,dict_alg_to_compare_requests[key] ,dict_baseline_min_requests[key])
        best_cloud+=1
    

baseline_makespan = baseline_makespan/cont
alg_comp_makespan = alg_comp_makespan/cont
mean_diff = mean_diff/cont
print(baseline_makespan,alg_comp_makespan,mean_diff)