# Bug in Verilator that leads to a wrong runtime value

To reproduce, please have Verilator installed and run:
```
bash run.sh
```

Tested with `Verilator 5.021 devel rev v5.020-31-g54922c036`.

Expected output:
```
Noflag
Output: 18
-fno-expand
Output: 18
```

Actual output:
```
Noflag
Output: 19
-fno-expand
Output: 18
```

Please note the bug is corrected by enabling any of these flags:
`-fno-expand`
`-fno-const-bit-op-tree`.
Any other unique flag, as well as traces, will preserve this bug.
