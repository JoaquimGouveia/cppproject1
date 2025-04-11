#ifndef DATABASE_MEMORY_H
#define DATABASE_MEMORY_H

#include "database_interface.h"
#include "newsgroup.h"
#include <string>
#include <vector>


class database_memory: public database_interface{
    public:
        database_memory();

        ~database_memory() = default;

        std::vector<Newsgroup> list_newsgroup() const;

        bool delete_newsgroup(int group_id);

        bool create_article(int group_id, std::string title, std::string author, std::string text);

        std::vector<std::pair<int, Article>> list_articles(int group_id) const;

        Article* get_article(int group_id, int article_id) const;

        bool delete_article(int group_id, int article_id);

        bool get_newsgroup(int groupid);
};
#endif