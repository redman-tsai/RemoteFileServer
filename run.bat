: run.bat 
@echo start server1
 start .\Server\Debug\Server.exe localhost 9080
@echo start server2
 start .\Server\Debug\Server.exe localhost 9070
@echo start gui
start .\Debug\CppCli-WPF-App.exe 9050
