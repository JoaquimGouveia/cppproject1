#ifndef DATABASE_MEMORY_H
#define DATABASE_MEMORY_H

#include "database_interface.h"
#include "newsgroup.h"
#include <string>
#include <vector>
#include <map>


class database_memory: public database_interface{
    public:
        database_memory();

        ~database_memory() = default;

        std::vector<Newsgroup> list_newsgroups() const;

        bool create_newsgroup(std::string group_name);

        bool delete_newsgroup(int group_id);

        bool create_article(int group_id, std::string title, std::string author, std::string text);

        std::vector<std::pair<int, std::string>> list_articles(int group_id) const;

        const std::optional<Article> get_article(int group_id, int article_id) const;

        bool delete_article(int group_id, int article_id);

        bool newsgroup_exists(int groupid);

        private:
            int group_id = 1;
            std::map<int, Newsgroup> groups;
};
#endif