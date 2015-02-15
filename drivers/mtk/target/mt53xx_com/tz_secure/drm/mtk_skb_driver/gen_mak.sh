ABC=TomCrypt_file.txt
rm -rf Makefile.customer
for i in `cat $ABC`; do
	#pushd $i
	C_FILE=`find $i -name "*.c" -maxdepth 1` 
	for j in $C_FILE; do
		echo "CSOURCES += $j" >> Makefile.customer
	done
	#popd
done