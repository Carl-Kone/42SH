expr=caca
caca=pipi

case $expr in 
	caca)
		echo caca
		;;

	pipi)
		echo pipi
		;;

	caca | pipi)
		echo caca pipi
		;;

esac

case $caca in 
	cacan)
		echo caca
		;;

	pipi)
		echo pipi
		;;

	caca | pipi)
		echo caca pipi
		;;

esac
