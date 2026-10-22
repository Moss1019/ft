#include <cassert>

class Adder
{
public:
    int add(int n1, int n2) { return n1 + n2; }
};

void test_adder()
{
    Adder adder;
    
    // Test 1 + 2 = 3
    assert(adder.add(1, 2) == 3);
    
    // Additional basic tests
    assert(adder.add(0, 0) == 0);
    assert(adder.add(-1, 1) == 0);
    assert(adder.add(100, 200) == 300);
}

int main()
{
    test_adder();
    return 0;
}
