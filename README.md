# LSM Join Guide

Follow these steps to set up and execute the LSM Join with RocksDB.

## Step 1: Download the Dataset

First, download the necessary dataset from Google Drive:

[Download Dataset](https://drive.google.com/drive/folders/1k_q1pq2C01rGChallg6aBPq4IOqJppzO?usp=sharing)

## Step 2: Compile RocksDB

Before running LSM Join, you need to compile RocksDB. Navigate to the root directory of RocksDB and execute the following command:

```bash
make static_lib
```

## Step 3: Run Benchmarks                                                   
                                                                              
After compiling RocksDB, enter the `lsm_join` directory and run the `.sh` scripts, specifying your data path. The scripts are named according to the corresponding section numbers in the paper, reflecting the experiments described in each section. Additionally, we provide scripts for comprehensive evaluations:       
                                                                             
- `test_overall.sh`: Overall evaluation of all join algorithms and datasets.                         
- `test_breakdown.sh`: Latency breakdown on join algorithms.     


For the **Pebble version** of the project, check out [lsm_join_pebble](https://github.com/we1pingyu/lsm_join_pebble).


