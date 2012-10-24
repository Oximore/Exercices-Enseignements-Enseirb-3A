### README by aurelien.esnard@labri.fr ###

# Compilation

  $ mkdir stubs
  $ idlj -fall -td stubs/ Echo.idl
  $ javac *.java stubs/*.java

# Execution

1) Starting Naming Service daemon

  $ tnameserv -ORBInitialPort 12457 &

2) Starting Echo Server

  $ java -cp stubs:. EchoServer -ORBInitRef NameService=corbaloc::localhost:12457/NameService

3) Running Echo Client

  $ java -cp stubs:. EchoClient -ORBInitRef NameService=corbaloc::localhost:12457/NameService
