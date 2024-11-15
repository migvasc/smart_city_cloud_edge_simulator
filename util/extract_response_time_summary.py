import sys
import pandas as pd


def process_data(input_file, output_folder):
    input_file = 'f{output_folder}/requests.csv'

    requests_file = open(f'{output_folder}/summary_requests.csv','w')
    requests_file.write(f'total_requests,completed,failed,completed_ratio,avg_time,50p_time,75p_time,90p_time,95p_time,99p_time,max_time\n')
    total_requests=0
    completed = 0
    failed =0
    completed_ratio = 0

    df = pd.read_csv(input_file,sep=',',header=0)
    cols_names = ['req_id','submission_time','finish_time','completed']
    df.columns=cols_names

    df['response_time'] = df['finish_time'] - df['submission_time']

    with open(input_file, "r") as energy_file_data:
        for (index,line) in enumerate(energy_file_data):  
            #0#req-0,0.000000,1.435000,1

            data_to_process = line.split(',')
            total_requests+=1.0
            status = data_to_process[3].replace('\n','')

            if status == '1':
                completed+=1.0
            else:
                failed+=1.0
    completed_ratio = completed/total_requests
    requests_file.write(f'{total_requests},{completed},{failed},{completed_ratio},{round(df.loc[:, 'response_time'].mean(),4)},{ round(df.loc[:, 'response_time'].quantile(.5),4)},{ round(df.loc[:, 'response_time'].quantile(.75),4)},{ round(df.loc[:, 'response_time'].quantile(.9),4)},{ round(df.loc[:, 'response_time'].quantile(.95),4)},{ round(df.loc[:, 'response_time'].quantile(.99),4)},{ round(df.loc[:, 'response_time'].max(),4)}\n')
    requests_file.close()

input_file_path = sys.argv[1]
output_folder = sys.argv[2]
process_data(input_file_path,output_folder)

