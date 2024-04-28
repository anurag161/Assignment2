# Read the topo.txt file and parse its contents
with open("topo.txt", "r") as file:
    lines = file.readlines()

client_server_conns = []
client_client_conns = []

connections = ""

servers = set()
clients = set()

for line in lines:
    parameters = line.split()

    node1, node2 = parameters[0], parameters[1]
    delay1, delay2 = parameters[2], parameters[3]

    if(node1[0] == 'c' and node2[0] == 'c'):
        clients.add(node1)
        clients.add(node2)

        if(node1[1] == node2[1]):
            conn = f"""\n\t\t{node1}.clientOut++ --> {{  delay = {delay1}ms; }} --> {node2}.clientIn++;\n"""
        else:
            conn = f"""\t\t{node1}.clientOut++ --> {{  delay = {delay1}ms; }} --> {node2}.clientIn++;
        {node2}.clientOut++ --> {{  delay = {delay2}ms; }} --> {node1}.clientIn++;\n\n"""

    elif(node1[0] == 'c' and node2[0] == 's'):
        clients.add(node1)
        servers.add(node2)

        conn = f"""\t\t{node2}.out++ --> {{  delay = {delay2}ms; }} --> {node1}.serverIn++;
        {node2}.in++ <-- {{  delay = {delay1}ms; }} <-- {node1}.serverOut++;\n\n"""

    elif(node1[0] == 's' and node2[0] == 'c'):
        clients.add(node2)
        servers.add(node1)

        conn = f"""\t\t{node1}.out++ --> {{  delay = {delay1}ms; }} --> {node2}.serverIn++;
        {node1}.in++ <-- {{  delay = {delay2}ms; }} <-- {node2}.serverOut++;\n"""
    connections += conn


submodules = ""
serversList = sorted(servers)
clientsList = sorted(clients)
numServers = len(serversList)
numClients = len(clientsList)

for i in range(numServers):
    submodules += f"""\t\t{serversList[i]}: Server{{
        \t@display("p={int(400 * (i + 1) / (numServers + 1))},{int(100 - min(i, numServers - i - 1) * (50 / ((numServers - 1) / 2)))}");
        \tId = {i};
        \tnumServers = parent.numServers;
        \tnumClients = parent.numClients;
        }}\n\n"""

for i in range(len(clientsList)):
    submodules += f"""\t\t{clientsList[i]}: Client{{
        \t@display("p={int(400 * (i+1) / (len(serversList) + 1))},{int(200 + min(i, numClients - i - 1) * (50 / ((numClients - 1) / 2)))}");
        \tId = {i};
        \tnumServers = parent.numServers;
        \tnumClients = parent.numClients;
        }}\n\n"""


ned_content = """simple Server
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
"""

ned_content += submodules  
    
ned_content += """
    connections:
"""

ned_content += connections

ned_content += "\n}"

# Write the content to a .ned file
with open("Network.ned", "w") as ned_file:
    ned_file.write(ned_content)
