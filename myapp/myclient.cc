#include <iostream>
#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"
#include "protocol.h"
#include <string>

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::shared_ptr;

/* Command codes, client -> server */
/*
COM_LIST_NG    = 1, // list newsgroups
COM_CREATE_NG  = 2, // create newsgroup
COM_DELETE_NG  = 3, // delete newsgroup
COM_LIST_ART   = 4, // list articles
COM_CREATE_ART = 5, // create article
COM_DELETE_ART = 6, // delete article
COM_GET_ART    = 7, // get article
COM_END        = 8, // command end
*/

void printMenu() {
    cout << "1. List newsgroups" << endl;
    cout << "2. Create newsgroup" << endl;
    cout << "3. Delete newsgroup" << endl;
    cout << "4. List articles" << endl;
    cout << "5. Create article" << endl;
    cout << "6. Delete article" << endl;
    cout << "7. Get article" << endl;
    cout << "8. Exit" << endl;
}

void list_newsgroups(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_LIST_NG));
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));
    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_LIST_NG)) {
        int num_ng = messageHandler.recvIntParameter();
        cout << "Number of newsgroups: " << num_ng << endl;
        for (int i = 0; i < num_ng; ++i) {
            int ng_id = messageHandler.recvIntParameter();
            string ng_name = messageHandler.recvStringParameter();
            cout << "Newsgroup ID: " << ng_id << ", Name: " << ng_name << endl;
        }
    } 
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

void create_newsgroup(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_CREATE_NG));
    string newsgroup_name;
    cout << "Enter newsgroup name: ";
    cin >> newsgroup_name; // Should we check if the name is valid?
    messageHandler.sendStringParameter(newsgroup_name);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));
    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_CREATE_NG)) { // Do I even have to check if this is the code received?
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            cout << "Newsgroup created successfully." << endl;
        } else {
            int error_code = messageHandler.recvCode();
            if (error_code == static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS)) {
                cout << "Error: Newsgroup already exists." << endl;
            } //else {
            //    cout << "Error: Unknown error occurred." << endl;
            //}
        }
    } 
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

void delete_newsgroup(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_DELETE_NG));
    int ng_id;
    cout << "Enter newsgroup ID to delete: ";
    cin >> ng_id;
    messageHandler.sendIntParameter(ng_id);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));
    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_DELETE_NG)) {
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            cout << "Newsgroup deleted successfully." << endl;
        } else {
            int error_code = messageHandler.recvCode();
            if (error_code == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                cout << "Error: Newsgroup does not exist." << endl;
            } //else {
            //    cout << "Error: Unknown error occurred." << endl;
            //}
        }
    }
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

void list_articles(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_LIST_ART));
    int ng_id;
    cout << "Enter newsgroup ID to list articles: ";
    cin >> ng_id;
    messageHandler.sendIntParameter(ng_id);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));
    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_LIST_ART)) {
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            int num_articles = messageHandler.recvIntParameter();
            for (int i = 0; i < num_articles; ++i) {
                int article_id = messageHandler.recvIntParameter();
                cout << "Article ID: " << article_id << endl;
                string article_name = messageHandler.recvStringParameter();
                cout << "Article Name: " << article_name << endl;
            }
        } else if (ack_code == static_cast<int>(Protocol::ANS_NAK)) {
            int code = messageHandler.recvCode();
            if (code == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                cout << "Error: Newsgroup does not exist." << endl;
            } //else {
              //  cout << "Error: Unknown error occurred." << endl;
            //}
        }    
    }
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }    
}

void create_article(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_CREATE_ART));
    int ng_id;
    string ng_name, article_name, article_author, article_body;

    // Retrieve newsgroup ID from user
    cout << "Enter newsgroup ID to create article: ";
    cin >> ng_id;
    messageHandler.sendIntParameter(ng_id); // We should have a check for validity here

    // Retrieve article name from user
    cout << "Enter article name: ";
    cin >> article_name; 
    messageHandler.sendStringParameter(article_name);

    // Retrieve article author from user
    cout << "Enter article author: ";
    cin >> article_author;
    messageHandler.sendStringParameter(article_author);
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer before reading the article body
    // Retrieve article body from user
    cout << "Enter article body: ";
    while (true) {
        string line;
        std::getline(cin, line);
        if (line.empty()) break; // Stop reading when an empty line is entered
        article_body += line + "\n"; // Append the line to the article body
    }
    if (!article_body.empty() && article_body.back() == '\n') {
        article_body.pop_back(); // Remove the last newline character
    }
    messageHandler.sendStringParameter(article_body);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));

    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_CREATE_ART)) {
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            cout << "Article created successfully." << endl;
        } else {
            int error_code = messageHandler.recvCode();
            if (error_code == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                cout << "Error: Newsgroup does not exist." << endl;
            } //else {
            //    cout << "Error: Unknown error occurred." << endl;
            //}
        }
    }
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

void delete_article(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_DELETE_ART));
    int ng_id;
    cout << "Enter newsgroup ID to delete article: ";
    cin >> ng_id;
    messageHandler.sendIntParameter(ng_id);
    int article_id;
    cout << "Enter article ID to delete: ";
    cin >> article_id;
    messageHandler.sendIntParameter(article_id);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));
    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_DELETE_ART)) {
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            cout << "Article deleted successfully." << endl;
        } else {
            int error_code = messageHandler.recvCode();
            if (error_code == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                cout << "Error: Newsgroup does not exist." << endl;
            } else if (error_code == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)) {
                cout << "Error: Article does not exist." << endl;
            } //else {
            //    cout << "Error: Unknown error occurred." << endl;
            //}
        }
    }
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

void get_article(MessageHandler& messageHandler) {
    messageHandler.sendCode(static_cast<int>(Protocol::COM_GET_ART));
    int ng_id;
    cout << "Enter newsgroup ID to get article: ";
    cin >> ng_id;
    messageHandler.sendIntParameter(ng_id);
    int article_id;
    cout << "Enter article ID to get: ";
    cin >> article_id;
    messageHandler.sendIntParameter(article_id);
    messageHandler.sendCode(static_cast<int>(Protocol::COM_END));

    int code = messageHandler.recvCode();
    if (code == static_cast<int>(Protocol::ANS_GET_ART)) {
        int ack_code = messageHandler.recvCode();
        if (ack_code == static_cast<int>(Protocol::ANS_ACK)) {
            string article_name = messageHandler.recvStringParameter();
            cout << "Article Name: " << article_name << endl;
            string article_author = messageHandler.recvStringParameter();
            cout << "Article Author: " << article_author << endl;
            string article_body = messageHandler.recvStringParameter();
            cout << "Article Body: " << article_body << endl;
        } else {
            int error_code = messageHandler.recvCode();
            if (error_code == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)) {
                cout << "Error: Newsgroup does not exist." << endl;
            } else if (error_code == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)) {
                cout << "Error: Article does not exist." << endl;
            } //else {
            //    cout << "Error: Unknown error occurred." << endl;
            //}
        }
    }
    int end_code = messageHandler.recvCode();
    if (end_code != static_cast<int>(Protocol::ANS_END)) {
        cerr << "Error: Expected end code, got " << end_code << endl;
    }
}

int app(MessageHandler messageHandler) {
    printMenu();
    int command;
    while (true) {
        cout << "Enter your choice:" << endl;
        cout << "Press 0 for help:" << endl;
        cin >> command;
        try {
            switch (command) {
                case 1:
                    list_newsgroups(messageHandler);
                    break;
                case 2:
                    create_newsgroup(messageHandler);
                    break;
                case 3:
                    delete_newsgroup(messageHandler);
                    break;
                case 4:
                    list_articles(messageHandler);
                    break;
                case 5:
                    create_article(messageHandler);
                    break;
                case 6:
                    delete_article(messageHandler);
                    break;
                case 7:
                    get_article(messageHandler);
                    break;
                case 8:
                    cout << "Exiting..." << endl;
                    return 0;
                case 0:
                    printMenu();
                    break;
                default:
                    cout << "Invalid command. Please try again." << endl;
                    break;
            }
        } catch (ConnectionClosedException& e) {
            TODO: // Handle connection closed exception
            return 0;
        }
    }
    return 0;
}


shared_ptr<Connection> init(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: myclient host-name port-number" << endl;
        exit(1);
    }

    int port = -1;
    try {
        // Convert the second argument to an integer, i.e: port number
        port = std::stoi(argv[2]);
    } catch (std::exception& e) {
        cerr << "Wrong port number. " << e.what() << endl;
        exit(2);
    }

    // Establish a connection to the server using the provided host and port
    auto connection = std::make_shared<Connection>(argv[1], port);
    cout << "Trying to connect to host: " << argv[1] << " on port: " << port << endl;
    if (!connection->isConnected()) {
        cerr << "Connection attempt failed" << endl;
        exit(3);
    }

    return connection;
}

int main (int argc, char* argv[])
{
    shared_ptr<Connection> conn = init(argc, argv);
    MessageHandler messageHandler(conn);
    int result = app(messageHandler);
    return result;
}

// SHOULD I MAKE SURE ALL INPUT IS VALID BEFORE SENDING ANYTHING TO THE SERVER????????