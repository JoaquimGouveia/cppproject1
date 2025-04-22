#include "database_disk.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "article.h"

using namespace std;

database_disk::database_disk() : id_uses(1000000, false) {
    vector<Newsgroup> list = list_newsgroups();

    // Initialize the id_uses vector and newsgroup names based on existing newsgroups
    newsgroup_names.resize(1000000, "");
    for (const auto& group : list) {
        int id = group.getId();
        id_uses[id] = true; // Mark the newsgroup as existing
        newsgroup_names[id] = group.getName(); // Store the newsgroup name
    }
}

vector<Newsgroup> database_disk::list_newsgroups() const{
    vector<Newsgroup> list;
    for (const auto& dir : filesystem::directory_iterator(root_path)) {
        if (!dir.is_directory()) {
            continue; // Skip non-directories like .gitkeep
        }
        string newsgroup = dir.path().filename().string();
        // The name of the directory will be <ID>_<NewsgroupName>
        auto separator_pos = newsgroup.find('_');
        int id = stoi(newsgroup.substr(0, separator_pos));
        string name = newsgroup.substr(separator_pos + 1);
        Newsgroup group(id, name);
        list.push_back(group);
    }
    return list;
}

bool database_disk::create_newsgroup(string group_name){
    auto newsgroups = list_newsgroups();
    for (const auto& group : newsgroups) {
        if (group.getName() == group_name) {
            return false; // Newsgroup already exists
        }
    }
    // Create a new directory for the newsgroup
    int id = find_unused_id();
    id_uses[id] = true; // Mark the newsgroup as existing
    newsgroup_names[id] = group_name; // Store the newsgroup name
    // Create the directory with the format <ID>_<NewsgroupName>
    filesystem::create_directory(root_path / (to_string(id) + "_" + group_name));
    return true;
}

bool database_disk::delete_newsgroup(int group_id){
    if (!id_uses[group_id]) {
        return false; // Newsgroup does not exist
    }
    // Remove the directory for the newsgroup
    vector<Newsgroup> list = list_newsgroups();
    for (const auto& group : list) {
        if (group.getId() == group_id) {
            filesystem::remove_all(root_path / (to_string(group_id) + "_" + group.getName()));
            id_uses[group_id] = false; // Mark the newsgroup as deleted
            newsgroup_names[group_id].clear(); // Clear the newsgroup name
        }
    }
    return true; // Newsgroup deleted successfully
}

bool database_disk::create_article(int group_id, string title, string author, string text){
    if (!id_uses[group_id]) {
        return false; // Newsgroup does not exist
    }

    // Get the newsgroup name
    string newsgroup_name = newsgroup_names[group_id];

    // Get the next available article ID
    vector<bool> id_uses = vector<bool>(1000, false);
    vector<pair<int, string>> articles = list_articles(group_id);
    for (const auto& article : articles) {
        id_uses[article.first] = true; // Mark the article as existing
    }
    int article_id = find_unused_id(id_uses); // Find the next available ID
    
    // Construct path: root/group_id_newsgroup_name/article_id.txt
    filesystem::path article_path = root_path / (to_string(group_id) + "_" + newsgroup_name) / (to_string(article_id) + ".txt");

    // Write title, author, and text to the file 
    ofstream article_file(article_path);
    if (!article_file) return false; // Check if the file opened successfully
    article_file << title << '\n' << author << '\n' << text << '\n'; // Write title, author, and text to the file
    article_file.close(); // Close the file

    return true; // Article created successfully
}

vector<pair<int, string>> database_disk::list_articles(int group_id) const{
    vector<pair<int, string>> articles;

    // If we get here it means that the newsgroup exists
    // Get the newsgroup name
    string newsgroup_name = newsgroup_names[group_id];

    // Construct path: root/groupID_newsgroup_name
    filesystem::path group_path = root_path / (to_string(group_id) + "_" + newsgroup_name);

    // Iterate through the files in the directory
    for (const auto& dir_entry : filesystem::directory_iterator(group_path)) {
        int article_id = stoi(dir_entry.path().filename().stem().string()); // Get the article ID from the filename
        ifstream article_file(dir_entry.path());
        if (article_file) {
            string title;
            getline(article_file, title); // Read the title from the file
            articles.push_back(make_pair(article_id, title)); // Add the article ID and title to the list
        }
    }
    return articles; // Return the list of articles
}


const std::optional<Article> database_disk::get_article(int group_id, int article_id) const{
    // If we get here it means that the newsgroup must exit
    // Get the newsgroup name
    string newsgroup_name = newsgroup_names[group_id];

    // Construct path: root/groupID_newsgroup_name/article_id.txt
    filesystem::path article_path = root_path / (to_string(group_id) + "_" + newsgroup_name) / (to_string(article_id) + ".txt");
    ifstream article_file(article_path);
    if (!article_file.is_open()) {
        return std::nullopt; // Article not found
    }

    string title, author, text;
    getline(article_file, title); // Read the title from the file
    getline(article_file, author); // Read the author from the file

    // Read the rest of the file as the article body
    vector<string> body_lines;
    string line;
    while (getline(article_file, line)) {
        body_lines.push_back(line); // Append each line to the body
    }
    article_file.close(); // Close the file

    for (size_t i = 0; i < body_lines.size(); ++i) {
        text += body_lines[i]; // Concatenate the lines to form the article body
        if (i != body_lines.size() - 1) {
            text += '\n'; // Add a newline character between lines
        }
    }

    return Article(title, author, text); // Return the article object
}

bool database_disk::delete_article(int group_id, int article_id){
    // If we get here it means that the newsgroup must exit
    // Get the newsgroup name
    string newsgroup_name = newsgroup_names[group_id];

    // Construct path: root/groupID_newsgroup_name/article_id.txt
    filesystem::path article_path = root_path / (to_string(group_id) + "_" + newsgroup_name) / (to_string(article_id) + ".txt");
    
    // Remove the article file
    if (filesystem::remove(article_path)) {
        return true; // Article deleted successfully
    } else {
        return false; // Article not found or could not be deleted
    }
}

bool database_disk::newsgroup_exists(int groupid){
    // Check if the newsgroup exists
    if (id_uses[groupid]) {
        return true; // Newsgroup exists
    } else {
        return false; // Newsgroup does not exist
    }
}

int database_disk::find_unused_id() const {
    for (size_t i = 0; i < id_uses.size(); ++i) {
        if (!id_uses[i]) {
            return static_cast<int>(i);
        }
    }
    return -1; // No unused ID found
}

int database_disk::find_unused_id(vector<bool> id_uses) const {
    for (size_t i = 0; i < id_uses.size(); ++i) {
        if (!id_uses[i]) {
            return static_cast<int>(i);
        }
    }
    return -1; // No unused ID found
}