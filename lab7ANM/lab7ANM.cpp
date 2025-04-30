#include <iostream>
#include <vector>
#include <numeric>
#include <random>
// #include <pair>
#include <map>

enum State
{
    NotFound,
    JustFound,
    Proceed
};

struct ResultEntry
{
    std::vector<int> a;
    std::vector<int> b;
};

std::ostream& operator<<(std::ostream& out, std::vector<int> arr)
{
    for (size_t i = 0; i < arr.size(); ++i)
    {
        out << arr[i];

        if (i != arr.size() - 1)
        {
            out << ", ";
        }
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, std::map<int, ResultEntry*> map)
{
    for (std::pair<int, ResultEntry*> m : map)
    {
        out << m.first;
        out << ": " << std::endl;
        out << "Indexes in the first array: ";
        out << m.second->a << std::endl;
        out << "Indexes in the second array: ";
        out << m.second->b << std::endl;
    }

    return out;
}


int partition(std::vector<int>& arr, int l, int r)
{
    int i = l;
    int j = r;

    int pivot = arr[std::floor((l + r) / 2)]; // pivot - key

     while (true)
    {
        while (arr[i] < pivot)
        {
            i++;
        }

        while (pivot < arr[j])
        {
            j--;
        }

        if (i >= j)
        {
            // std::cout << arr << std::endl;
            return j;
        }

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;

        i++;
        j--;
    };
}

void quick_sort(std::vector<int>& arr, int l, int r)
{
    if (l < r)
    {
        int p = partition(arr, l, r);
        quick_sort(arr, l, p);
        quick_sort(arr, p + 1, r);
    }
}

int linearSearch(std::vector<int> arr, int key, int& iterationCount)
{
    arr.push_back(key);
    int i = 0;

    while (arr[i] != key)
    {
        iterationCount++;
        i++;
    }

    return i;
}

int binarySearch(std::vector<int> arr, int key, int& iterationCount)
{
    int l = 0;
    int r = arr.size() - 1;

    while (l <= r)
    {
        iterationCount++;
        int m = abs((l + r) / 2);

        if (arr[m] == key)
        {
            return m;
        }

        arr[m] < key ? l = m + 1 : r = m - 1;
    }
}

void  calculateEfficiency(double& iterationsLinear, double& iterationsBinary)
{
    std::vector<int> v(1024);
    std::iota(std::begin(v), std::end(v), 1);
    
    int i = 0;
    int sumIterationsBinary = 0;
    int sumIterationsLinear = 0;

    while(i < 1000)
    {
        // random number generation
        std::random_device seed;
        std::mt19937 gen{ seed() };
        std::uniform_int_distribution<> dist{ 1, 1024 };
        int guess = dist(gen); 

        binarySearch(v, guess, sumIterationsBinary);
        linearSearch(v, guess, sumIterationsLinear);

        i++;
    }

    iterationsLinear = (static_cast<double>(sumIterationsLinear)) / 1000;
    iterationsBinary = (static_cast<double>(sumIterationsBinary)) / 1000;
}

std::vector<std::pair<int, int>> createIndexedSorted(const std::vector<int>& vector1)
{
    std::vector<std::pair<int, int>> v; // tuple solutions is better, yet vector<vector> is a better demonstration (tuple === kortez)

    for (int i = 0; i < vector1.size(); i++)
    {
        v.emplace_back(i, vector1[i]);
    }

    std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.second < b.second; });
    
    return v;
}

std::map<int, ResultEntry*> finiteStateMachine(const std::vector<int>& vector1, const std::vector<int>& vector2)
{
    std::map<int, ResultEntry*> result;
    std::vector<std::pair<int, int>> v1 = createIndexedSorted(vector1); 
    std::vector<std::pair<int, int>> v2 = createIndexedSorted(vector2);

    int p = 0, q = 0;
    State state = NotFound;

    int v1IndexTemp;
    int v2IndexTemp;
    int tempValue = NULL;

    while (p < vector1.size() and q < vector2.size())
    {
        if (v1[p].second == v2[q].second)
        {
            if (state != NotFound and tempValue != v1[p].second)
            {
                state = NotFound;
            }

            if (state == NotFound)
            {
                state = JustFound;

                v1IndexTemp = v1[p].first;
                v2IndexTemp = v2[q].first;
                tempValue = v1[p].second;
            }
            else if (state == JustFound)
            {
                ResultEntry* resultEntry = new ResultEntry;
                resultEntry->a = std::vector<int> { v1IndexTemp, v1[p].first };
                resultEntry->b = std::vector<int>{ v2IndexTemp, v2[q].first };

                result.insert({ tempValue, resultEntry });
                state = Proceed;
            }
            else
            {
                auto entry = result[tempValue];

                entry->a.push_back(v1[p].first);
                entry->b.push_back(v2[q].first);
            }

            q++;
            p++;
            continue;
        }

        if (tempValue != NULL and tempValue == v1[p].second and state == Proceed)
        {
            auto entry = result[tempValue];

            entry->a.push_back(v1[p].first);
        }

        if (tempValue != NULL and tempValue == v2[q].second and state == Proceed)
        {
            auto entry = result[tempValue];

            entry->b.push_back(v2[q].first);
        }

        if (v1[p].second < v2[q].second)
        {
            p++;
            state = NotFound;
            continue;
        }

        if (v2[q].second < v1[p].second)
        {
            q++;
            state = NotFound;
            continue;
        }
    }

    while (p < vector1.size())
    {
        if (tempValue != NULL and tempValue == v1[p].second)
        {
            auto entry = result[tempValue];

            entry->a.push_back(v1[p].first);
        }

        p++;
    }

    while (q < vector2.size())
    {
        if (tempValue != NULL and tempValue == v2[q].second)
        {
            auto entry = result[tempValue];

            entry->a.push_back(v2[q].first);
        }

        q++;
    }

    return result;
}

int main()
{
    std::vector<int> v(1000); 
    std::iota(std::begin(v), std::end(v), 0);

    double iterationsBinary = 0;
    double iterationsLinear = 0;

    calculateEfficiency(iterationsLinear, iterationsBinary);

    std::cout << "Linear efficiency: " << iterationsLinear << std::endl;
    std::cout << "Binary efficiency: " << iterationsBinary << std::endl;

    std::vector<int> a = { 1, 11, 12, 15, 12, 11 };
    std::vector<int> b = { 11, 11, 3, 90, 12, 57, 90, 12, 1, 12 };

    // time complexity - O(n*logn)
    // ! HASH functions solution is more efficient!!!
    auto result = finiteStateMachine(a, b);

    std::cout << result;

    for (std::pair<int, ResultEntry*> m : result)
    {
        delete m.second;

        //1
    }
}
