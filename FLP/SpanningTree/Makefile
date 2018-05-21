make:
	swipl $(ARGS) -q -g start -o flp18-log -c flp18-log.pl

clean:
	rm flp18-log flp-log-xmarus06.zip

zip:
	zip -r flp-log-xmarus06.zip flp18-log.pl Makefile README Tests
