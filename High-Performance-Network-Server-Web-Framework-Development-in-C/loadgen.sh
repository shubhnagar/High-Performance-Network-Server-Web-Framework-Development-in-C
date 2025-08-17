#!/bin/bash
echo "Analyzing and Generating PNG!!"

# Different Sizes of Clients
SIZES='
    1
    10
    100
    500
    1000
    5000
    8000
    15000
'      
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <server_ip> <server_port>"
    exit 1
fi

gcc client.c -o client  

SERVER_IP=$1
SERVER_PORT=$2


echo "making a result file"
touch result.txt
> result.txt

run_clients() {
    NUM_CLIENTS=$1
    start=$(date +%s.%N)
    for ((i = 1; i <= NUM_CLIENTS; i++)); do
        echo "Starting client #$i"
        ./client $SERVER_IP $SERVER_PORT $i &
    done
    wait  
    end=$(date +%s.%N)
    elapsed=$(echo "$end - $start" | bc)
    echo $NUM_CLIENTS $elapsed  >> result.txt
}

for clients in $SIZES; do
    run_clients $clients
done

echo "Done!!"
