/*
 * MIT License
 *
 * Copyright (c) 2020 Erik Larsson, karl.erik.larsson@gmail.com
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include <iostream>

#include <ulocator/servicelocator.h>

class IFoo
{
public:
    virtual ~IFoo() = default;

    virtual void doWork() = 0;
};

class IBar
{
public:
    virtual ~IBar() = default;

    virtual void tellFoo() = 0;
};

class ImplFoo : public IFoo
{
public:
    ImplFoo()
    {}
    void doWork() override
    {
        std::cout << "Doing ImpFoo work" << std::endl;
    }
};

class ImplBar : public IBar
{
public:
    ImplBar(std::shared_ptr<IFoo> foo) : m_foo(foo) {}

    void tellFoo() override
    {
        std::cout << "Telling Foo to do its work!" << std::endl;
        if (m_foo) {
            m_foo->doWork();
        }
    }
private:
    std::shared_ptr<IFoo> m_foo;
};

class ImplFooSLInterface : public ServiceLocator::Interface
{
public:
    void load() override {
        bind<IFoo>()->to<ImplFoo>([](std::shared_ptr<ServiceLocator> /*sl*/) {
            return std::make_shared<ImplFoo>();
        });
    }
};

class ImplBarSLInterface : public ServiceLocator::Interface
{
public:
    void load() override {
        bind<IBar>()->to<ImplBar>([](std::shared_ptr<ServiceLocator> sl) {
            return std::make_shared<ImplBar>(sl->resolve<IFoo>());
        });
    }
};

int main()
{
    // Create an empty ServiceLocator
    auto sl = ServiceLocator::create();

    sl->bind<IFoo>()->to<ImplFoo>([](std::shared_ptr<ServiceLocator>) {
        return std::make_shared<ImplFoo>();
    });

    // You can also bind like this
    //sl->bind<IFoo>()->to<ImplFoo>();

    // The in this way
    //auto fooInit = std::make_shared<ImplFoo>();
    //sl->bind<IFoo>()->to<ImplFoo>(fooInit);

    // Add creators for interface IFoo and IBar
    sl->interfaces().add<ImplBarSLInterface>();

    // Resolve implementation for IFoo
    auto foo = sl->resolve<IFoo>();
    if (foo) {
        foo->doWork();
    }

    // Resolve implementation for IBar
    auto bar = sl->resolve<IBar>();
    if (bar) {
        bar->tellFoo();
    }

    return 0;
}
