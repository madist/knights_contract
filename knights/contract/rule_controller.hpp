#pragma once

template <class DataType, class TableType>
class rule_controller {
protected:
    TableType table;
    account_name self;
    name rule_name;

public:
    rule_controller(account_name _self, table_name _rule_name)
            : table(_self, _self)
            , self(_self) {
        rule_name.value = _rule_name;
    }

    void create_rules(const std::vector<DataType> &data) {
        require_auth(self);

        for (auto iter = data.begin(); iter != data.end(); ++iter) {
            table.emplace(self, [&](auto &rule) {
                rule = *iter;
            });
        }

        rversion_table table(self, self);
        auto iter = table.find(rule_name);
        if (iter != table.cend()) {
            table.modify(iter, self, [&](auto &target) {
                target.version++;
            });
        } else {
            table.emplace(self, [&](auto &target) {
                target.rule = rule_name;
                target.version = 1;
            });
        }
    }

    void truncate_rules() {
        require_auth(self);

        auto iter = table.begin();
        while (iter != table.cend()) {
            iter = table.erase(iter);
        }
    }

    TableType& get_table() {
        return table;
    }
};