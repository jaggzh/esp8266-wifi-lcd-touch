# IP_TOUCHDEV=192.168.1.20  # Should set this here or in environment for 'make hit'

hit:
	curl "$$IP_TOUCHDEV/cs?cls=r=100&col=r=200,g=300,b=250&frect=50,50,50,20,5&col=r=30,g=230&frect=100,100,80,30,1&col=b=230&frect=180,180,40,50,10&txt=s=1,t=Hello,x=0&tfg=r=225,b=255&txt=s=2,t=World,x=30,y=30&tfg=r=255,g=255&txt=s=3,t=+Over+there"

tags: *.cpp *.c *.ino *.h
	ctags *.cpp *.c *.ino *.h

vi:
	vim Makefile \
		README.md \
		esp8266-wifi-lcd-touch.ino \
		esp8266-wifi-lcd-touch.h \
		wifi.cpp \
		wifi.h \
		~/Arduino/libraries/mini-ppm-info/mini-ppm-info.cpp \
		~/Arduino/libraries/mini-ppm-info/mini-ppm-info.h \
		log.cpp \
		log.h \
		httptime.cpp \
		httptime.h \
		settings.h \
		esp-subparams.h \
		colorstuff.cpp \
		colorstuff.h \
		non-arduino.h \
		printutils.h \
		ota.cpp \
		ota.h \
		wifi_config--example.h \
		wifi_config.h \
		libsecretebmp.c \
		libsecretebmp.h \
		test-libbmp.c-disabled
