#ifndef DATABASE_DISK_H
#define DATABASE_DISK_H

#include "database_interface.h"
#include "newsgroup.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>

class database_disk: public database_interface {
    public:
        database_disk();
        ~database_disk() = default;

        std::vector<Newsgroup> list_newsgroups() const override;
        bool create_newsgroup(std::string group_name) override;
        bool delete_newsgroup(int group_id) override;
        bool create_article(int group_id, std::string title, std::string author, std::string text) override;
        std::vector<std::pair<int, std::string>> list_articles(int group_id) const override;
        const std::optional<Article> get_article(int group_id, int article_id) const override;
        bool delete_article(int group_id, int article_id) override;
        bool newsgroup_exists(int group_id) override;
        int find_unused_id() const;
        int find_unused_id(std::vector<bool> id_uses) const;

    private:
        std::filesystem::path root_path = "../src/root";
        std::vector<bool> id_uses;
        std::vector<std::string> newsgroup_names;
};

#endif // DATABASE_DISK_H