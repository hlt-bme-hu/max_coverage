k=6
n=150
t=64

python generate_boundaries.py $n $k $t > boundaries.txt

head -n $n test/umbc.vocab500.txt > test.txt

time parallel --eta --sshloginfile .. bash "$PWD/one.sh" test.txt $k "{1}" "{2}" ::::+ <(head -n -1 boundaries.txt) <(tail -n +2 boundaries.txt) > result.txt
