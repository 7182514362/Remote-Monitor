#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
private:
    Singleton(){}
public:
    static T &getInstance()
    {
        static T instance;
        return instance;
    }
};

#endif // SINGLETON_H
