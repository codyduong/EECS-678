find $1 -name '*'.[ch] | xargs grep -c $2 | sort -t : +1.0 -2.0 --numeric --reverse | head --lines=$3
