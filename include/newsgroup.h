#ifndef NEWSGROUP_H
#define NEWSGROUP_H
#include "article.h"
#include <map>
#include <string>
#include <vector>

class Newsgroup {
private:
    int nextArticleId = 1;

public:
    int id;
    std::string name;
    std::map<int, Article> articles;

    Newsgroup(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    
    int addArticle(const std::string& title, const std::string& author, const std::string& content);
    int addArticle(Article c);

    bool deleteArticle(int articleId);
    bool deleteArticle(const std::string& title);

    const std::optional<Article> getArticle(int articleId) const;
    const std::optional<Article> getArticle(const std::string& title) const;

    std::vector<std::pair<int, std::string>> listArticles() const;

};
#endif