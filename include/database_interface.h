#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H

#include <string>
#include <vector>
#include <optional>
#include "article.h"
#include "newsgroup.h"

class database_interface{
    public:
        virtual ~database_interface() = default;

        virtual std::vector<Newsgroup> list_newsgroups() const = 0;

        virtual bool create_newsgroup(std::string group_name) = 0;

        virtual bool delete_newsgroup(int group_id) = 0;

        virtual bool create_article(int group_id, std::string title, std::string author, std::string text) = 0;

        virtual std::vector<std::pair<int, std::string>> list_articles(int group_id) const = 0;

        virtual const std::optional<Article> get_article(int group_id, int article_id) const = 0;

        virtual bool delete_article(int group_id, int article_id) = 0;

        virtual bool newsgroup_exists(int groupid) = 0;
};

#endif
