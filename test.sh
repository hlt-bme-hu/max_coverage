file="$1"
boundaries="$2"

k=`head -n 1 < "$boundaries" | wc -w`

time parallel --eta --sshloginfile .. bash "$PWD/one.sh" $file $k "{1}" "{2}" ::::+ <(head -n -1 "$boundaries") <(tail -n +2 "$boundaries")
