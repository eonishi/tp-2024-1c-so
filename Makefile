
clean:
	make clean -C ./cpu
	make clean -C ./memory
	make clean -C ./kernel
	make clean -C ./io
	make clean -C ./shared