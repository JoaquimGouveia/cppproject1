#include "article.h"

Article::Article(const std::string& title,
                 const std::string& author,
                 const std::string& content)
    : title(title), author(author), content(content) {}

std::string Article::getTitle() const {
    return title;
}
    
std::string Article::getAuthor() const {
    return author;
}

std::string Article::getContent() const {
    return content;
} 