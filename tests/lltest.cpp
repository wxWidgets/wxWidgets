#include "wx/longlong.h"

#include <time.h>
#include <stdlib.h>
#if defined(__MWERKS__) && macintosh
#include <SIOUX.h>
#include <Events.h>
#endif

static const unsigned long long NumberOfTests = 100000ll;

//using namespace std;
int oddbits(unsigned int number);

int oddbits(unsigned int number)
{
    int sum = 0;
    while(number > 0)
    {
        sum += number & 1;
        number >>= 1;
    }

    return sum & 1;
}
void random(long& hi, unsigned long& lo);

void random(long& hi, unsigned long& lo)
{
    hi = 0;
    lo = 0;

    for (int i = 0; i < 32; i++)
    {
        hi |= oddbits(rand()) << i;
        lo |= oddbits(rand()) << i;
    }
}

int shifttest();

int shifttest()
{
    std::cout << std::endl << "Starting shift tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongWx l1c(hi, lo);
        wxLongLongWx l1b;
        wxLongLongNative l2a(hi, lo);
        wxLongLongNative l2c(hi, lo);
        wxLongLongNative l2b;

        int shift = rand() % 64;
        int leftshift = oddbits(rand());

        if (leftshift)
        {
            l1b = l1a << shift;
            l2b = l2a << shift;
            l1c <<= shift;
            l2c <<= shift;
        }
        else
        {
            l1b = l1a >> shift;
            l2b = l2a >> shift;
            l1c >>= shift;
            l2c >>= shift;
        }

        void *oneb = l1b.asArray();
        void *twob = l2b.asArray();
        void *onec = l1c.asArray();
        void *twoc = l2c.asArray();

        if ((memcmp(oneb, twob, 8) != 0) || (memcmp(onec, twoc, 8) != 0) || (memcmp(oneb, onec, 8) != 0))
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            if (leftshift)
                std::cout << "<< ";
            else
                std::cout << ">> ";
            std::cout << shift << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

int add1test();

int add1test()
{
    std::cout << std::endl << "Starting first addition tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1d(hi, lo);
        wxLongLongNative l2d(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);
        wxLongLongWx l1c;
        wxLongLongNative l2c;

        l1c = l1a + l1b;
        l2c = l2a + l2b;
        l1d += l1b;
        l2d += l2b;

        void *onec = l1c.asArray();
        void *twoc = l2c.asArray();
        void *oned = l1d.asArray();
        void *twod = l2d.asArray();

        if ((memcmp(onec, twoc, 8) != 0) || (memcmp(oned, twod, 8) != 0) || (memcmp(onec, oned, 8) != 0))
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

/*
int add2test();

int add2test()
{
    std::cout << std::endl << "Starting second addition tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1c(hi, lo);
        wxLongLongNative l2c(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);

        l1b += l1a;
        l2b += l2a;

        void *one = l1b.asArray();
        void *two = l2b.asArray();

        if (memcmp(one, two, 8) != 0)
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}
*/

int sub1test();

int sub1test()
{
    std::cout << std::endl << "Starting first subtraction tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1d(hi, lo);
        wxLongLongNative l2d(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);
        wxLongLongWx l1c;
        wxLongLongNative l2c;

        l1c = l1a - l1b;
        l2c = l2a - l2b;
        l1d -= l1b;
        l2d -= l2b;

        void *onec = l1c.asArray();
        void *twoc = l2c.asArray();
        void *oned = l1d.asArray();
        void *twod = l2d.asArray();

        if ((memcmp(onec, twoc, 8) != 0) || (memcmp(oned, twod, 8) != 0) || (memcmp(onec, oned, 8) != 0))
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            std::cout << l1d << std::endl;
            std::cout << l2d << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

/*
int sub2test();

int sub2test()
{
    std::cout << std::endl << "Starting second subtraction tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1c(hi, lo);
        wxLongLongNative l2c(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);

        l1b -= l1a;
        l2b -= l2a;

        void *one = l1b.asArray();
        void *two = l2b.asArray();

        if (memcmp(one, two, 8) != 0)
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

*/

int incdectest();

int incdectest()
{
    std::cout << std::endl << "Starting pre/post increment/decrement tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);

        int increment = oddbits(rand());
        int pre = oddbits(rand());

        if (increment)
            if (pre)
            {
                ++l1b;
                ++l2b;
            }
            else
            {
                l1b++;
                l2b++;
            }
        else
            if (pre)
            {
                --l1b;
                --l2b;
            }
            else
            {
                l1b--;
                l2b--;
            }

        void *one = l1b.asArray();
        void *two = l2b.asArray();

        if (memcmp(one, two, 8) != 0)
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << (pre ? "pre" : "post") << (increment ? "increment" : "decrement") << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

int negationtest();

int negationtest()
{
    std::cout << std::endl << "Starting negation tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1b(-l1a);
        wxLongLongNative l2b(-l2a);

        void *one = l1b.asArray();
        void *two = l2b.asArray();

        if (memcmp(one, two, 8) != 0)
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

int multiplicationtest();

int multiplicationtest()
{
    std::cout << std::endl << "Starting multiplication tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1d(hi, lo);
        wxLongLongNative l2d(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);

        wxLongLongWx l1c(l1a * l1b);
        wxLongLongNative l2c(l2a * l2b);

        l1d *= l1b;
        l2d *= l2b;

        void *onec = l1c.asArray();
        void *twoc = l2c.asArray();
        void *oned = l1c.asArray();
        void *twod = l2c.asArray();

        if ((memcmp(onec, twoc, 8) != 0) || (memcmp(oned, twod, 8) != 0) || (memcmp(onec, oned, 8) != 0))
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            std::cout << l1d << std::endl;
            std::cout << l2d << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

int bitwisetest();

int bitwisetest()
{
    std::cout << std::endl << "Starting bitwise tests." << endl;

    long hi = 0;
    unsigned long lo = 0;

    unsigned wxLongLong_t counter = 0;

    while (counter < NumberOfTests)
    {
        if ((counter % 1000) == 999)
        {
            std::cerr << "+";
#if defined(__MWERKS__) && macintosh
            GetNextEvent(0, NULL);
#endif
        }

        random(hi, lo);
        wxLongLongWx l1a(hi, lo);
        wxLongLongNative l2a(hi, lo);
        wxLongLongWx l1d(hi, lo);
        wxLongLongNative l2d(hi, lo);
        random(hi, lo);
        wxLongLongWx l1b(hi, lo);
        wxLongLongNative l2b(hi, lo);

        wxLongLongWx l1c(l1a * l1b);
        wxLongLongNative l2c(l2a * l2b);

        l1d *= l1b;
        l2d *= l2b;

        void *onec = l1c.asArray();
        void *twoc = l2c.asArray();
        void *oned = l1c.asArray();
        void *twod = l2c.asArray();

        if ((memcmp(onec, twoc, 8) != 0) || (memcmp(oned, twod, 8) != 0) || (memcmp(onec, oned, 8) != 0))
        {
            std::cout << std::endl << "After " << counter << " successful trials: " << std::endl;
            std::cout << l1a << std::endl;
            std::cout << l2a << std::endl;
            std::cout << l1b << std::endl;
            std::cout << l2b << std::endl;
            std::cout << l1c << std::endl;
            std::cout << l2c << std::endl;
            std::cout << l1d << std::endl;
            std::cout << l2d << std::endl;
            return 0;
        }
        counter++;
    }

    std::cout << std::endl << counter << " successful trial" << (counter == 1 ? "." : "s.") << std::endl;

    return 1;
}

int main()
{
#if defined(__MWERKS__) && macintosh
    SIOUXSettings.asktosaveonclose = 0;
    SIOUXSettings.showstatusline = 1;
    SIOUXSettings.autocloseonquit = 0;
#endif

    std::cout << "Starting tests." << endl;

#if defined(__MWERKS__) && macintosh
    GetNextEvent(0, NULL);
#endif

    srand(time(NULL));

    if (!multiplicationtest())
        return 1;

    if (!shifttest())
        return 1;

    if (!add1test())
        return 1;

//    if (!add2test())
//        return 1;

    if (!sub1test())
        return 1;

//    if (!sub2test())
//        return 1;

    if (!incdectest())
        return 1;

    if (!negationtest())
        return 1;

    std::cout << std::endl << "The tests are finished." << std::endl;

    return 0;
}
