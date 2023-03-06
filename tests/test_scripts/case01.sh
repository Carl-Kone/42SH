expr=caca

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

case $expr in 
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
