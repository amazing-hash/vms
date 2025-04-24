CC=g++
CFLAGS = -c -Wall -std=c++11 -g `pkg-config --cflags opencv`
LDFLAGS = -lpthread `pkg-config --libs opencv`

SOURCES = sources/app.cpp\
	sources/session/Session.cpp\
	sources/session/StateResponseImpl.cpp\
	sources/session/handlers/HandlerViewVideoStream.cpp\
	sources/session/handlers/HandlerFetchHtml.cpp\
	sources/session/handlers/HandlerFetchCss.cpp\
	sources/session/handlers/HandlerFetchSvg.cpp\
	sources/session/handlers/HandlerFetchJs.cpp\
	sources/session/handlers/HandlerFetchTemplateHtml.cpp\
	sources/session/handlers/HandlerFetchJson.cpp\
	sources/session/RequestFormParse.cpp\
	sources/session/StateRequestImpl.cpp\
	sources/routes/routes.cpp\
	sources/statistics/Statistics.cpp\
	sources/common/base64/base64.cpp\
	sources/common/DateTimeUtil/DateTimeUtil.cpp\
	sources/asyncio.cpp\
	sources/session/Dispatcher.cpp\
	sources/session/TokenCollection.cpp\
	sources/session/ParamCollection.cpp\
	sources/parser/http/picohttpparser.cpp\
	sources/storage/CameraStorage.cpp\
	sources/videowriter/VideoWriter.cpp\
	sources/cameracapture/CameraCaptureMJPEGOverRTSP.cpp\
	sources/cameracapture/CameraCaptureH264OverRTSP.cpp\
	sources/cameracapture/CameraCaptureMJPEG.cpp
    
	
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = vms-duo

all: create_required_dirs copy_db copy_static $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o build/vms-duo $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(OBJECTS)
copy_db:
	cp sources/db/cameras.json build/db/cameras.json
	
copy_static:
	cp -r sources/static/*				    build/static/
	cp sources/supervisor.conf			    build/supervisor.conf
	
create_required_dirs:
	@if [ ! -d build ]; then mkdir build; fi	
	@if [ ! -d build/db ]; then mkdir build/db; fi	
	@if [ ! -d build/static ]; then mkdir build/static; fi	
	@if [ ! -d build/storage ]; then mkdir build/storage; fi	
	
install:
	/etc/init.d/supervisor stop
	@if [ ! -d /opt/vms-duo ]; then mkdir /opt/vms-duo/; fi
	@if [ ! -d /var/log/vms-duo ]; then mkdir /var/log/vms-duo/; fi
	cp -r build/* /opt/vms-duo
	@if [ -z `getent passwd vms-duo` ]; then useradd -p 'vms-duo' vms-duo; fi
	chown -hR vms-duo /opt/vms-duo
	chown -hR vms-duo /var/log/vms-duo
	cp build/supervisor.conf /etc/supervisor/conf.d/supervisor-vms-duo.conf
	
	/etc/init.d/supervisor start