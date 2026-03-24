//
// Created by nexie on 09.11.2025.
//

#ifndef NX_CORE_CONNECTION_HPP
#define NX_CORE_CONNECTION_HPP

#include <unordered_map>

#include <nx/core/detail/function_id.hpp>
#include <nx/core/object/Signal.hpp>


namespace nx {

    class Object;
    namespace detail
    {
        size_t hash ( void * o, void * m );
        size_t hash ( void * o, detail::function_id m );
    }

    class Connection
    {
    public:
        using id = size_t;
        static id MakeId ( void * sender, detail::function_id signal, void * receiver, detail::function_id slot );

        enum Type : uint8_t
        {
            Auto            = 0x00,
            Direct          = 0x01,
            Queued          = 0x02,
        };

        enum Flag : uint8_t
        {
            NoFlags         = 0x00,
            Unique          = 0x10,
            SingleShot      = 0x20,
            Blocking        = 0x40,
        };

        Connection (FunctorPtr functor,
            void * sender, detail::function_id signal,
            void * receiver, detail::function_id slot,
            Type type, uint8_t flags,
            bool object_receiver = false);

        template<typename... Args>
        void transmit (Args&&...);


        [[nodiscard]] bool isUnique () const;
        [[nodiscard]] bool isSingleShot () const;
        [[nodiscard]] bool isBlocking () const;
        [[nodiscard]] void * getSender () const;
        [[nodiscard]] void * getReceiver () const;
        [[nodiscard]] size_t getSenderId () const;
        [[nodiscard]] size_t getId () const;

    protected:
        void _transmitImpl (Signal && s);

        FunctorPtr functor;
        void * sender;
        detail::function_id signal;
        void * receiver;
        detail::function_id slot;

        bool object_receiver;
        Type type;
        uint8_t flags;
    };

    template <typename... Args>
    void Connection::transmit (Args&&... args)
    {
        if (object_receiver)
        {
            auto r = static_cast <Object *> (receiver);
            auto s = static_cast <Object *> (sender);
            return _transmitImpl (Signal(s, r, functor, std::forward<Args>(args)...));
        }
        return _transmitImpl (Signal(nullptr, nullptr, functor, std::forward<Args>(args)...));
    }




}
#endif //CONNECTION_HPP
