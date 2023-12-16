#pragma once

#include <vector>
#include <string>
#include <cassert>

using namespace std;
using namespace madlib;

class VectorTest {
protected:

    struct Point {
        int x, y;
    };

    // Define a function to compare custom objects (Point in this case)
    static bool comparePoints(const Point& p1, const Point& p2) {
        return (p1.x == p2.x && p1.y == p2.y);
    }

    static void cleanup() {
        remove("numbers.dat");
        remove("doubles.dat");
        remove("points.dat");
        remove("numbers_ref.dat");
    }

public:

    static void test_vector_create_destroy() {

        struct Foo {
            int data;
            explicit Foo(int data): data(data) {}
        };

        vector<Foo*> foos;
        vector_create(foos, 123);
        vector_create(foos, 234);
        vector_create(foos, 345);
        assert(foos[0]->data == 123);
        assert(foos[1]->data == 234);
        assert(foos[2]->data == 345);
        vector_destroy(foos);
        assert(foos.size() == 0);
    }

    static void testVector_concat() {
        cleanup();

        // Test case 1
        vector<string> words1 = {"Hello", "world"};
        string result1 = vector_concat(words1, " ");
        assert(result1 == "Hello world");

        // Test case 2
        vector<string> words2 = {"One", "Two", "Three"};
        string result2 = vector_concat(words2, "-");
        assert(result2 == "One-Two-Three");

        // Test case 3: Empty vector
        vector<string> emptyVector;
        string result3 = vector_concat(emptyVector, " ");
        assert(result3.empty());

        // Test case 4: Concatenating with an empty separator
        vector<string> words4 = {"A", "B", "C"};
        string result4 = vector_concat(words4, "");
        assert(result4 == "ABC");

        // Test case 5: Concatenating with a longer separator
        vector<string> words5 = {"X", "Y", "Z"};
        string result5 = vector_concat(words5, "123");
        assert(result5 == "X123Y123Z");

        // Test case 6: Concatenating with numbers
        vector<string> numbers = {"1", "2", "3"};
        string result6 = vector_concat(numbers, ",");
        assert(result6 == "1,2,3");

        cleanup();
    }

    static void testVector_save_and_load() {
        cleanup();

        // Test case 1: Saving and loading integers
        vector<int> numbers1 = {1, 2, 3, 4, 5};
        vector_save("numbers.dat", numbers1);
        vector<int> loadedNumbers1 = vector_load<int>("numbers.dat");
        assert(vector_compare<int>(numbers1, loadedNumbers1));

        // Test case 2: Saving and loading doubles
        vector<double> doubles = {1.1, 2.2, 3.3, 4.4, 5.5};
        vector_save("doubles.dat", doubles);
        vector<double> loadedDoubles = vector_load<double>("doubles.dat");
        assert(vector_compare<double>(doubles, loadedDoubles));

        // Test case 3: Saving and loading custom objects (struct)
        vector<Point> points = {{1, 2}, {3, 4}, {5, 6}};
        vector_save("points.dat", points);
        vector<Point> loadedPoints = vector_load<Point>("points.dat");
        assert(vector_compare<Point>(points, loadedPoints, comparePoints));

        cleanup();
    }

    static void testVector_load_and_load_with_reference() {
        cleanup();

        // Test case 1: Loading integers
        vector<int> savedNumbers = {1, 2, 3, 4, 5};
        vector_save("numbers.dat", savedNumbers);

        vector<int> loadedNumbers;
        vector_load("numbers.dat", loadedNumbers);
        assert(vector_compare(savedNumbers, loadedNumbers));

        // Test case 2: Loading doubles
        vector<double> savedDoubles = {1.1, 2.2, 3.3, 4.4, 5.5};
        vector_save("doubles.dat", savedDoubles);

        vector<double> loadedDoubles;
        vector_load("doubles.dat", loadedDoubles);
        assert(vector_compare(savedDoubles, loadedDoubles));

        // Test case 3: Loading custom objects (struct)
        vector<Point> savedPoints = {{1, 2}, {3, 4}, {5, 6}};
        vector_save("points.dat", savedPoints);
        vector<Point> loadedPoints;
        vector_load("points.dat", loadedPoints);
        assert(vector_compare(savedPoints, loadedPoints, comparePoints));

        // Test case 5: Loading with reference (using same data)
        vector<int> savedNumbersRef = {9, 8, 7, 6, 5};
        vector_save("numbers_ref.dat", savedNumbersRef);
        vector<int> loadedNumbersRef = {1, 2, 3}; // Pre-existing data
        vector_load("numbers_ref.dat", loadedNumbersRef);
        assert(vector_compare(savedNumbersRef, loadedNumbersRef));

        cleanup();
    }
};
