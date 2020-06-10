#! /bin/zsh
dir="../traces"

for file in `ls $dir`
do
	echo $file
  # bunzip2 -kc $dir/$file | ./predictor --gshare:$1
	bunzip2 -kc $dir/$file | ./predictor --custom:$1:9
done
