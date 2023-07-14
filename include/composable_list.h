#ifndef COMPOSABLE_LIST_H
#define COMPOSABLE_LIST_H

#include <functional>
#include <list>

template <typename T>
struct Composable_Function_Holder {
    std::function<T(T&)> data;

    Composable_Function_Holder(const std::function<T(T)> & func) : data(func) {}

    T operator()(T&v) {
        return data(v);
    }
};

template <>
struct Composable_Function_Holder<void> {
    std::function<void()> data;

    Composable_Function_Holder(const std::function<void()> & func) : data(func) {}

    void operator()() {
        data();
    }
};

template <typename T>
class Composable_List final {
    std::list<Composable_Function_Holder<T>*> * list = nullptr;
    typename std::list<Composable_Function_Holder<T>*>::iterator iterator;

    Composable_List(Composable_List<T> * parent) : list(parent->list) {}

    public:
    
    Composable_List() {
        list = new std::list<Composable_Function_Holder<T>*>();
        iterator = list->insert(list->begin(), nullptr);
    }

    template <typename Item>
    Composable_List(const Item & f) {
        list = new std::list<Composable_Function_Holder<T>*>();
        iterator = list->insert(list->begin(), new Composable_Function_Holder<T>(f));
    }

    template <typename Item>
    Composable_List<T> * before(const Item & f) {
        auto a = new Composable_List<T>(this);
        a->iterator = list->insert(iterator, new Composable_Function_Holder<T>(f));
        return a;
    }

    template <typename Item>
    Composable_List<T> * after(const Item & f) {
        auto a = new Composable_List<T>(this);
        auto copy = iterator;
        copy++;
        a->iterator = list->insert(copy, new Composable_Function_Holder<T>(f));
        return a;
    }

    void visit(const std::function<void(Composable_Function_Holder<T> * n)> & f) {
        for (auto current = iterator, end = list->end(); current != end; current++) {
            f(*current);
        }
    }

    void visitFromStart(const std::function<void(Composable_Function_Holder<T> * n)> & f) {
        for (auto current = list->begin(), end = list->end(); current != end; current++) {
            f(*current);
        }
    }
};

template <typename T>
class Composable_List_COW final {
    std::list<Composable_Function_Holder<T>*> * list = nullptr;
    std::list<uint64_t> * list_ids = nullptr;
    uint64_t id = 0;

    Composable_List_COW<T> * clone() {
        auto a = new Composable_List_COW<T>();
        a->list->clear();
        a->list_ids->clear();
        for (auto current = list->begin(), end = list->end(); current != end; current++) {
            a->list->insert(a->list->end(), *current);
        }
        for (auto current = list_ids->begin(), end = list_ids->end(); current != end; current++) {
            a->list_ids->insert(a->list_ids->end(), *current);
        }
        return a;
    }

    public:
    
    Composable_List_COW() {
        list = new std::list<Composable_Function_Holder<T>*>();
        list->insert(list->begin(), nullptr);
        list_ids = new std::list<uint64_t>();
        list_ids->insert(list_ids->begin(), id);
    }

    template <typename Item>
    Composable_List_COW(const Item & f) {
        list = new std::list<Composable_Function_Holder<T>*>();
        list->insert(list->begin(), new Composable_Function_Holder<T>(f));
        list_ids = new std::list<uint64_t>();
        list_ids->insert(list_ids->begin(), id);
    }

    template <typename Item>
    Composable_List_COW<T> * before(const Item & f) {
        auto a = clone();
        a->list->push_front(new Composable_Function_Holder<T>(f));
        a->id = id + 1;
        a->list_ids->push_front(a->id);
        return a;
    }

    template <typename Item>
    Composable_List_COW<T> * after(const Item & f) {
        auto a = clone();
        a->list->push_back(new Composable_Function_Holder<T>(f));
        a->id = id + 1;
        a->list_ids->push_back(a->id);
        return a;
    }

    template <typename Item>
    Composable_List_COW<T> * before(Composable_List_COW<T> * list, const Item & f) {
        auto a = new Composable_List_COW<T>();
        a->list->clear();
        a->list_ids->clear();

        a->id = id + 1;

        uint64_t index = 0;
        uint64_t target_index = 0;

        for (auto current = list_ids->begin(), end = list_ids->end(); current != end; current++, index++) {
            if (*current == list->id) {
                target_index = index;
                a->list_ids->insert(a->list_ids->end(), a->id);
            }
            a->list_ids->insert(a->list_ids->end(), *current);
        }

        index = 0;

        for (auto current = this->list->begin(), end = this->list->end(); current != end; current++, index++) {
            if (index == target_index) {
                a->list->insert(a->list->end(), new Composable_Function_Holder<T>(f));
            }
            a->list->insert(a->list->end(), *current);
        }

        return a;
    }

    template <typename Item>
    Composable_List_COW<T> * after(Composable_List_COW<T> * list, const Item & f) {
        auto a = new Composable_List_COW<T>();
        a->list->clear();
        a->list_ids->clear();

        a->id = id + 1;

        uint64_t index = 0;
        uint64_t target_index = 0;

        for (auto current = list_ids->begin(), end = list_ids->end(); current != end; current++, index++) {
            a->list_ids->insert(a->list_ids->end(), *current);
            if (*current == list->id) {
                target_index = index;
                a->list_ids->insert(a->list_ids->end(), a->id);
            }
        }

        index = 0;

        for (auto current = this->list->begin(), end = this->list->end(); current != end; current++, index++) {
            a->list->insert(a->list->end(), *current);
            if (index == target_index) {
                a->list->insert(a->list->end(), new Composable_Function_Holder<T>(f));
            }
        }

        return a;
    }

    void visit(const std::function<void(Composable_Function_Holder<T> * n)> & f) {
        bool can_call = false;
        auto current = list->begin();
        auto current_id = list_ids->begin();
        auto end = list->end();
        while (current != end) {
            if (can_call) {
                f(*current);
            } else {
                if (*current_id == id) {
                    can_call = true;
                    f(*current);
                } else {
                    current_id++;
                }
            }
            current++;
        }
    }

    void visitFromStart(const std::function<void(Composable_Function_Holder<T> * n)> & f) {
        for (auto current = list->begin(), end = list->end(); current != end; current++) {
            f(*current);
        }
    }
};

#endif
