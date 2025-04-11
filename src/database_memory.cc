#include "database_memory.h"
#include <vector>
#include <iostream>
#include "article.h"

using namespace std;

database_memory::database_memory(){};

database_memory::~database_memory(){};

vector<Newsgroup> database_memory::list_newsgroup() const{
    vector<Newsgroup> list;
    list.reserve(groups.size());
    for(const auto& [id, group] : groups){
        list.push_back(group);
    }
}

bool database_memory::create_newsgroup(string group_name){
    for(auto g : groups){
        if(g.second.getName() == group_name){
            return false;
        }
    }
    
    Newsgroup group(group_id, group_name);
    groups.emplace(group_id, group);
    group_id++;
    return true;
}

bool database_memory::delete_newsgroup(int group_id){
    if(groups.erase(group_id) == 1){
        return true;
    } else {
        return false;
    }
}

bool database_memory::create_article(int group_id, string title, string author, string text){
    try{
        groups.at(group_id).addArticle(title, author, text);
        return true;
    } catch(out_of_range &oor){
        return false;
    }
}

vector<pair<int, string>> database_memory::list_articles(int group_id) const{
    vector<pair<int, string>> articles;
    try{
        articles = groups.at(group_id).listArticles();
        return articles;
     }catch (out_of_range &oor){
        return articles;
    }
}

Article* database_memory::get_article(int group_id, int article_id) const{
    try{
        const auto& group = groups.at(group_id);
        return group.getArticle(article_id);
    }catch(const std::out_of_range& oor){
        return nullptr;
    }
}

bool database_memory::delete_article(int group_id, int article_id){
    try {
        auto& group = groups.at(group_id);
        return group.deleteArticle(article_id);
    } catch (const std::out_of_range& oor) {
        return false;
    }
}


bool database_memory::get_newsgroup(int groupid){
    try {
        auto& group = groups.at(group_id);
        return true;
    } catch (const std::out_of_range& oor) {
        return false;
    }
}
