#!/bin/bash

# Configuration
NUM_NODES=7  # Number of witness nodes
BASE_PORT=8545
BASE_RPC_PORT=9545
DATA_DIR="./testnet"
BINARY_PATH="./build/quids"

# Create data directory
mkdir -p "$DATA_DIR"

# Generate node keys and configs
for i in $(seq 0 $((NUM_NODES-1))); do
    NODE_DIR="$DATA_DIR/node$i"
    mkdir -p "$NODE_DIR"
    
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
        "max_transactions": 1000,
        "batch_interval_ms": 1200,
        "witness_count": $NUM_NODES,
        "consensus_threshold": 0.67,
        "use_quantum_proofs": true,
        "quantum_circuit_depth": 20,
        "enable_error_correction": true
    },
    "data_dir": "$NODE_DIR",
    "is_witness": true,
    "log_level": "debug"
}
EOF

    # Generate node key if it doesn't exist
    if [ ! -f "$NODE_DIR/node_key.pem" ]; then
        openssl genpkey -algorithm ED25519 -out "$NODE_DIR/node_key.pem"
        openssl pkey -in "$NODE_DIR/node_key.pem" -pubout -out "$NODE_DIR/node_key.pub"
    fi
done

# Start nodes
for i in $(seq 0 $((NUM_NODES-1))); do
    NODE_DIR="$DATA_DIR/node$i"
    
    # Start node in background
    $BINARY_PATH \
        --config "$NODE_DIR/config.json" \
        --key "$NODE_DIR/node_key.pem" \
        --log-file "$NODE_DIR/node.log" &
    
    # Store PID
    echo $! > "$NODE_DIR/node.pid"
    
    echo "Started node $i with PID $(cat $NODE_DIR/node.pid)"
    
    # Wait a bit before starting next node
    sleep 2
done

# Wait for nodes to initialize
sleep 5

# Register witnesses
for i in $(seq 1 $((NUM_NODES-1))); do
    NODE_DIR="$DATA_DIR/node$i"
    PUBKEY=$(cat "$NODE_DIR/node_key.pub" | base64)
    
    # Register with first node (coordinator)
    curl -X POST "http://127.0.0.1:$BASE_RPC_PORT/register_witness" \
         -H "Content-Type: application/json" \
         -d "{\"node_id\": \"node$i\", \"public_key\": \"$PUBKEY\"}"
done

echo "Testnet deployed with $NUM_NODES nodes"
echo "Monitor logs in $DATA_DIR/node*/node.log"

# Helper functions
function stop_testnet() {
    for i in $(seq 0 $((NUM_NODES-1))); do
        NODE_DIR="$DATA_DIR/node$i"
        if [ -f "$NODE_DIR/node.pid" ]; then
            kill $(cat "$NODE_DIR/node.pid")
            rm "$NODE_DIR/node.pid"
        fi
    done
    echo "Testnet stopped"
}

# Register cleanup handler
trap stop_testnet EXIT

# Keep script running
echo "Press Ctrl+C to stop testnet"
while true; do sleep 1; done 