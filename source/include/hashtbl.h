// @author: Jonas, Neylane e Selan.

#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include <algorithm>         // copy, find_if, for_each
#include <cmath>             // sqrt
#include <forward_list>      // forward_list
#include <initializer_list>  // std::initializer_list
#include <iostream>          // cout, endl, ostream
#include <iterator>          // std::begin(), std::end()
#include <memory>            // std::unique_ptr
#include <utility>           // std::pair

namespace ac  // Associative container
{
template <class KeyType, class DataType>
struct HashEntry {
    KeyType m_key;    //! Data key
    DataType m_data;  //! The data

    // Regular constructor.
    HashEntry(KeyType kt_, DataType dt_) : m_key{kt_}, m_data{dt_} {}

    friend std::ostream& operator<<(std::ostream& os_, const HashEntry& he_) { return os_ << he_.m_data; }
};

template <class KeyType, class DataType, class KeyHash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType> >
class HashTbl {
   public:
    using size_type = std::size_t;
    using entry_type = HashEntry<KeyType, DataType>;
    using list_type = std::forward_list<entry_type>;

   private:
    size_type m_size;                      //!< Tamanho da tabela.
    size_type m_count;                     //!< Numero de elementos na tabela.
    float m_load_factor;                   //!< Fator da tabela.
    std::unique_ptr<list_type[]> m_table;  //!< Ponteiro para tabela.

    static const short DEFAULT_SIZE = 10;

   public:
    explicit HashTbl(size_type table_sz_ = DEFAULT_SIZE);
    HashTbl(const HashTbl&);
    HashTbl(const std::initializer_list<entry_type>&);
    HashTbl& operator=(const HashTbl&);
    HashTbl& operator=(const std::initializer_list<entry_type>&);

    virtual ~HashTbl();

    bool insert(const KeyType&, const DataType&);
    bool retrieve(const KeyType&, DataType&) const;
    bool erase(const KeyType&);
    void clear();
    bool empty() const;
    inline size_type size() const { return m_count; };
    DataType& at(const KeyType&);
    DataType& operator[](const KeyType&);
    size_type count(const KeyType&) const;
    float max_load_factor() const;
    void max_load_factor(float mlf);

    friend std::ostream& operator<<(std::ostream& os_, const HashTbl& ht_) {
        os_ << "{ ";
        for (std::size_t index{0}; index < ht_.m_size; index++) {
            for (const auto& e : ht_.m_table[index]) os_ << e << ", ";
        }
        os_ << "}";

        return os_;
    }

   private:
    static size_type find_next_prime(size_type);
    static bool is_prime(const size_type&);
    void rehash(void);
};

}  // namespace ac
#include "hashtbl.inl"
#endif
