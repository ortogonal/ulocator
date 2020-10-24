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
#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <typeindex>

class ServiceLocator
{
public:

    static constexpr auto version = "0.0.1";

    class InterfaceClause;

    /**
     * @brief The InterfaceCreator class More or less dummy class for inheritance
     */
    class InterfaceCreator
    {
    public:
        virtual ~InterfaceCreator() = default;
    };

    /**
     * @brief The TypedInterfaceCreator class Internal class that hold creator and data
     *
     * TypeInterfaceCreator is a class that is attached to each interface that
     * is added to the ServiceLocator. It holds pointer to the created object but can also
     * hold a pointer to a creator class that will be used to create the object
     * when someone does a resolve.
     */
    template<class TIfc>
    class TypedInterfaceCreator : public InterfaceCreator
    {
    public:

        /**
         * @brief The ToClause class Internal class to temporay hold correct type
         *
         * This class is used to bind an interface against an implementation. There
         * are three different ways to use use this class.
         */
        class ToClause
        {
        public:
            ToClause(std::shared_ptr<ServiceLocator> sl)
                : m_sl(sl)
            {}

            template<class TImpl>
            void to(std::function<std::shared_ptr<TImpl>(std::shared_ptr<ServiceLocator>)> createFunc)
            {
                auto typeIndex = std::type_index(typeid(TIfc));

                if (m_sl->m_creators.find(typeIndex) == m_sl->m_creators.end()) {
                    auto tIfcCreate = std::make_unique<TypedInterfaceCreator<TIfc>>();
                    tIfcCreate->creator = createFunc;
                    m_sl->m_creators[typeIndex] = std::move(tIfcCreate);
                } else {
                    // TODO
                    std::cerr << "Hmm what to do here?" << std::endl;
                }
            }

            template<class TImpl>
            void to(std::shared_ptr<TImpl> ptr)
            {
                auto typeIndex = std::type_index(typeid(TIfc));

                if (m_sl->m_creators.find(typeIndex) == m_sl->m_creators.end()) {
                    auto tIfcCreate = std::make_unique<TypedInterfaceCreator<TIfc>>();
                    tIfcCreate->ptr = ptr;
                    m_sl->m_creators[typeIndex] = std::move(tIfcCreate);
                } else {
                    // TODO
                    std::cerr << "Hmm what to do here?" << std::endl;
                }
            }

            template<class TImpl>
            void to()
            {
                auto typeIndex = std::type_index(typeid(TIfc));

                if (m_sl->m_creators.find(typeIndex) == m_sl->m_creators.end()) {
                    auto tIfcCreate = std::make_unique<TypedInterfaceCreator<TIfc>>();
                    tIfcCreate->ptr = std::make_shared<TImpl>();
                    m_sl->m_creators[typeIndex] = std::move(tIfcCreate);
                } else {
                    // TODO
                    std::cerr << "Hmm what to do here?" << std::endl;
                }
            }

        private:
            std::shared_ptr<ServiceLocator> m_sl;

            friend class ServiceLocator;
        };

        virtual ~TypedInterfaceCreator() = default;

        std::function<std::shared_ptr<TIfc>(std::shared_ptr<ServiceLocator>)> creator;
        std::shared_ptr<TIfc> ptr;
    };

    class Interface
    {
    public:
        virtual void load() = 0;

        template<class T>
        std::shared_ptr<typename TypedInterfaceCreator<T>::ToClause> bind()
        {
            return std::make_shared<typename TypedInterfaceCreator<T>::ToClause>(m_sl);
        }

    private:
        std::shared_ptr<ServiceLocator> m_sl;

        friend class InterfaceClause;
        friend class ServiceLocator;
    };

    // Builder pattern
    class InterfaceClause
    {
    public:
        InterfaceClause() = delete;
        InterfaceClause(const InterfaceClause &) = delete;
        InterfaceClause(InterfaceClause &&) = delete;

        InterfaceClause(std::shared_ptr<ServiceLocator> sl)
            : m_sl(sl)
        {}

        template<class T>
        InterfaceClause &add()
        {
            auto interface = std::make_unique<T>();
            interface->m_sl = m_sl;
            interface->load();

            return *this;
        }

    private:
        std::shared_ptr<ServiceLocator> m_sl;
    };


    ServiceLocator(const ServiceLocator &) = delete;
    ServiceLocator(ServiceLocator &&) = delete;

    InterfaceClause &interfaces()
    {
        if (m_interfaceClause == nullptr) {
            m_interfaceClause = std::make_shared<InterfaceClause>(m_this.lock());
        }
        return *m_interfaceClause;
    }

    static std::shared_ptr<ServiceLocator> create()
    {
        auto sl = std::shared_ptr<ServiceLocator>(new ServiceLocator());
        sl->m_this = sl;
        return sl;
    }

    template<class TIfc>
    std::shared_ptr<typename TypedInterfaceCreator<TIfc>::ToClause> bind()
    {
        return std::make_shared<typename TypedInterfaceCreator<TIfc>::ToClause>(m_this.lock());
    }


    template<class TIfc>
    std::shared_ptr<TIfc> resolve()
    {
        auto typeIndex = std::type_index(typeid(TIfc));
        auto creator = m_creators.find(typeIndex);
        if (creator != m_creators.end()) {
            auto typedCreator = dynamic_cast<TypedInterfaceCreator<TIfc> *>(creator->second.get());
            if (typedCreator) {
                if (typedCreator->ptr == nullptr) {
                    auto ptr = typedCreator->creator(m_this.lock());
                    typedCreator->ptr = ptr;
                }
                return typedCreator->ptr;
            } else {
                std::cerr << "Cannot cast " << std::type_index(typeid(creator->second.get())).name()
                          << std::endl;
            }
        }
        return nullptr;
    }

    template<class TIfc>
    bool canResolve()
    {
        auto typeIndex = std::type_index(typeid(TIfc));
        return m_creators.find(typeIndex) != m_creators.end();
    }

private:
    ServiceLocator(){};

    std::shared_ptr<InterfaceClause> m_interfaceClause;
    std::weak_ptr<ServiceLocator> m_this;

    std::map<std::type_index, std::unique_ptr<InterfaceCreator>> m_creators;
};
