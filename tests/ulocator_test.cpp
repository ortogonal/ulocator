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
#include <gtest/gtest.h>
#include <ulocator/servicelocator.h>

class IFoo
{
public:
    virtual ~IFoo() = default;

    virtual int sum(int a, int b) const = 0;
};

class Foo : public IFoo
{
public:
    int sum(int a, int b) const override
    {
        return a + b;
    }
};

TEST(uLocator, Resolve)
{
    auto sl = ServiceLocator::create();

    EXPECT_EQ(sl->canResolve<int>(), false);
    EXPECT_EQ(sl->canResolve<bool>(), false);
    EXPECT_EQ(sl->canResolve<float>(), false);
    EXPECT_EQ(sl->canResolve<std::string>(), false);
    EXPECT_EQ(sl->canResolve<IFoo>(), false);
    EXPECT_EQ(sl->resolve<int>(), nullptr);
    EXPECT_EQ(sl->resolve<bool>(), nullptr);
    EXPECT_EQ(sl->resolve<float>(), nullptr);
    EXPECT_EQ(sl->resolve<std::string>(), nullptr);
    EXPECT_EQ(sl->resolve<IFoo>(), nullptr);

    sl->bind<IFoo>()->to<Foo>();
    EXPECT_EQ(sl->canResolve<IFoo>(), true);
    auto foo = sl->resolve<IFoo>();
    EXPECT_TRUE(foo != nullptr);

    if (foo) {
        EXPECT_EQ(foo->sum(2, 3), 5);
    }
}
