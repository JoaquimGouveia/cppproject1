#include "newsgroup.h"
#include "article.h"

Newsgroup::Newsgroup(int id, const std::string& name)
    : id(id), name(name) {}

//Lägger till en ny artikel till mappen (newsgroup)   
int Newsgroup::addArticle(const std::string& title, const std::string& author, const std::string& content) {
    articles[nextArticleId] = Article(title, author, content);
    return nextArticleId++;
}
//Lägger till en ny artikel till mappen (newsgroup), givet att artikeln redan var skapad    
int Newsgroup::addArticle(Article c) {
    articles[nextArticleId] = c;
    return nextArticleId++;
}

int Newsgroup::getId() const {
    return id;
}

std::string Newsgroup::getName() const {
    return name;
}

//Tar bort en artikel givet idnummret, returnerar true om en artikel togs bort, annars false
bool Newsgroup::deleteArticle(int articleId) {
    return articles.erase(articleId) > 0;
}

bool Newsgroup::deleteArticle(const std::string& title) {
    for (auto it = articles.begin(); it != articles.end(); ++it) {
        if (it->second.getTitle() == title) {
            articles.erase(it);
            return true;
        }
    }
    return false;
}

const Article* Newsgroup::getArticle(int articleId) const {
    auto it = articles.find(articleId);
    if (it != articles.end()) {
        return &it->second;
    }
    return nullptr;
}

const Article* Newsgroup::getArticle(const std::string& title) const {
    for (auto& [id, article] : articles) {
        if (article.getTitle() == title) {
            return &article;
        }
    }
    return nullptr;
}

std::vector<std::pair<int, std::string>> Newsgroup::listArticles() const {
    std::vector<std::pair<int, std::string>> result;
    for (const auto& [id, article] : articles) {
        result.emplace_back(id, article.getTitle());
    }
    return result;
}