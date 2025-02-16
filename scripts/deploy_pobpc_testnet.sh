#!/bin/bash

# POBPC Testnet Configuration
NUM_WITNESSES=7
BASE_PORT=8545
BASE_RPC_PORT=9545
DATA_DIR="./testnet_pobpc"
BINARY_PATH="./build/quids"

# Create data directory
mkdir -p "$DATA_DIR"

# Generate Dilithium keys for each witness
for i in $(seq 0 $((NUM_WITNESSES-1))); do
    NODE_DIR="$DATA_DIR/witness$i"
    mkdir -p "$NODE_DIR"
    
    # Generate Dilithium keys
    $BINARY_PATH keygen \
        --type dilithium \
        --output "$NODE_DIR/dilithium" \
        --strength 5

    # Generate Kyber keys
    $BINARY_PATH keygen \
        --type kyber \
        --output "$NODE_DIR/kyber" \
        --strength 1024
    
    # Generate node configuration
    cat > "$NODE_DIR/config.json" << EOF
{
    "network": {
        "listen_port": $((BASE_PORT + i)),
        "rpc_port": $((BASE_RPC_PORT + i)),
        "bootstrap_nodes": [
            "127.0.0.1:$BASE_PORT"
        ]
    },
    "consensus": {
        "type": "pobpc",
        "max_transactions": 1000,
        "batch_interval_ms": 1200,
        "witness_count": $NUM_WITNESSES,
        "consensus_threshold": 0.67,
        "use_quantum_proofs": true,
        "quantum_circuit_depth": 20,
        "enable_error_correction": true,
        "num_parallel_verifiers": 4
    },
    "crypto": {
        "dilithium_key": "$NODE_DIR/dilithium",
        "kyber_key": "$NODE_DIR/kyber",
        "key_rotation_interval": 50,
        "quantum_audit": true
    },
    "data_dir": "$NODE_DIR",
    "is_witness": true,
    "log_level": "debug"
}
EOF
done

# Start witness nodes
for i in $(seq 0 $((NUM_WITNESSES-1))); do
    NODE_DIR="$DATA_DIR/witness$i"
    
    # Start node with POBPC configuration
    $BINARY_PATH node \
        --config "$NODE_DIR/config.json" \
        --role witness \
        --metrics-port $((10000 + i)) &
    
    # Store PID
    echo $! > "$NODE_DIR/node.pid"
    
    echo "Started POBPC witness $i with PID $(cat $NODE_DIR/node.pid)"
    
    # Wait for node initialization
    sleep 2
done

# Wait for network stabilization
sleep 5

# Register witnesses with coordinator
for i in $(seq 1 $((NUM_WITNESSES-1))); do
    NODE_DIR="$DATA_DIR/witness$i"
    DILITHIUM_PUB=$(cat "$NODE_DIR/dilithium.pub" | base64)
    
    # Register with coordinator node
    curl -X POST "http://127.0.0.1:$BASE_RPC_PORT/register_witness" \
         -H "Content-Type: application/json" \
         -d "{
             \"node_id\": \"witness$i\",
             \"dilithium_key\": \"$DILITHIUM_PUB\",
             \"role\": \"pobpc_witness\"
         }"
done

echo "POBPC testnet deployed with $NUM_WITNESSES witnesses"
echo "Monitor metrics at http://localhost:10000/metrics"

# Helper functions
function stop_testnet() {
    for i in $(seq 0 $((NUM_WITNESSES-1))); do
        NODE_DIR="$DATA_DIR/witness$i"
        if [ -f "$NODE_DIR/node.pid" ]; then
            kill $(cat "$NODE_DIR/node.pid")
            rm "$NODE_DIR/node.pid"
        fi
    done
    echo "POBPC testnet stopped"
}

# Register cleanup handler
trap stop_testnet EXIT

# Keep script running and display metrics
echo "Press Ctrl+C to stop testnet"
while true; do
    echo "Current TPS: $(curl -s http://localhost:10000/metrics | grep pobpc_tps)"
    echo "Batch efficiency: $(curl -s http://localhost:10000/metrics | grep pobpc_efficiency)"
    echo "Energy usage: $(curl -s http://localhost:10000/metrics | grep pobpc_energy)"
    sleep 5
done 