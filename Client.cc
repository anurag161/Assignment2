#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include <random>
#include <vector>
#include <random>
#include <ctime>
#include "TaskMes_m.h"  // Include the generated header file for your message class
#include "GossipMessage_m.h"  // Include the generated header file for your message class
#include <fstream>

using namespace omnetpp;

class Client : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void performTask1();
    virtual void performTask2();
    virtual void calculateServerScores();
    virtual void updateServerScores(std::string scores, bool avg);
    virtual void distributeServerScores();


    int noOfRounds = 2;
    std::string task[2] = {"abcdeefasdasdsad", "aaaaaasdasdsadaa"};


    std::vector<std::vector<std::pair<int,int>>> serverResponses;
    int serverResponseCount = 0;
    int clientResponseCount = 0;
    int currentRound = 0;

    std::vector<int> serverScores;

    int numServers;
    int numClients;

    int id;

};

Define_Module(Client);

void createLogFile(std::string personType ,int Index){
    std::string filename = personType + "_Log_" + std::to_string(Index) + ".txt";
    std::ofstream logfile(filename);
    if (logfile.is_open()) {
        std::cout << "Log file created: " << filename << std::endl;
        logfile.close();
    } else {
        std::cerr << "Error: Unable to create log file." << std::endl;
    }
}

void writeToLogFile(std::string personType, std::string output, int personId){
    std::string filename = personType + "_Log_" + std::to_string(personId) + ".txt";
    std::ofstream logfile(filename, std::ios_base::app); // Open the file in append mode
    if (logfile.is_open()) {
        logfile << output << std::endl; // Write the output to the file
        logfile.close();
    } else {
        std::cerr << "Error: Unable to open log file for writing." << std::endl;
    }
}


int generateRandomNumber(int numServers) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, numServers - 1);

    return dis(gen);
}

int majorityResponse(std::vector<std::pair<int,int>> responses){
    std::map<int, int> answers;
    for(auto response:responses){
        if(answers.find(response.second) == answers.end())
            answers[response.second] = 1;
        else
            answers[response.second]++;
    }

    int majorityResponse = answers.begin()->first;
    int reponseOccurance = answers.begin()->second;
    for(auto it : answers){
        if(it.second > reponseOccurance){
            majorityResponse = it.first;
            reponseOccurance = it.second;
        }
    }

    return majorityResponse;
}

std::string serializeScores(std::vector<int> serverScores){
    std::string scores = "";
    for(auto score:serverScores){
        scores += std::to_string(score);
        scores += "_";
    }
    return scores;
}

std::vector<int> deserializeScores(std::string scores){
    std::vector<int> serverScores;
    std::string temp = "";
    for(int index = 0; index < scores.length(); index++){
        if(scores[index] == '_'){
            serverScores.push_back(stoi(temp));
            temp = "";
        }
        else
            temp += scores[index];
    }
    return serverScores;
}


void Client::initialize()
{
    numServers = par("numServers").intValue();
    numClients = par("numClients").intValue();
    id = par("Id").intValue();

    serverScores.resize(numServers, 0);
    serverResponses.resize(numServers);  // Number of Subtasks

    createLogFile("ClientResponses", id);
    createLogFile("ClientGossips", id);

    performTask1();
}

void Client::performTask1(){
    int startIndex = 0;

    for(int subtask = 0; subtask < numServers; subtask++){
        // Subtask Query String
        std::string subTaskQuery;
        if(subtask != (numServers-1))
            subTaskQuery = task[currentRound].substr(startIndex, 2);
        else
            subTaskQuery = task[currentRound].substr(startIndex);

        // Select N/2 + 1 Servers
        std::set<int> randomServers;
        int range = numServers;

        while(randomServers.size() < (numServers/2+1)){
            int num = generateRandomNumber(numServers);
            randomServers.insert(num);
        }

        // Send Message
        for(auto serverId : randomServers){
            TaskMes *msg = new TaskMes("Query");
            msg->setText(subTaskQuery.c_str());
            msg->setTaskId(currentRound);
            msg->setSubTaskId(subtask);

            auto currentTime = std::time(nullptr);
            struct tm *localTime = std::localtime(&currentTime);
            char time[9]; // Format: "hh:mm:ss\0"
            std::strftime(time, sizeof(time), "%H:%M:%S", localTime);
            msg->setTimestamp(time);

            send(msg, "serverOut", serverId);
        }
        startIndex += 2;
    }
}

void Client::performTask2(){
    int startIndex = 0;

    for(int subtask = 0; subtask < numServers; subtask++){
        // Subtask Query String
        std::string subTaskQuery;
        if(subtask != (numServers-1))
            subTaskQuery = task[currentRound].substr(startIndex, 2);
        else
            subTaskQuery = task[currentRound].substr(startIndex);

        // Sort servers by their scores
        std::vector<std::pair<int, int>> trustedServers;
        for(int serverId = 0; serverId < numServers; serverId++)
            trustedServers.push_back({serverScores[serverId], serverId});

        sort(trustedServers.rbegin(), trustedServers.rend());


        // Send Message
        for(int index = 0; index < (numServers / 2 + 1); index++){
            TaskMes *msg = new TaskMes("Query");
            msg->setText(subTaskQuery.c_str());
            msg->setTaskId(currentRound);
            msg->setSubTaskId(subtask);

            auto currentTime = std::time(nullptr);
            struct tm *localTime = std::localtime(&currentTime);
            char time[9]; // Format: "hh:mm:ss\0"
            std::strftime(time, sizeof(time), "%H:%M:%S", localTime);
            msg->setTimestamp(time);

            send(msg, "serverOut", trustedServers[index].second);
        }
        startIndex += 2;
    }
}

void Client::calculateServerScores()
{
    for(int subtask = 0; subtask < numServers; subtask++){
        int correctAnswer = majorityResponse(serverResponses[subtask]);

        for(auto response:serverResponses[subtask]){
            if(response.second == correctAnswer)
                serverScores[response.first] += 1;
        }
    }
}

void Client::distributeServerScores()
{
    std::string scores = serializeScores(serverScores);
    for(int client = 0; client < numClients; client++){
        if(client != id){
            GossipMessage *msg = new GossipMessage("Gossip");
            msg->setText(scores.c_str());
            msg->setTaskId(currentRound);

            auto currentTime = std::time(nullptr);
            struct tm *localTime = std::localtime(&currentTime);
            char time[9]; // Format: "hh:mm:ss\0"
            std::strftime(time, sizeof(time), "%H:%M:%S", localTime);
            msg->setTimestamp(time);

            send(msg, "clientOut", client);
        }
    }
}

void Client::updateServerScores(std::string scores, bool roundCompleted)
{
    std::vector<int> scoreValues = deserializeScores(scores);
    for(int server = 0; server < numServers; server++){
        serverScores[server] += scoreValues[server];
        if(roundCompleted)
            serverScores[server] /= numClients;
    }
}

void Client::handleMessage(cMessage *msg)
{
    if (dynamic_cast<TaskMes*>(msg) != nullptr) {
        TaskMes *myMsg = check_and_cast<TaskMes*>(msg);
        int clientId = id;


        cGate *arrivalGate = myMsg->getArrivalGate();
        int serverId = arrivalGate->getIndex();

        int taskId = myMsg->getTaskId();
        int subTaskId = myMsg->getSubTaskId();

        std::string logMessage = "";

        logMessage = "Query Response, From Server-" + std::to_string(serverId) + ", To Client-" + std::to_string(clientId) + ", Task-" + std::to_string(taskId) + ", SubTask-" + std::to_string(subTaskId) + ", Response:" + myMsg->getText() + ", Timestamp:" + myMsg->getTimestamp();

        EV_INFO << logMessage << endl;

        // Write the output to the log file
        writeToLogFile("ClientResponses",logMessage, clientId);


        serverResponses[subTaskId].push_back({serverId, std::stoi(myMsg->getText())});
        serverResponseCount++;

        if(serverResponseCount >= (numServers/2+1)*(numServers)){
            calculateServerScores();
            distributeServerScores();
        }
    }
    else if(dynamic_cast<GossipMessage*>(msg) != nullptr){
        GossipMessage *myMsg = check_and_cast<GossipMessage*>(msg);
        int receiverId = id;

        cGate *arrivalGate = myMsg->getArrivalGate();
        int senderId = arrivalGate->getIndex();

        int taskId = myMsg->getTaskId();

        std::string logMessage = "";

        logMessage = "Gossip, From Client-" + std::to_string(senderId) + ", To Client-" + std::to_string(receiverId) + ", Task-" + std::to_string(taskId) + ", Scores:" + myMsg->getText() + ", Timestamp:" + myMsg->getTimestamp();


        EV_INFO << logMessage <<endl;

        writeToLogFile("ClientGossips",logMessage, receiverId);


        clientResponseCount++;
        if(clientResponseCount < (numClients-1))
            updateServerScores(myMsg->getText(), false);
        else{
            updateServerScores(myMsg->getText(), true);
            currentRound++;
            if(currentRound < noOfRounds){
                serverResponseCount = 0;
                clientResponseCount = 0;
                serverResponses.clear();
                serverResponses.resize(numServers);
                performTask2();
            }
        }

    }
    delete(msg);
}
