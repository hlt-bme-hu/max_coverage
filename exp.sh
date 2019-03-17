k=6
n=150
t=16

python generate_boundaries.py $n $k $t > boundaries.txt

head -n $n test/umbc.vocab500.txt > test.txt

bash test.sh test.txt boundaries.txt > result.txt

bash post_process.sh result.txt
