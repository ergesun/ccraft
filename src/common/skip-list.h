/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <cstdint>
#include <utility>
#include <functional>

namespace ccraft {
namespace sl {
const uint32_t SKIPLIST_MAX_LEVEL = 32 - 1;
const uint32_t SKIPLIST_DEFAULT_LEVEL = 16 - 1;
const uint32_t SKIPLIST_MIN_LEVEL = 4 - 1;

template <typename T, typename _Compare = std::less<T>>
class skip_list {
private:
    class Node;
public:
    class iterator {
        friend class skip_list;
    public:
        iterator(const iterator &iter) {
            this->m_pCur = iter.m_pCur;
            this->m_pHead = iter.m_pHead;
        }

        iterator& operator=(const iterator &iter) {
            this->m_pCur = iter.m_pCur;
            this->m_pHead = iter.m_pHead;

            return *this;
        }

        T& operator*() const {
            return m_pCur->Data;
        }

        bool operator==(const iterator& src) const {
            return (m_pCur == src.m_pCur);
        }

        bool operator!=(const iterator& src) const {
            return (m_pCur != src.m_pCur);
        }

        // ++A
        iterator& operator++() {
            if (m_pCur == nullptr) {
                return *this;
            }

            m_pCur = m_pCur->Next[0];
            return *this;
        }

        // A++
        iterator operator++(int) {
            auto tmp = *this;
            operator++();
            return tmp;
        }

    private:
        iterator(skip_list *psl) : m_pHead(psl->m_pHead), m_pCur(psl->m_pHead->Next[0]) {}
        iterator(Node *ph, Node *pn) : m_pHead(ph), m_pCur(pn) {}

        // disable now!
        // --A
        iterator& operator--() {
            return *this;
        }
        // A--
        iterator operator--(int) { return operator--(); }

        Node        *m_pHead = nullptr;
        Node        *m_pCur  = nullptr;
    };

    friend class iterator;
public:
    skip_list() : m_iMaxLevel(SKIPLIST_DEFAULT_LEVEL) { init(); }
    explicit skip_list(int maxLevel) : m_iMaxLevel(maxLevel - 1) { init(); }
    ~skip_list() = default;

    void insert(T t) {
        Node *targetPrev[m_iMaxLevel];
        find_previous_levels_nodes(t, targetPrev);
        auto q = targetPrev[0]->Next[0];
        // equals data, update data value
        if (q && !m_cmp(q->Data, t) && !m_cmp(t, q->Data)) {
            q->Data = t;
            return;
        }

        auto nodeLevel = rand_level();
        // update level and prev
        if (nodeLevel > m_iCurLevel) {
            for (auto i = m_iCurLevel; i <= nodeLevel; ++i) {
                targetPrev[i] = m_pHead;
            }

            m_iCurLevel = nodeLevel;
        }

        auto pNode = create_node(nodeLevel, t);
        // insert node for every level
        for (auto i = 0; i <= nodeLevel; ++i) {
            pNode->Next[i] = targetPrev[i]->Next[i];
            targetPrev[i]->Next[i] = pNode;
        }
    }

    bool erase(const T &t) {
        Node *targetPrev[m_iMaxLevel];
        find_previous_levels_nodes(t, targetPrev);
        auto q = targetPrev[0]->Next[0];
        // cannot find
        if (!q || (m_cmp(q->Data, t) || m_cmp(t, q->Data))) {
            return false;
        }

        for (auto i = 0; i <= m_iCurLevel; ++i) {
            if (targetPrev[i]->Next[i] == q) {
                targetPrev[i]->Next[i] = q->Next[i];
            } else {
                break;
            }
        }

        delete q;

        for (auto i = m_iCurLevel; i > 0; --i) {
            if (nullptr == m_pHead->Next[i]) {
                --m_iCurLevel;
            }
        }

        return true;
    }

    void clear() {
        Node *p, *q = nullptr;
        p = m_pHead;
        q = p->Next[0];
        while (q != nullptr) {
            p = q->Next[0];
            delete q;
            q = nullptr;
            if (p != nullptr) {
                q = p->Next[0];
            }
        }

        for (auto i = 0; i <= m_iMaxLevel; ++i) {
            m_pHead->Next[i] = nullptr;
        }
    }

    std::pair<bool, T> get(const T &t) {
        auto node = find_node(t);
        if (node == nullptr) {
            T defaultRs;
            return std::make_pair(false, defaultRs);
        } else {
            return std::make_pair(true, node->Data);
        }
    }

    bool contains(const T &t) {
        auto node = find_node(t);
        return node != nullptr;
    }

    iterator begin() {
        return iterator(this);
    }

    iterator& end() {
        return s_end;
    }

private:
    struct Node {
        friend class iterator;
        T            Data;
        Node        *Next[1];
    };

private:
    void init() {
        srand(time(nullptr));
        m_iMaxLevel = m_iMaxLevel < SKIPLIST_MAX_LEVEL ? SKIPLIST_MIN_LEVEL : m_iMaxLevel;
        T defaultData;
        m_pHead = create_node(m_iMaxLevel, defaultData);
    }

    uint32_t rand_level() {
        uint32_t l = 0;
        while ((rand() % 12) < 4 && l < m_iMaxLevel) {
            ++l;
        }

        return l;
    }

    Node* create_node(uint32_t level, T &data) {
        auto pNode = (Node*)calloc(1, sizeof(Node) + level * sizeof(Node*));
        pNode->Data = data;
        return pNode;
    }

    void find_previous_levels_nodes(const T &t, Node* targetPrev[]) {
        Node *p, *q = nullptr;
        p = m_pHead;
        // find insert previous node for every level from top to bottom.
        for (auto i = m_iCurLevel; i >= 0; --i) {
            while ((q = p->Next[i]) && m_cmp(q->Data, t)) {
                p = q;
            }

            targetPrev[i] = p;
        }
    }

    Node* find_node(const T &t) {
        Node *p, *q = nullptr;
        p = m_pHead;
        // find insert previous node for every level from top to bottom.
        for (auto i = m_iCurLevel; i >= 0; --i) {
            while ((q = p->Next[i]) && m_cmp(q->Data, t)) {
                p = q;
            }

            if ((q != nullptr) && !m_cmp(t, q->Data)) {
                return q;
            }
        }

        return nullptr;
    }

private:
    _Compare    m_cmp;
    Node       *m_pHead         = nullptr;
    int         m_iCurLevel     = 0;
    int         m_iMaxLevel     = -1;
    static iterator s_end;
};

template <typename T, typename L>
typename skip_list<T, L>::iterator skip_list<T, L>::s_end(nullptr, nullptr);
}
}

#endif /* SKIP_LIST_H */
