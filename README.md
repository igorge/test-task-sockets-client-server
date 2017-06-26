Test task: simple tcp client/server using old plain sockets.
====


Tested on gcc 6.3.0 (Debian) and Microsoft Visual Studio Community 2017 (Windows 7).


On windows you have to have cmake and visual studio installed, then run build-server.bat and build-client.bat to get objserver.exe and objclient.exe in server/buildwin/Debug and client/buildwin/Debug respectively.


objserver.exe listens on hardcoded port 8888.


objserver.exe usage:
objserver.exe filename-1 filename-2 ... filename-2


objclient.exe usage:
objclient.exe dns-name-or-ip filename-to-get


Example:
----

objserver.exe mesh1.obj c:/meshes/mesh2.obj ../meshes/mesh3.obj

to get first mesh: objclient.exe 127.0.0.1 mesh1.obj

to get second mesh: objclient.exe 127.0.0.1 c:/meshes/mesh2.obj

to get third mesh: objclient.exe 127.0.0.1 ../meshes/mesh3.obj


Note that you have to specify **full path** (as provided to objserver.exe) to get required file from server.


