#ifndef ARTICLE_H
#define ARTICLE_H
#include <string>

class Article {
private:
    std::string title;
    std::string author;
    std::string content;

public:
    Article() = default;
    Article(const std::string& title,
            const std::string& author,
            const std::string& content);

    std::string getTitle() const;
    std::string getAuthor() const;
    std::string getContent() const;

};
#endif