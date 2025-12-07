#include "simple_vector.h"

#include <cassert>
#include <iostream>

using namespace std;

ReserveProxyObj Reserve(size_t capacity) {
    return ReserveProxyObj(capacity);
}

void TestReserveConstructor() {
    cout << "TestReserveConstructor"s << endl;
    SimpleVector<int> v(Reserve(5));
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> v;
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());

    v.Reserve(1);
    assert(v.GetCapacity() == 5);
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    v.Reserve(100);
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
    cout << "Done!"s << endl;
}

int main() {
    TestReserveConstructor();
    TestReserveMethod();
}