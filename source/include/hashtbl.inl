#include "hashtbl.h"

namespace ac {
/**
 * @brief Constructs empty container.
 *
 * @param sz Hashtable size to use at startup. If not specified, the implementation-defined value is used.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(size_type sz)
    : m_size{find_next_prime(sz)}, m_count{0}, m_load_factor{1.0} {
    m_table = std::make_unique<list_type[]>(m_size);
}

/**
 * @brief Copy constructor. Constructs the container with the copy of the contents of source.
 *
 * @param source Another container to be used as source to initialize the elements of the container.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const HashTbl& source) : HashTbl(source.m_size) {
    m_load_factor = source.m_load_factor;
    for (std::size_t index{0}; index < this->m_size; index++) {
        for (const auto& e : source.m_table[index]) insert(e.m_key, e.m_data);
    }
}

/**
 * @brief Initializer constructor. Constructs the container with the contents of the initializer list ilist.
 *
 * @param ilist Initializer list to initialize the elements of the container.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const std::initializer_list<entry_type>& ilist) : HashTbl() {
    for (auto e : ilist) insert(e.m_key, e.m_data);
}

/**
 * @brief Copy assignment operator. Replaces the contents with a copy of the contents of clone.
 *
 * @param clone Another container to use as data source.
 * @return *this
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(
    const HashTbl& clone) {
    if (this != &clone) {
        this->~HashTbl();

        m_size = clone.m_size;
        m_load_factor = clone.m_load_factor;
        m_table = std::make_unique<list_type[]>(m_size);

        for (std::size_t index{0}; index < m_size; index++) {
            for (auto& e : clone.m_table[index]) insert(e.m_key, e.m_data);
        }
    }

    return *this;
}

/**
 * @brief Assignment initializer list. Replaces the contents with those identified by initializer list ilist.
 *
 * @param ilist Initializer list to use as data source.
 * @return *this
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(
    const std::initializer_list<entry_type>& ilist) {
    this->~HashTbl();

    m_size = find_next_prime(std::distance(ilist.begin(), ilist.end()));
    m_table = std::make_unique<list_type[]>(m_size);
    m_count = 0;
    m_load_factor = 1.f;

    for (const auto& e : ilist) insert(e.m_key, e.m_data);

    return *this;
}

/**
 * @brief Desconstructor. Class destructor that frees memory pointed to by m_table.
 *
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
HashTbl<KeyType, DataType, KeyHash, KeyEqual>::~HashTbl() {
    for (std::size_t index{0}; index < m_size; index++) m_table[index].~forward_list();
    m_table.reset();
}

/**
 * @brief Inserts into the table the information contained in new_data_ and associated with a key_.
 *
 * @param key_ Data key.
 * @param new_data_ The data.
 * @return If the insertion was performed the function successfully, returns true. If the key already exists in the
 * table, it returns false.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::insert(const KeyType& key_, const DataType& new_data_) {
    HashEntry<KeyType, DataType> hashEntry(key_, new_data_);
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    if (m_table[index].empty()) {
        m_table[index].push_front(hashEntry);
        m_count++;
        if ((static_cast<float>(m_count) / static_cast<float>(m_size)) > m_load_factor) rehash();
        return true;
    }

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return keyEqual(key_, hashEntry.m_key);
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator == m_table[index].end()) {
        m_table[index].push_front(hashEntry);
        m_count++;
        if ((static_cast<float>(m_count) / static_cast<float>(m_size)) > m_load_factor) rehash();
        return true;
    }

    *iterator = hashEntry;
    if ((static_cast<float>(m_count) / static_cast<float>(m_size)) > m_load_factor) rehash();
    return false;
}

/**
 * @brief Clears all memory associated with collision lists from the table by removing all its elements.
 *
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear() {
    for (std::size_t index{0}; index < m_size; index++) m_table[index].clear();
    m_count = 0;
}

/**
 * @brief Tests whether the table is empty.
 *
 * @return True is table is empty, false otherwise.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const {
    return m_count == 0;
}

/**
 * @brief Retrieves a data item from the table, based on the key associated with the data.
 *
 * @param key_ Data key to search for in the table.
 * @param data_item_ Data record to be filled in when data item is found.
 * @return True if the data item is found, false otherwise.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve(const KeyType& key_, DataType& data_item_) const {
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return keyEqual(key_, hashEntry.m_key);
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator != m_table[index].end()) {
        data_item_ = iterator->m_data;
        return true;
    }

    return false;
}

/**
 * @brief It is a private method that must be called when the load factor λ is greater than m_load_factor. This method
 * will create a new table whose size will be equal to smaller prime number than twice the size of the table before the
 * rehash() call.
 *
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash(void) {
    auto _old_m_size = m_size;
    m_size = find_next_prime(2 * m_size);
    std::unique_ptr<list_type[]> _old_m_table = std::move(m_table);
    m_table = std::make_unique<list_type[]>(m_size);
    m_count = 0;

    for (std::size_t index{0}; index < _old_m_size; index++) {
        for (const auto& e : _old_m_table[index]) insert(e.m_key, e.m_data);
        _old_m_table[index].~forward_list();
    }

    _old_m_table.reset();
}

/**
 * @brief Removes a table item identified by its data key.
 *
 * @param key_ Data key.
 * @return If the key is found the method returns true, false otherwise.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::erase(const KeyType& key_) {
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return keyEqual(key_, hashEntry.m_key);
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator != m_table[index].end()) {
        m_table[index].remove_if(predicate);
        m_count--;
        return true;
    }

    return false;
}

/**
 * @brief Find the next prime greater than number.
 *
 * @param n_ smaller than the next prime number.
 * @return Prime number.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
std::size_t HashTbl<KeyType, DataType, KeyHash, KeyEqual>::find_next_prime(size_type n_) {
    while (!is_prime(n_)) n_++;
    return n_;
}

/**
 * @brief Verify whether number is prime.
 *
 * @param number Number that will be checked for prime.
 * @return True if it is prime, false otherwise.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::is_prime(const size_type& number) {
    if (number == 2 || number == 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;

    size_type divisor = 6;
    while (divisor * divisor - 2 * divisor + 1 <= number) {
        if (number % (divisor - 1) == 0) return false;
        if (number % (divisor + 1) == 0) return false;
        divisor += 6;
    }

    return true;
}

/**
 * @brief Returns the number of table elements that are in the collision list associated with data key.
 *
 * @param key_ Data key.
 * @return Number of elements inside collision list.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
typename HashTbl<KeyType, DataType, KeyHash, KeyEqual>::size_type HashTbl<KeyType, DataType, KeyHash, KeyEqual>::count(
    const KeyType& key_) const {
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return keyEqual(key_, hashEntry.m_key);
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator == m_table[index].end()) return 0;

    return std::distance(m_table[index].begin(), m_table[index].end());
}

/**
 * @brief Returns a reference to the data associated with the given data key. If the key does not is in the table, the
 * method throws an exception of type std::out_of_range.
 *
 * @param key_ Data key of the element to find.
 * @return Reference to the data of the requested element.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at(const KeyType& key_) {
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return keyEqual(key_, hashEntry.m_key);
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator != m_table[index].end()) return iterator->m_data;

    throw std::out_of_range("out_of_range");
}

/**
 * @brief Returns a reference to the data associated with the given data key, performing an insertion if such key does
 * not already exist.
 *
 * @param key_ Data key of the element to find.
 * @return Returns a reference to the data associated with the data key provided, if exist. If the key is not in the
 * table, return the reference for the data just inserted into the table.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[](const KeyType& key_) {
    KeyHash hashFunc;
    KeyEqual keyEqual;
    auto index{hashFunc(key_) % m_size};

    auto predicate = [&key_, &keyEqual](const HashEntry<KeyType, DataType> hashEntry) {
        return key_ == hashEntry.m_key;
    };
    auto iterator = std::find_if(m_table[index].begin(), m_table[index].end(), predicate);

    if (iterator != m_table[index].end()) return iterator->m_data;

    insert(key_, DataType{});

    return this->operator[](key_);
}

/**
 * @brief Returns the maximum load factor value.
 *
 * @return Current maximum load factor.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
float HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor() const {
    return m_load_factor;
}

/**
 * @brief Sets the maximum load factor.
 *
 * @param mlf New maximum load factor setting.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor(float mlf) {
    m_load_factor = mlf;
}

}  // Namespace ac.
