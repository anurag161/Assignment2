simple Server
{
    parameters:
    	int Id;
        int numServers;
        int numClients;
    gates:
        input in[];
        output out[];
}

simple Client
{
    parameters:
    	int Id;
        int numServers;
        int numClients; 
    gates:
        input serverIn[];
        output serverOut[];
        input clientIn[];
        output clientOut[];
}

network Net
{
    parameters:
        int numServers;
        int numClients;
        @display("bgb=400,300");
        
    submodules:
		s0: Server{
        	@display("p=100,100");
        	Id = 0;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }

		s1: Server{
        	@display("p=200,50");
        	Id = 1;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }

		s2: Server{
        	@display("p=300,100");
        	Id = 2;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }

		c0: Client{
        	@display("p=100,200");
        	Id = 0;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }

		c1: Client{
        	@display("p=200,250");
        	Id = 1;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }

		c2: Client{
        	@display("p=300,200");
        	Id = 2;
        	numServers = parent.numServers;
        	numClients = parent.numClients;
        }


    connections:
		s0.out++ --> {  delay = 100ms; } --> c0.serverIn++;
        s0.in++ <-- {  delay = 100ms; } <-- c0.serverOut++;

		s1.out++ --> {  delay = 100ms; } --> c0.serverIn++;
        s1.in++ <-- {  delay = 100ms; } <-- c0.serverOut++;

		s2.out++ --> {  delay = 100ms; } --> c0.serverIn++;
        s2.in++ <-- {  delay = 100ms; } <-- c0.serverOut++;

		s0.out++ --> {  delay = 100ms; } --> c1.serverIn++;
        s0.in++ <-- {  delay = 100ms; } <-- c1.serverOut++;

		s1.out++ --> {  delay = 100ms; } --> c1.serverIn++;
        s1.in++ <-- {  delay = 100ms; } <-- c1.serverOut++;

		s2.out++ --> {  delay = 100ms; } --> c1.serverIn++;
        s2.in++ <-- {  delay = 100ms; } <-- c1.serverOut++;

		s0.out++ --> {  delay = 100ms; } --> c2.serverIn++;
        s0.in++ <-- {  delay = 100ms; } <-- c2.serverOut++;

		s1.out++ --> {  delay = 100ms; } --> c2.serverIn++;
        s1.in++ <-- {  delay = 100ms; } <-- c2.serverOut++;

		s2.out++ --> {  delay = 100ms; } --> c2.serverIn++;
        s2.in++ <-- {  delay = 100ms; } <-- c2.serverOut++;


		c0.clientOut++ --> {  delay = 100ms; } --> c0.clientIn++;
		c0.clientOut++ --> {  delay = 100ms; } --> c1.clientIn++;
        c1.clientOut++ --> {  delay = 100ms; } --> c0.clientIn++;

		c0.clientOut++ --> {  delay = 100ms; } --> c2.clientIn++;
        c2.clientOut++ --> {  delay = 100ms; } --> c0.clientIn++;


		c1.clientOut++ --> {  delay = 100ms; } --> c1.clientIn++;
		c1.clientOut++ --> {  delay = 100ms; } --> c2.clientIn++;
        c2.clientOut++ --> {  delay = 100ms; } --> c1.clientIn++;


		c2.clientOut++ --> {  delay = 100ms; } --> c2.clientIn++;

}