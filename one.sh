cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
./`hostname`/maxcover --input "$1" -k $2 --begin "$3" --end "$4" 2> /dev/null
