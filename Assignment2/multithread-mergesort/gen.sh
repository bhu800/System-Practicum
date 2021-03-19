for((i = 2;i<600; i=i*2)); do
	echo $i
	(./a $i) >> "log.txt"
done