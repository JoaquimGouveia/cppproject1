/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "newsgroup.h"
#include "article.h"
#include "protocol.h"
#include "messagehandler.h"
#include "database_interface.h"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <memory>
#include <stdexcept>
#include <string>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

Server init(int argc, char* argv[])
{
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = std::stoi(argv[1]); //turns string to integer
        } catch (std::exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

void serve_one(Server& server, database_interface& db)
{
        auto conn = server.waitForActivity();
        if (conn != nullptr) {
                //Inte bestämt hur vi ska välja/hantera databasen!
                try {
                    process_command(conn, db);
                } catch (ConnectionClosedException&) {
                        server.deregisterConnection(conn);
                        cout << "Client closed connection" << endl;
                }
        } else {
                conn = std::make_shared<Connection>();
                server.registerConnection(conn);
                cout << "New client connects" << endl;
        }
}
void list_newsgroups(MessageHandler& msg, database_interface& db) {
        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
                return;
        }

        //Get newsgroups from the database
        std::vector<Newsgroup> groups = db.list_newsgroups();

        //First byte
        msg.sendCode(static_cast<int>(Protocol::ANS_LIST_NG));

        //Second byte
        msg.sendIntParameter(groups.size());

         //Send each group's id and name
        for (const Newsgroup& group : groups) {
                msg.sendIntParameter(group.getId());
                msg.sendStringParameter(group.getName());
        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void create_newsgroup(MessageHandler& msg, database_interface& db) {
        std::string newsgroupName = msg.recvStringParameter();
        
        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
        return;
        }
        
        //First byte 
        msg.sendCode(static_cast<int>(Protocol::ANS_CREATE_NG));

        //Second and potentially third byte (if name already exits)
        bool success = db.create_newsgroup(newsgroupName);
        
        if(success){ 
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK)); //creates the newsgroup
        }else{ 
                msg.sendCode(static_cast<int>(Protocol::ANS_NAK));
                msg.sendCode(static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS));

        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void delete_newsgroup(MessageHandler& msg, database_interface& db) {  
        int id = msg.recvIntParameter();

        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
        return;
        }

        //First byte
        msg.sendCode(static_cast<int>(Protocol::ANS_DELETE_NG));

        //Second and potentially third byte (if no newsgroup was deleted)
        bool deleted = db.delete_newsgroup(id);
        if(deleted){
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK));
        }else{
                msg.sendCode(static_cast<int>(Protocol::ANS_NAK));
                msg.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void list_articles(MessageHandler& msg, database_interface& db) {
        int newsgroupsid = msg.recvIntParameter();

        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
        return;
        }

        //First byte 
        msg.sendCode(static_cast<int>(Protocol::ANS_LIST_ART));

        bool newsgroupFound = db.get_newsgroup(newsgroupsid);
        if(newsgroupFound){
                auto articles = db.list_articles(newsgroupsid);
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK));
                msg.sendIntParameter(articles.size());

                
                for (const auto& [id, article] : articles) {
                        msg.sendIntParameter(id);
                        msg.sendStringParameter(article.getTitle());
                }
                        
        }else{
                msg.sendCode(static_cast<int>(Protocol::ANS_NAK));
                msg.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void create_article(MessageHandler& msg, database_interface& db) {
        int newsgroupsid = msg.recvIntParameter();
        std::string title = msg.recvStringParameter();
        std::string author = msg.recvStringParameter();
        std::string text = msg.recvStringParameter();

        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                std::cerr << "Expected COM_END after COM_LIST_NG" << std::endl;
        return;
        }

        //First byte
        msg.sendCode(static_cast<int>(Protocol::ANS_CREATE_ART));

        bool created = db.create_article(newsgroupsid, title, author, text);
        if(created){
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK));
        }else{
                msg.sendCode(static_cast<int>(Protocol::ANS_NAK));
                msg.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void delete_article(MessageHandler& msg, database_interface& db) {   
        int newsgroupsid = msg.recvIntParameter();
        int articlesid = msg.recvIntParameter();

        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
        return;
        }

        //First byte
        msg.sendCode(static_cast<int>(Protocol::ANS_DELETE_ART));

        bool deleted = db.delete_article(newsgroupsid, articlesid);
        if(deleted){
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK));
        }else{
                msg.sendCode(static_cast<int>(Protocol::ANS_NAK));
                bool newsgroupFound = db.get_newsgroup(newsgroupsid);
                if(!newsgroupFound){
                        msg.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
                }else{
                        msg.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
                }
        }

        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}
    
void get_article(MessageHandler& msg, database_interface& db) {
        int newsgroupsid = msg.recvIntParameter();
        int articlesid = msg.recvIntParameter();

        //Checks for COM_END
        int end = msg.recvCode();
        if (end != static_cast<int>(Protocol::COM_END)) {
                cerr << "Expected COM_END after COM_LIST_NG" << endl;
        return;
        }

        //First byte
        msg.sendCode(static_cast<int>(Protocol::ANS_GET_ART));

        bool newsgroupFound = db.get_newsgroup(newsgroupsid);
        if(!newsgroupFound){
                msg.sendCode(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
        }
                        
        Article* article = db.get_article(newsgroupsid, articlesid);

        if(article != nullptr){
                msg.sendCode(static_cast<int>(Protocol::ANS_ACK));
                msg.sendStringParameter(article->getTitle());
                msg.sendStringParameter(article->getAuthor());
                msg.sendStringParameter(article->getContent());

        }else{
                msg.sendCode(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
        }
        //Last byte
        msg.sendCode(static_cast<int>(Protocol::ANS_END));
}

void process_command(std::shared_ptr<Connection>& conn, database_interface& db){
        MessageHandler msg(conn);
        int cmd = msg.recvCode(); //Läser första byten, t.ex. COM_LIST_ART

        switch (cmd) {
                case 1: //Protocol::COM_LIST_NG
                    list_newsgroups(msg, db);
                    break;
                case 2: //Protocol::COM_CREATE_NG
                    create_newsgroup(msg, db);
                    break;
                case 3: //Protocol::COM_DELETE_NG
                    delete_newsgroup(msg, db);
                    break;
                case 4://Protocol::COM_LIST_ART
                    list_articles(msg, db);
                    break;
                case 5: //Protocol::COM_CREATE_ART
                    create_article(msg, db);
                    break;
                case 6: //Protocol::COM_DELETE_ART
                    delete_article(msg, db);
                    break;
                case 7: //Protocol::COM_GET_ART
                    get_article(msg, db);
                    break;
                default:
                    std::cerr << "Unknown command received: " << cmd << std::endl;
                    break;
                }
        }
}
/*
COM_LIST_NG    = 1, // list newsgroups
COM_CREATE_NG  = 2, // create newsgroup
COM_DELETE_NG  = 3, // delete newsgroup
COM_LIST_ART   = 4, // list articles
COM_CREATE_ART = 5, // create article
COM_DELETE_ART = 6, // delete article
COM_GET_ART    = 7, // get article
COM_END        = 8, // command end

Answer codes, server -> client 
ANS_LIST_NG    = 20, // answer list newsgroups
ANS_CREATE_NG  = 21, // answer create newsgroup
ANS_DELETE_NG  = 22, // answer delete newsgroup
ANS_LIST_ART   = 23, // answer list articles
ANS_CREATE_ART = 24, // answer create article
ANS_DELETE_ART = 25, // answer delete article
ANS_GET_ART    = 26, // answer get article
ANS_END        = 27, // answer end
ANS_ACK        = 28, // acknowledge
ANS_NAK        = 29, // negative acknowledge
*/
        
int main(int argc, char* argv[])        
{
        auto server = init(argc, argv);
        //skapar database objekt! saknas!!
        while (true) {
            serve_one(server, db);
        }
        return 0;
}
