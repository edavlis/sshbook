sshbook: sshbook.c
	$(CC) sshbook.c -o sshbook 

install: sshbook
	install -m 755 sshbook /usr/local/bin/sshbook
