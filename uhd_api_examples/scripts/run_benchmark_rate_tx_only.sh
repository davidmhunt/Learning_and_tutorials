#!/bin/bash

/home/david/uhd/host/build/examples/benchmark_rate --args serial="3231EFC" --duration=10 --rx_subdev "A:A A:B" --tx_subdev "A:A A:B" --rx_stream_args "" --tx_stream_args "" --tx_rate 50000000  --ref "internal" --channels "0,1" --tx_channels "0" --overrun-threshold 1 --underrun-threshold -1 --drop-threshold 100 --seq-threshold 100 --tx_delay 0.25