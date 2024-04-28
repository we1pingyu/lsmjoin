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
                                                                              
After compiling RocksDB, enter the `lsm_join` directory and run the `.sh` scripts with your data path. The scripts serve the following functions:      
                                                                             
- `test_overall.sh`: Overall evaluation of all join algorithms and datasets, corresponding to Figure 3 in the paper.                                   
- `test_overall_different_size.sh`: Evaluation on different data sizes and distributions, corresponding to Figure 4 in the paper.                    
- `test_lsm.sh`: Evaluation on different LSM-tree configurations, corresponding to Figures 4 and 5 in the paper.                             
- `test_breakdown.sh`: Latency breakdown on join algorithms, corresponding to Table 6 in the paper.                      

