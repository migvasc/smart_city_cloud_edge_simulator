
def load_dummy_dict():
    request_info_dict = {}
    request_info_dict['total_requests'] =  0.0
    request_info_dict['completed'] = 0.0
    request_info_dict['failed'] = 0.0
    request_info_dict['completed_ratio'] = 0.0
    request_info_dict['avg_time'] = 0.0
    request_info_dict['50p_time'] = 0.0
    request_info_dict['75p_time'] = 0.0
    request_info_dict['90p_time'] = 0.0
    request_info_dict['95p_time'] = 0.0
    request_info_dict['99p_time'] = 0.0
    request_info_dict['max_time'] = 0.0
    return request_info_dict

def load_requests_summary(path):
    request_info_dict = {}
    return load_dummy_dict()

    with open(f'{path}/summary_requests.csv') as input_file:
        for index,line in enumerate(input_file):
            if index == 0:
                continue
            cols = line.split(',')
            request_info_dict['total_requests'] = float(cols[0])
            request_info_dict['completed'] = float(cols[1])
            request_info_dict['failed'] = float(cols[2])
            request_info_dict['completed_ratio'] = float(cols[3])
            request_info_dict['avg_time'] = float(cols[4])
            request_info_dict['50p_time'] = float(cols[5])
            request_info_dict['75p_time'] = float(cols[6])
            request_info_dict['90p_time'] = float(cols[7])
            request_info_dict['95p_time'] = float(cols[8])
            request_info_dict['99p_time'] = float(cols[9])
            request_info_dict['max_time'] = float(cols[10].replace('\n',''))            

    return request_info_dict

dict_baseline  = load_requests_summary('output_paper_last/output_baseline_toulouse_100k/')
dict_heft =         load_requests_summary('output_paper_last/output_heft_toulouse_100k/')
dict_lcaheft_fr =   load_requests_summary('output_paper_last/output_co2_volume_toulouse_100k/')
dict_lcaheft_swe =  load_requests_summary('output_paper_last/output_co2_volume_toulouse_100k_2/')
dict_lcaheft_usa =  load_requests_summary('output_paper_last/output_co2_volume_toulouse_100k_3/')
dict_cloud_only =   load_requests_summary('output_paper_last/output_cloud_only_toulouse_100k/')

output_file = open('requests.txt','w')

output_file.write('\\begin{table}\n')
output_file.write("\\caption{Statistics for the requests' response time (in seconds), in which $P_{n}$ denotes the $n$ percentile.}\\label{tab:requests_statistics} \\centering\n")
output_file.write("\\begin{tabular}{|l|r|r|r|r|r|}\n")
output_file.write("\\hline\n")
output_file.write("\\textbf{Algorithm} &   \\textbf{Avg.} & \\boldmath{$P_{50}$} & \\boldmath{$P_{90}$} & \\boldmath{$P_{95}$}& \\boldmath{$P_{99}$}  \\\\\n")
output_file.write("\\hline\n")
output_file.write(f" Baseline  & {round(dict_baseline['avg_time'],2)} & {round(dict_baseline['50p_time'],2)}  & {round(dict_baseline['90p_time'],2)} &  {round(dict_baseline['95p_time'],2)} & {round(dict_baseline['99p_time'],2)} \\\\")  
output_file.write("\\hline\n")
output_file.write(f"HEFT  &  {round(dict_heft['avg_time'],2)} & {round(dict_heft['50p_time'],2)}  & {round(dict_heft['90p_time'],2)} &  {round(dict_heft['95p_time'],2)} & {round(dict_heft['99p_time'],2)} \\\\\n")
output_file.write("\\hline\n")
output_file.write(f"Offload to cloud  & {round(dict_cloud_only['avg_time'],2)} & {round(dict_cloud_only['50p_time'],2)}  & {round(dict_cloud_only['90p_time'],2)} &  {round(dict_cloud_only['95p_time'],2)} & {round(dict_cloud_only['99p_time'],2)} \\\\\n")
output_file.write("\\hline\n")
output_file.write("\\begin{tabular}{@{}l@{}}Low-Carbon-Aware\\\\ HEFT (cloud FR)\\end{tabular}")
output_file.write(f"& {round(dict_lcaheft_fr['avg_time'],2)} & {round(dict_lcaheft_fr['50p_time'],2)}  & {round(dict_lcaheft_fr['90p_time'],2)} &  {round(dict_lcaheft_fr['95p_time'],2)} & {round(dict_lcaheft_fr['99p_time'],2)} \\\\\n")
output_file.write("\\hline\n")
output_file.write("\\begin{tabular}{@{}l@{}}Low-Carbon-Aware\\\\ HEFT (cloud USA)\\end{tabular}")
output_file.write(f"& {round(dict_lcaheft_swe['avg_time'],2)} & {round(dict_lcaheft_swe['50p_time'],2)}  & {round(dict_lcaheft_swe['90p_time'],2)} &  {round(dict_lcaheft_swe['95p_time'],2)} & {round(dict_lcaheft_swe['99p_time'],2)} \\\\\n")
output_file.write("\\hline\n")
output_file.write("\\begin{tabular}{@{}l@{}}Low-Carbon-Aware\\\\ HEFT (cloud SWE)\\end{tabular}")
output_file.write(f"& {round(dict_lcaheft_usa['avg_time'],2)} & {round(dict_lcaheft_usa['50p_time'],2)}  & {round(dict_lcaheft_usa['90p_time'],2)} &  {round(dict_lcaheft_usa['95p_time'],2)} & {round(dict_lcaheft_usa['99p_time'],2)} \\\\\n")
output_file.write("\\hline\n")
output_file.write("\\end{tabular}\n")
output_file.write("\\end{table}\n")


output_file.close()