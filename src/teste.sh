
declare -i a=4800

for i in {1..16}
do
   echo "python3 generate_workload.py 'Toulouse, France' $a 100 4 'toulouse-computing-nodes.csv' 'output' &"
   a+=100
done


echo "wait"
echo 'echo "Finished! "'


#for i in {30000..30400}
#do
#   echo 'rm req_'$i'.json'
#done

