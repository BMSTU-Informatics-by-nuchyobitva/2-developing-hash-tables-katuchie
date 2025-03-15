#include <iostream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

class Cat {
public:
    string name;
    int weight;

    Cat(string name, int weight) : name(name), weight(weight) {}

    Cat()
    {
        this->name = "";
        this->weight = 0;
    }
};


class HashTable {
private:
    vector<Cat> table;
    size_t capacity; //размер хэш-таблицы
    size_t itemCount;//текущая заполненность таблицы
    int hashFunction( Cat cat)
    {
        //хэш-функция на основе битовых операций
        int hash = 0;
        for (char c : cat.name)
        {
            hash = (hash * 5) + c;
        }

        hash = (hash << 5) ^ (hash >> 27);
        hash ^= cat.weight * 17;
       return hash;
    }

    int hash2(Cat cat) {
        return 7 - (hashFunction(cat) % 7); // Вторая хеш-функция, никогда не ноль
    }

    int findSlot( Cat cat)  {
        size_t index = hashFunction(cat) % capacity;
        size_t step = hash2(cat);

        for (size_t i = 0; i < capacity; ++i) {
            size_t helpIndex = (index + i * step) % capacity;//Первая итерация цикла, сразу пытаемся ткнуться по полю index, тк. i=0 и step занулится.
            if (table[helpIndex].name.empty()) {
                return helpIndex;
            }
            if (table[helpIndex].name== cat.name && table[helpIndex].weight==cat.weight) {//Если верхний вариант не выгорел, применяем другой индекс, найденый с использованием 2-ой хэш функции
                return helpIndex;
            }
        }
        return capacity; // Таблица заполнена
    }

    void rehash() {
        size_t newCapacity = capacity * 2;//доступный размер увеличиваем в два
        vector<Cat> newTable(newCapacity);
        capacity = newCapacity;
        itemCount = 0;
        vector<Cat> oldTable = table;
        table = newTable;

        //Перекидываем котов в новую расширенную табличку
        for (Cat cat : oldTable) {
            if (!cat.name.empty()) {
                insert(cat);
            }
        }
    }

public:
    HashTable(size_t initialCapacity = 3) : capacity(initialCapacity), table(initialCapacity), itemCount(0) {
        for (int i = 0; i < capacity; i++) {
            table[i] = Cat("", 0);
        }
    }

    ~HashTable() {}

    void insert(Cat cat) {
        size_t index = findSlot(cat);
        if (index < capacity) {
            table[index] = cat;
            itemCount++;
            if (itemCount > capacity / 2) {
                rehash();
            }
        }
    }

    Cat search(Cat cat) {
        size_t index = hashFunction(cat) % capacity;
        size_t step = hash2(cat);

        for (size_t i = 0; i < capacity; ++i) {
            size_t helpIndex = (index + i * step) % capacity;
            if (table[helpIndex].name.empty()) {
                return Cat("", 0);
            }
            if (table[helpIndex].name == cat.name && table[helpIndex].weight == cat.weight) {
                return table[helpIndex];
            }
        }
        return Cat("", 0);
    }

    bool remove(Cat cat) {
        size_t index = hashFunction(cat) % capacity;
        size_t step = hash2(cat);

        for (size_t i = 0; i < capacity; ++i) {
            size_t helpIndex = (index + i * step) % capacity;
            if (table[helpIndex].name.empty()) {
                return false;
            }
            if (table[helpIndex].name == cat.name && table[helpIndex].weight == cat.weight) {
                table[helpIndex] = Cat("", 0);
                itemCount--;
                return true;
            }
        }
        return false; 
    }

    void print(bool withEmpties) //Функция принта с возможностью не выводить пустые
    {
        for (size_t i = 0; i < capacity; i++) {
            if (!table[i].name.empty()) {
                cout << "Индекс " << i << ": (" << table[i].name << ", " << table[i].weight << ")" << endl;
            }
            else {
                if (withEmpties)
                cout << "Индекс " << i << ": Пустор" << endl;
            }
        }
    }

};

int main() {

    setlocale(LC_ALL, "Russian");
    HashTable hashTable(10);

    Cat cat1("Барсик", 5);
    Cat cat2("Мурзик", 7);
    Cat cat3("Автобус", 6);
    Cat cat4("Барсик", 5);

    hashTable.insert(cat1);
    hashTable.insert(cat2);
    hashTable.insert(cat3);


    cout << "Таблица после инсЁртов:" << endl;
    hashTable.print(true);

    Cat foundCat = hashTable.search(cat4);
    if (!foundCat.name.empty()) {
        cout << "Найденный кот: " << foundCat.name << ", " << foundCat.weight << endl;
    }
    else {
        cout << "Не найдено" << endl;
    }

    if (hashTable.remove(cat1)) {
        cout << "Удалённый кот: " << cat1.name << endl;
    }
    else {
        cout << "Кот не найден" << endl;
    }

    cout << "Таблица после удаления кота" << endl;
    hashTable.print(false);

    return 0;
}